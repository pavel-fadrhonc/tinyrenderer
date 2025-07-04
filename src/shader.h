#pragma once
#include <cassert>
#include <map>

#include "geometry.h"
#include "model.h"
#include "tgaimage.h"

constexpr i32 NORMAL_NDC_VARYING_DATA_HASH = 1;
constexpr i32 UV_VARYING_DATA_HASH = 2;
constexpr i32 VERTEX_WS_VARYING_DATA_HASH = 3;
constexpr i32 VERTEX_CS_VARYING_DATA_HASH = 4;
constexpr i32 VERTEX_VIEWSPACE_VARYING_DATA_HASH = 5;

class IShaderBase
{
public:
	template<typename DataType>
	using VertexDataContainer = std::array<DataType, 3>;
	using Vec4Container = VertexDataContainer < Vec4f> ;
	using Vec3Container = VertexDataContainer<Vec3f>;
	using Vec2Container = VertexDataContainer<Vec2f>;
	using FloatContainer = VertexDataContainer<float>;

	template<typename DataType>
	using VaryingDataContainer = std::map<i32, VertexDataContainer<DataType>>;
	using VaryingDataContainer1 = VaryingDataContainer<float>;
	using VaryingDataContainer2 = VaryingDataContainer<Vec2f>;
	using VaryingDataContainer3 = VaryingDataContainer<Vec3f>;
	using VaryingDataContainer4 = VaryingDataContainer<Vec4f>;


protected:
	std::map<i32, Vec4Container> m_VaryingData4;
	std::map<i32, Vec3Container> m_VaryingData3;
	std::map<i32, Vec2Container> m_VaryingData2;
	std::map<i32, FloatContainer> m_VaryingData1;
};

class IFragmentShader : virtual public IShaderBase
{
public:
	/// <summary>
	/// Executes a fragment shader which result should be write to FinalColor
	/// </summary>
	/// <returns> False if fragment should be discarded, true otherwise. </returns>
	virtual bool fragment() = 0;

	const Vec4f& GetFinalColor() const { return m_FinalColor; }

	void SetAlbedoTexture(TGAImage* albedoTexture) { m_AlbedoTexture = albedoTexture; }
	void SetNormalTexture(TGAImage* normalTexture) { m_NormalTexture = normalTexture; }
	void SetSpecularTexture(TGAImage* specularTexture) { m_SpecularTexture = specularTexture; }

	void SetBarycentricCoordinates(Vec3f coords) { m_BaryCoords = coords; }
protected:
	Vec4f m_FinalColor;
	Vec3f m_BaryCoords;
	TGAImage* m_AlbedoTexture { nullptr };
	TGAImage* m_NormalTexture { nullptr };
	TGAImage* m_SpecularTexture { nullptr };

	Vec4f GetInterpolatedData4(i32 dataHash) const;
	Vec3f GetInterpolatedData3(i32 dataHash) const;
	Vec2f GetInterpolatedData2(i32 dataHash) const;
	float GetInterpolatedData1(i32 dataHash) const;

	Vec2f GetVaryingData2(i32 dataHash, u8 vertIndex) const;
	Vec3f GetVaryingData3(i32 dataHash, u8 vertIndex) const;
	Vec4f GetVaryingData4(i32 dataHash, u8 vertIndex) const;

	template<typename TDataType>
	TDataType GetVaryingData(i32 dataHash, const VaryingDataContainer<TDataType>& container, u8 vertIndex) const
	{
		assert(container.contains(dataHash));
		assert(vertIndex < 3);

		const auto& vertexData = container.at(dataHash);
		return vertexData[vertIndex];
	}
	
	template<typename TDataType>
	TDataType GetInterpolatedData(i32 dataHash, const VaryingDataContainer<TDataType>& container) const
	{
		assert(container.contains(dataHash));

		const auto& vertexData = container.at(dataHash);
		const TDataType& data1 = vertexData.at(0);
		const TDataType& data2 = vertexData.at(1);
		const TDataType& data3 = vertexData.at(2);

		return data1 * m_BaryCoords.x + data2 * m_BaryCoords.y + data3 * m_BaryCoords.z;
	}

};

class IVertexShader : virtual public IShaderBase
{
public:
	virtual Vec3f vertex(u32 faceIdx, u8 vertIdx) = 0;

	void SetModel(Model* model) { m_Model = model; }

protected:
	Model* m_Model {nullptr};

	void SetVaryingData4(i32 hash, u8 vertexIdx, const Vec4f& data);
	void SetVaryingData3(i32 hash, u8 vertexIdx, const Vec3f& data);
	void SetVaryingData2(i32 hash, u8 vertexIdx, const Vec2f& data);
	void SetVaryingData1(i32 hash, u8 vertexIdx, const float& data);

	template<typename DataType>
	void SetVaryingData(i32 hash, u8 vertexIdx, const DataType& data, VaryingDataContainer<DataType>& container)
	{
		if (!container.contains(hash))
			container.emplace(hash, VertexDataContainer<DataType>{});

		container.at(hash)[vertexIdx] = data;
	}

};

class BasicScreenSpace : public IVertexShader
{
public:
	Vec3f vertex(u32 faceIdx, u8 vertIdx) override;
};

class Phong : public IFragmentShader
{
public:
	bool fragment() override;
};

class QuantizeFragmentShader : public IFragmentShader
{
public:
	QuantizeFragmentShader(const Vec3f& tint, int levels)
		: m_Tint(tint), m_Levels(levels) {}

	bool fragment() override;

	void SetTint(Vec3f tint) { m_Tint = tint; }
	void SetLevels(int levels) { m_Levels = levels; }
private:
	Vec3f m_Tint;
	int m_Levels;
};

class NormalMappedPhongFragmentShader : public IFragmentShader
{
public:
	NormalMappedPhongFragmentShader(float shininess)
		: m_shininess(shininess) {}
	 
	bool fragment() override;

	float m_shininess;
};

class NormalMappedPhongShader : public NormalMappedPhongFragmentShader, public BasicScreenSpace
{
public:
	NormalMappedPhongShader(float shininess)
		: NormalMappedPhongFragmentShader(shininess) {}
};

class BasicPhongShader : public BasicScreenSpace, public Phong {};

class QuantizeShadar : public BasicScreenSpace, public QuantizeFragmentShader
{
public:
	QuantizeShadar(const Vec3f& tint, int levels)
		: QuantizeFragmentShader(tint, levels) {}
};