#include "texture.h"

namespace sor
{
	//--------------------------------------------------------------------------------------------------
	Texture::Texture(int width, int height, ETextureFormat tf):
		m_Width(width), m_Height(height), m_TextureFormat(tf)
	{
		unsigned long nbytes = m_Width * m_Height * (int)m_TextureFormat;
		m_pData = new unsigned char[nbytes];
		memset(m_pData, 0, nbytes);
	}

	//--------------------------------------------------------------------------------------------------
	Texture::Texture(const Texture& texture)
	{
		m_Width = texture.m_Width;
		m_Height = texture.m_Height;
		m_TextureFormat = texture.m_TextureFormat;
		unsigned long nbytes = m_Width * m_Height * (int) m_TextureFormat;
		m_pData = new unsigned char[nbytes];
		memcpy(m_pData, texture.m_pData, nbytes);
	}

	//--------------------------------------------------------------------------------------------------
	Texture::Texture(Texture&& texture) noexcept
		:Texture()
	{
		swap(*this, texture);
	}

	//--------------------------------------------------------------------------------------------------
	Texture& Texture::operator=(const Texture& texture)
	{
		Texture tex(texture);
		swap(*this, tex);

		return *this;
	}

	//--------------------------------------------------------------------------------------------------
	Texture& Texture::operator=(Texture&& texture) noexcept
	{
		if (&texture != this)
		{
			swap(*this, texture);
		}

		return *this;
	}

	//--------------------------------------------------------------------------------------------------
	Texture::~Texture()
	{
		delete[] m_pData;
	}

	//--------------------------------------------------------------------------------------------------
	void Texture::Clear()
	{
		memset(m_pData, 0, m_Height * m_Width * (int) m_TextureFormat);
	}

	//--------------------------------------------------------------------------------------------------
	void Texture::SetPixel(int x, int y, TGAColor c)
	{
		if (!m_pData || x < 0 || y < 0 || x >= m_Width || y >= m_Height) {
			return;
		}
		memcpy(m_pData + (x + y * m_Width) * (int)m_TextureFormat, c.raw, (int)m_TextureFormat);
	}

	//--------------------------------------------------------------------------------------------------
	TGAColor Texture::GetPixel(int x, int y) const
	{
		if (!m_pData || x < 0 || y < 0 || x >= m_Width || y >= m_Height) {
			return TGAColor();
		}
		return TGAColor(m_pData + (x + y * m_Width) * (int) m_TextureFormat, (int) m_TextureFormat);
	}
}
