#pragma once

#include <iostream>

#include "TGAColor.h"

namespace sor
{
	class Texture
	{
	public:
		friend class TGAImage;
		// also gives bytes per pixel when converted to int
		enum class ETextureFormat : u8
		{
			GREYSCALE = 1,
			RGB = 3,
			RGBA = 4
		};

		Texture() = default;

		Texture(int width, int height, ETextureFormat tf);

		Texture(const Texture& texture);
		Texture(Texture&& texture) noexcept;
		Texture& operator=(const Texture& texture);
		Texture& operator=(Texture&& texture) noexcept;

		~Texture();

		int GetHeight() const { return m_Height; }
		int GetWidth() const { return m_Width; }
		ETextureFormat GetTextureFormat() const { return m_TextureFormat; }

		void Clear();

		void SetPixel(int x, int y, TGAColor c);
		TGAColor GetPixel(int x, int y) const;
		const unsigned char* GetBuffer() const { return m_pData; }

		friend void swap(Texture& tex1, Texture& tex2)
		{
			std::swap(tex1.m_pData, tex2.m_pData);
			std::swap(tex1.m_Height, tex2.m_Height);
			std::swap(tex1.m_Width, tex2.m_Width);
			std::swap(tex1.m_TextureFormat, tex2.m_TextureFormat);
		}

	private:

		unsigned char* m_pData{ nullptr };
		int m_Width = 0;
		int m_Height = 0;
		ETextureFormat m_TextureFormat = ETextureFormat::RGB;
	};
}

