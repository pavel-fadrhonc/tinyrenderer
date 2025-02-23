#include <array>
#include "shader.h"

#include <assert.h>
#include <complex.h>
#include <iostream>

#include "my_gl.h"

Vec4f IFragmentShader::GetInterpolatedData4(i32 dataHash) const
{
	return GetInterpolatedData(dataHash, m_VaryingData4);
}

Vec3f IFragmentShader::GetInterpolatedData3(i32 dataHash) const 
{
	return GetInterpolatedData(dataHash, m_VaryingData3);
}

Vec2f IFragmentShader::GetInterpolatedData2(i32 dataHash) const
{
	return GetInterpolatedData(dataHash, m_VaryingData2);
}

float IFragmentShader::GetInterpolatedData1(i32 dataHash) const
{
	return GetInterpolatedData(dataHash, m_VaryingData1);
}

Vec2f IFragmentShader::GetVaryingData2(i32 dataHash, u8 vertIndex) const
{
	return GetVaryingData(dataHash, m_VaryingData2, vertIndex);
}

Vec3f IFragmentShader::GetVaryingData3(i32 dataHash, u8 vertIndex) const
{
	return GetVaryingData(dataHash, m_VaryingData3, vertIndex);
}

Vec4f IFragmentShader::GetVaryingData4(i32 dataHash, u8 vertIndex) const
{
	return GetVaryingData(dataHash, m_VaryingData4, vertIndex);
}


void IVertexShader::SetVaryingData4(i32 hash, u8 vertexIdx, const Vec4f& data)
{
	SetVaryingData(hash, vertexIdx, data, m_VaryingData4);
}

void IVertexShader::SetVaryingData3(i32 hash, u8 vertexIdx, const Vec3f& data)
{
	SetVaryingData(hash, vertexIdx, data, m_VaryingData3);
}

void IVertexShader::SetVaryingData2(i32 hash, u8 vertexIdx, const Vec2f& data)
{
	SetVaryingData(hash, vertexIdx, data, m_VaryingData2);
}

void IVertexShader::SetVaryingData1(i32 hash, u8 vertexIdx, const float& data)
{
	SetVaryingData(hash, vertexIdx, data, m_VaryingData1);
}

Vec3f BasicScreenSpace::vertex(u32 faceIdx, u8 vertIdx)
{
	assert(m_Model);

	Vec3f vertexNormal = m_Model->NormalForFaceAndVertex(faceIdx, vertIdx);
	Vec3f vertexNormalNDC = (mgl::MVP_IT * vertexNormal.ToDirection()).ToVec3().normalize();
	SetVaryingData3(NORMAL_NDC_VARYING_DATA_HASH, vertIdx, vertexNormalNDC);

	Vec2f uv = m_Model->UVForFaceAndVertex(faceIdx, vertIdx);
	SetVaryingData2(UV_VARYING_DATA_HASH, vertIdx, uv);

	Vec3f vertex = m_Model->VertexForFace(faceIdx, vertIdx);
	Vec4f positionWS = (mgl::ModelMat * vertex.ToPoint());
	 
	SetVaryingData4(VERTEX_WS_VARYING_DATA_HASH, vertIdx, positionWS);

	Vec4f positionCS = mgl::ProjectionMat * mgl::ViewMat * positionWS;
	SetVaryingData4(VERTEX_CS_VARYING_DATA_HASH, vertIdx, positionCS);

	SetVaryingData3(VERTEX_NDC_VARYING_DATA_HASH, vertIdx, positionCS.FromHomogeneous());

	return (mgl::MVP * vertex.ToPoint()).FromHomogeneous();
}

bool Phong::fragment()
{
	assert(m_AlbedoTexture);

	Vec3f normal = GetInterpolatedData3(NORMAL_NDC_VARYING_DATA_HASH);
	const float NdotL = std::max(normal.dot(mgl::LightDir), 0.0f);

	const auto* tex = m_AlbedoTexture;
	Vec2f uv = GetInterpolatedData2(UV_VARYING_DATA_HASH);
	TGAColor color = tex->get(tex->get_width() * uv.u, tex->get_height() * uv.v);

	m_FinalColor = color.ToFloat() * NdotL;

	return true;
}

bool QuantizeFragmentShader::fragment()
{
	Vec3f normal = GetInterpolatedData3(NORMAL_NDC_VARYING_DATA_HASH);
	const float NdotL = std::max(normal.dot(mgl::LightDir), 0.0f);

	Vec3f finalCol = m_Tint * static_cast<float>(static_cast<int>(NdotL * static_cast<float>(m_Levels)))* (1.0f / static_cast<float>(m_Levels));

	m_FinalColor = Vec4f{ finalCol, 1.0f };

	return true;
}

bool NormalMappedPhongFragmentShader::fragment()
{
	assert(m_NormalTexture);
	assert(m_AlbedoTexture);

	Vec2f uv = GetInterpolatedData2(UV_VARYING_DATA_HASH);

	const auto* albText = m_AlbedoTexture;
	TGAColor color = albText->get(albText->get_width() * uv.u, albText->get_height() * uv.v);

	const auto* normText = m_NormalTexture;
	TGAColor textureNormalRaw = normText->get(normText->get_width() * uv.u, albText->get_height() * uv.v);

	Vec3f textureNormal = textureNormalRaw.ToFloat().ToVec3().normalize();
	const Vec3f textureNormalNDC = (mgl::MVP_IT * textureNormal.ToDirection()).ToVec3().normalize();

	Vec3f vertexNormal = GetInterpolatedData3(NORMAL_NDC_VARYING_DATA_HASH).normalize();

	Vec3f vertex0 = GetVaryingData3(VERTEX_NDC_VARYING_DATA_HASH, 0);
	Vec3f vertex1 = GetVaryingData3(VERTEX_NDC_VARYING_DATA_HASH, 1);
	Vec3f vertex2 = GetVaryingData3(VERTEX_NDC_VARYING_DATA_HASH, 2);

	Vec2f uv0 = GetVaryingData2(UV_VARYING_DATA_HASH, 0);
	Vec2f uv1 = GetVaryingData2(UV_VARYING_DATA_HASH, 1);
	Vec2f uv2 = GetVaryingData2(UV_VARYING_DATA_HASH, 2);

	// transform normals from tangent space to world space
	Mat3d A;
	Vec3f firstDelta = vertex1 - vertex0;
	Vec3f secondDelta = vertex2 - vertex0;
	A.SetRow(0, Vec3d {firstDelta.x, firstDelta.y, firstDelta.z});
	A.SetRow(1, Vec3d {secondDelta.x, secondDelta.y, secondDelta.z});
	A.SetRow(2, Vec3d{ vertexNormal.x, vertexNormal.y, vertexNormal.z });

	Mat3d A_I = A.GetInverse();

	//std::cout << A_I << "\n";



	const Vec3d tangentDouble = (A_I * Vec3d(uv1.u - uv0.u, uv2.u - uv0.u, 0.f)).normalize();
	const Vec3d bitangentDouble = (A_I * Vec3d(uv1.v - uv0.v, uv2.v - uv0.v, 0.f)).normalize();

	const Vec3f tangent = Vec3f{ (float)tangentDouble.x, (float)tangentDouble.y, (float)tangentDouble.z };
	const Vec3f bitangent = Vec3f{ (float)bitangentDouble.x, (float)bitangentDouble.y, (float)bitangentDouble.z };

	//std::cout << Vec3f(uv1.u - uv0.u, uv2.u - uv0.u, 0.f) << '\n';
	//std::cout << Vec3f(uv1.v - uv0.v, uv2.v - uv0.v, 0.f) << '\n';
	//std::cout << '\n';

	Mat3f tangentSpaceMat;
	tangentSpaceMat.SetColumn(0, tangent);
	tangentSpaceMat.SetColumn(1, bitangent);
	tangentSpaceMat.SetColumn(2, vertexNormal);

	const Vec3f textureNormalTStoNDC = (tangentSpaceMat * textureNormal).normalize();

	// transform rest of the necessary vectors into NDC space and compute diffuse and specular

	const Vec3f lightNDC = (mgl::VP * mgl::LightDir.ToDirection()).ToVec3().normalize();
	//std::cout << normal << '\n';
	const float NdotL = std::max(textureNormalTStoNDC.dot(lightNDC), 0.0f);
	 
	const Vec3f fragmentPosWS = GetInterpolatedData4(VERTEX_WS_VARYING_DATA_HASH).ToVec3();
	const Vec3f viewWS = mgl::CameraPos - fragmentPosWS;
	const Vec3f viewNDC = (mgl::VP * viewWS.ToDirection()).ToVec3().normalize();
	const Vec3f halfNDC = (lightNDC + viewNDC).normalize();

	// the specular version with phong and reflected vector (as opposed to bling phong and half vector) just fail to produce any reflections :((
	const Vec3f reflectedLight = (textureNormalNDC * (2.0f * textureNormalNDC.dot(lightNDC))) - lightNDC;
	//const float specularPhong = std::pow(std::max(viewCS.dot(reflectedLight), 0.0f), m_shininess);
	//const float specularPhong = std::pow(std::max(reflectedLight.z, 0.0f), m_shininess);


	const float specularBlingPhong = std::pow(std::max(halfNDC.dot(textureNormalTStoNDC), 0.0f), m_shininess);
	const Vec3f specularCol = mgl::LightDirColor * specularBlingPhong;

	const Vec3f diffuseCol = mgl::LightDirColor * NdotL;

	//m_FinalColor = color.ToFloat() * specularBlingPhong;
	m_FinalColor = color.ToFloat() * Vec4f(diffuseCol + specularCol, 1.0f);

	//m_FinalColor = Vector4(specularCol, 1.0f);

	//m_FinalColor = Vec4f(Vec3f(std::abs(uv1.v - uv0.v), std::abs(uv2.v - uv0.v), 0.f), 1.0);
	//m_FinalColor = Vector4(textureNormalTStoNDC, 1.0f);
	//m_FinalColor = Vec4f(Vec3f(uv.u, uv.v, 0.f), 1.0);
	//m_FinalColor = Vec4f(textureNormal, 0.0f);
	//m_FinalColor = Vector4(tangent, 1.0f);

	//m_FinalColor = vertexNormal.ToPoint();

	return true;
}


