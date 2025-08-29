#include <array>
#include <iostream>
#include "shader.h"
#include "geometry.h"

#include <assert.h>
#include <complex.h>
#include <iostream>

#include "my_gl.h"

namespace sor
{
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

	//--------------------------------------------------------------------------------------------------
	Vec4f BasicScreenSpace::vertex(u32 faceIdx, u8 vertIdx)
	{
		assert(m_Model);

		Vec2f uv = m_Model->UVForFaceAndVertex(faceIdx, vertIdx);
		SetVaryingData2(UV_VARYING_DATA_HASH, vertIdx, uv);

		Vec3f vertex = m_Model->VertexForFace(faceIdx, vertIdx);
		Vec4f positionWS = (ModelMat * vertex.ToPoint());

		SetVaryingData4(VERTEX_WS_VARYING_DATA_HASH, vertIdx, positionWS);

		Vec4f positionCS = ProjectionMat * ViewMat * positionWS;
		SetVaryingData4(VERTEX_CS_VARYING_DATA_HASH, vertIdx, positionCS);

		SetVaryingData3(VERTEX_VIEWSPACE_VARYING_DATA_HASH, vertIdx, (ViewMat * positionWS).FromHomogeneous());

		return positionCS;
	}

	//--------------------------------------------------------------------------------------------------
	Vec4f BasicScreenSpaceWithNormals::vertex(u32 faceIdx, u8 vertIdx)
	{
		Vec3f vertexNormal = m_Model->NormalForFaceAndVertex(faceIdx, vertIdx);
		Vec3f vertexNormalNDC = (MVP_IT * vertexNormal.ToDirection()).ToVec3();
		SetVaryingData3(NORMAL_NDC_VARYING_DATA_HASH, vertIdx, vertexNormalNDC);

		return BasicScreenSpace::vertex(faceIdx, vertIdx);
	}

	//--------------------------------------------------------------------------------------------------
	bool FlatColorFragmentShader::fragment()
	{
		assert(m_AlbedoTexture);

		const auto* tex = m_AlbedoTexture;
		Vec2f uv = GetInterpolatedData2(UV_VARYING_DATA_HASH);
		TGAColor color = tex->get(tex->get_width() * uv.u, tex->get_height() * uv.v);

		m_FinalColor = color.ToFloat();

		return true;
	}

	//--------------------------------------------------------------------------------------------------
	bool Phong::fragment()
	{
		assert(m_AlbedoTexture);

		Vec3f normal = GetInterpolatedData3(NORMAL_NDC_VARYING_DATA_HASH);
		const float NdotL = std::max(normal.dot(LightDir), 0.0f);

		const auto* tex = m_AlbedoTexture;
		Vec2f uv = GetInterpolatedData2(UV_VARYING_DATA_HASH);
		TGAColor color = tex->get(tex->get_width() * uv.u, tex->get_height() * uv.v);

		m_FinalColor = color.ToFloat() * NdotL;

		return true;
	}

	//--------------------------------------------------------------------------------------------------
	bool QuantizeFragmentShader::fragment()
	{
		Vec3f normal = GetInterpolatedData3(NORMAL_NDC_VARYING_DATA_HASH);
		const float NdotL = std::max(normal.dot(LightDir), 0.0f);

		Vec3f finalCol = m_Tint * static_cast<float>(static_cast<int>(NdotL * static_cast<float>(m_Levels))) * (1.0f / static_cast<float>(m_Levels));

		m_FinalColor = Vec4f{ finalCol, 1.0f };

		return true;
	}

	//--------------------------------------------------------------------------------------------------
	bool NormalMappedPhongFragmentShader::fragment()
	{
		assert(m_NormalTexture);
		assert(m_AlbedoTexture);

		Vec2f uv = GetInterpolatedData2(UV_VARYING_DATA_HASH);

		const auto* albText = m_AlbedoTexture;
		TGAColor color = albText->get(albText->get_width() * uv.u, albText->get_height() * uv.v);

		const auto* normText = m_NormalTexture;
		TGAColor textureNormalRaw = normText->get(normText->get_width() * uv.u, albText->get_height() * uv.v);

		TGAColor specularTextureColor;
		if (m_SpecularTexture)
			specularTextureColor = m_SpecularTexture->get(m_SpecularTexture->get_width() * uv.u, m_SpecularTexture->get_height() * uv.v);

		Vec3f textureNormal = textureNormalRaw.ToFloat().ToVec3();
		textureNormal = textureNormal * 2.0;
		textureNormal = textureNormal - 1.f;
		textureNormal = Vec3{ textureNormal.z, textureNormal.y, textureNormal.x };

		//textureNormal = Vec3f(textureNormalRaw.b, textureNormalRaw.g, textureNormalRaw.r) * 2.f / 255.f - Vec3{ 1.f, 1.f, 1.f };

		const Vec3f textureNormalNDC = (MVP_IT * textureNormal.ToDirection()).ToVec3().normalize();

		Vec3f vertexNormal = GetInterpolatedData3(NORMAL_NDC_VARYING_DATA_HASH).normalize();
		Vec3f vertex0 = GetVaryingData3(VERTEX_VIEWSPACE_VARYING_DATA_HASH, 0);
		Vec3f vertex1 = GetVaryingData3(VERTEX_VIEWSPACE_VARYING_DATA_HASH, 1);
		Vec3f vertex2 = GetVaryingData3(VERTEX_VIEWSPACE_VARYING_DATA_HASH, 2);
		Vec2f uv0 = GetVaryingData2(UV_VARYING_DATA_HASH, 0);
		Vec2f uv1 = GetVaryingData2(UV_VARYING_DATA_HASH, 1);
		Vec2f uv2 = GetVaryingData2(UV_VARYING_DATA_HASH, 2);  // transform normals from tangent space to world space
		Vec3f firstDelta = vertex1 - vertex0;
		Vec3f secondDelta = vertex2 - vertex0;
		Mat3d A;
		A.SetRow(0, Vec3d{ firstDelta.x, firstDelta.y, firstDelta.z });
		A.SetRow(1, Vec3d{ secondDelta.x, secondDelta.y, secondDelta.z });
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
		const Vec3f lightNDC = (ViewMat * LightDir.ToDirection()).ToVec3().normalize();
		//std::cout << normal << '\n';
		const float NdotL = std::max(vertexNormal.dot(lightNDC), 0.0f);
		const Vec3f fragmentPosWS = GetInterpolatedData4(VERTEX_WS_VARYING_DATA_HASH).ToVec3();
		const Vec3f viewWS = CameraPos - fragmentPosWS;
		const Vec3f viewNDC = (ViewMat * viewWS.ToDirection()).ToVec3().normalize();
		const Vec3f halfNDC = (lightNDC + viewNDC).normalize();
		// the specular version with phong and reflected vector (as opposed to bling phong and half vector) just fail to produce any reflections :((
		//const Vec3f reflectedLight = (textureNormalNDC * (2.0f * textureNormalNDC.dot(lightNDC))) - lightNDC;
		//const float specularPhong = std::pow(std::max(viewCS.dot(reflectedLight), 0.0f), m_shininess);
		//const float specularPhong = std::pow(std::max(reflectedLight.z, 0.0f), m_shininess);
		float shininess = m_SpecularTexture ? 5.f + 4.f * (specularTextureColor.r) : m_shininess;

		const float specularBlingPhong = std::pow(std::max(halfNDC.dot(textureNormalTStoNDC), 0.0f), shininess);
		const Vec3f specularCol = LightDirColor * specularBlingPhong;

		const Vec3f diffuseCol = LightDirColor * NdotL;

		//m_FinalColor = color.ToFloat() + Vec4f(specularCol, 1.f);
		m_FinalColor = color.ToFloat();
		//m_FinalColor = Vec4f(specularCol, 1.f);
		//m_FinalColor = Vec4f(textureNormalTStoNDC, 1.f);
		//m_FinalColor = specularTextureColor.ToFloat();
		//m_FinalColor = Vec4f(diffuseCol, 1.0f);
		//m_FinalColor = Vec4f(firstDelta.normalize(), 1.0f);
		//m_FinalColor = Vec4f(vertex2.normalize(), 1.0f);
	//	m_FinalColor = color.ToFloat() * Vec4f(diffuseCol + specularCol, 1.0f);
		// std::cout << vertex0 << "\n";
		// std::cout << vertex1 << "\n";
		// std::cout << vertex2 << "\n";


		//m_FinalColor = Vector4(specularCol, 1.0f);

		//m_FinalColor = Vec4f(textureNormal, 0.0f);
		//m_FinalColor = Vector4(tangent, 1.0f);

		//m_FinalColor = vertexNormal.ToPoint();
		//m_FinalColor.Clamp01();

		return true;
	}


}
