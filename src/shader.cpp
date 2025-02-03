#include <array>
#include "shader.h"

#include <assert.h>

#include "my_gl.h"

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

	Vec3f normal = m_Model->NormalForFaceAndVertex(faceIdx, vertIdx);
	SetVaryingData3(NORMAL_VARYING_DATA_HASH, vertIdx, normal);

	Vec2f uv = m_Model->UVForFaceAndVertex(faceIdx, vertIdx);
	SetVaryingData2(UV_VARYING_DATA_HASH, vertIdx, uv);

	Vec3f vertex = m_Model->VertexForFace(faceIdx, vertIdx);
	return (mgl::MVP * vertex.ToPoint()).FromHomogeneous();
}

bool GouradShader::fragment()
{
	assert(m_AlbedoTexture);

	Vec3f normal = GetInterpolatedData3(NORMAL_VARYING_DATA_HASH);
	const float NdotL = std::max(normal.dot(mgl::LightDir), 0.0f);

	const auto* tex = m_AlbedoTexture;
	Vec2f uv = GetInterpolatedData2(UV_VARYING_DATA_HASH);
	TGAColor color = tex->get(tex->get_width() * uv.u, tex->get_height() * uv.v);

	m_FinalColor = color.ToFloat() * NdotL;

	return true;
}

bool QuantizeFragmentShader::fragment()
{
	Vec3f normal = GetInterpolatedData3(NORMAL_VARYING_DATA_HASH);
	const float NdotL = std::max(normal.dot(mgl::LightDir), 0.0f);

	Vec3f finalCol = m_Tint * static_cast<float>(static_cast<int>(NdotL * static_cast<float>(m_Levels)))* (1.0f / static_cast<float>(m_Levels));

	m_FinalColor = Vec4f{ finalCol, 1.0f };

	return true;
}


