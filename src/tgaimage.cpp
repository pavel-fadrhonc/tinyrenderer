#include <iostream>
#include <fstream>
#include <string.h>
#include <time.h>
#include <math.h>
#include "tgaimage.h"
#include "geometry.h"

namespace sor
{
	bool TGAImage::read_tga_file(const char* filename) {
		if (texture.m_pData) delete[] texture.m_pData;
		texture.m_pData = nullptr;
		std::ifstream in;
		in.open(filename, std::ios::binary);
		if (!in.is_open()) {
			std::cerr << "can't open file " << filename << "\n";
			in.close();
			return false;
		}
		TGA_Header header;
		in.read((char*)&header, sizeof(header));
		if (!in.good()) {
			in.close();
			std::cerr << "an error occured while reading the header\n";
			return false;
		}
		texture.m_Width = header.width;
		texture.m_Height = header.height;
		texture.m_TextureFormat = (Texture::ETextureFormat) (header.bitsperpixel >> 3);
		if (texture.m_Width <= 0 || texture.m_Height <= 0 || (texture.m_TextureFormat != Texture::ETextureFormat::GREYSCALE
			&& texture.m_TextureFormat != Texture::ETextureFormat::RGB && texture.m_TextureFormat != Texture::ETextureFormat::RGBA)) {
			in.close();
			std::cerr << "bad bpp (or texture.width/texture.height) value\n";
			return false;
		}
		unsigned long nbytes = (int) texture.m_TextureFormat * texture.m_Width * texture.m_Height;
		texture.m_pData = new unsigned char[nbytes];
		if (3 == header.datatypecode || 2 == header.datatypecode) {
			in.read((char*)texture.m_pData, nbytes);
			if (!in.good()) {
				in.close();
				std::cerr << "an error occured while reading the texture.data\n";
				return false;
			}
		}
		else if (10 == header.datatypecode || 11 == header.datatypecode) {
			if (!load_rle_data(in)) {
				in.close();
				std::cerr << "an error occured while reading the texture.data\n";
				return false;
			}
		}
		else {
			in.close();
			std::cerr << "unknown file format " << (int)header.datatypecode << "\n";
			return false;
		}
		if (!(header.imagedescriptor & 0x20)) {
			flip_vertically();
		}
		if (header.imagedescriptor & 0x10) {
			flip_horizontally();
		}
		std::cerr << texture.m_Width << "x" << texture.m_Height << "/" << (int) texture.m_TextureFormat * 8 << "\n";
		in.close();
		return true;
	}

	bool TGAImage::load_rle_data(std::ifstream& in) {
		unsigned long pixelcount = texture.m_Width * texture.m_Height;
		unsigned long currentpixel = 0;
		unsigned long currentbyte = 0;
		TGAColor colorbuffer;
		do {
			unsigned char chunkheader = 0;
			chunkheader = in.get();
			if (!in.good()) {
				std::cerr << "an error occured while reading the texture.data\n";
				return false;
			}
			if (chunkheader < 128) {
				chunkheader++;
				for (int i = 0; i < chunkheader; i++) {
					in.read((char*)colorbuffer.raw, (int) texture.m_TextureFormat);
					if (!in.good()) {
						std::cerr << "an error occured while reading the header\n";
						return false;
					}
					for (int t = 0; t < (int) texture.m_TextureFormat; t++)
						texture.m_pData[currentbyte++] = colorbuffer.raw[t];
					currentpixel++;
					if (currentpixel > pixelcount) {
						std::cerr << "Too many pixels read\n";
						return false;
					}
				}
			}
			else {
				chunkheader -= 127;
				in.read((char*)colorbuffer.raw, (int) texture.m_TextureFormat);
				if (!in.good()) {
					std::cerr << "an error occured while reading the header\n";
					return false;
				}
				for (int i = 0; i < chunkheader; i++) {
					for (int t = 0; t < (int) texture.m_TextureFormat; t++)
						texture.m_pData[currentbyte++] = colorbuffer.raw[t];
					currentpixel++;
					if (currentpixel > pixelcount) {
						std::cerr << "Too many pixels read\n";
						return false;
					}
				}
			}
		} while (currentpixel < pixelcount);
		return true;
	}

	bool TGAImage::write_tga_file(const char* filename, bool rle) {
		unsigned char developer_area_ref[4] = { 0, 0, 0, 0 };
		unsigned char extension_area_ref[4] = { 0, 0, 0, 0 };
		unsigned char footer[18] = { 'T','R','U','E','V','I','S','I','O','N','-','X','F','I','L','E','.','\0' };
		std::ofstream out;
		out.open(filename, std::ios::binary);
		if (!out.is_open()) {
			std::cerr << "can't open file " << filename << "\n";
			out.close();
			return false;
		}
		TGA_Header header;
		memset((void*)&header, 0, sizeof(header));
		header.bitsperpixel = (int) texture.m_TextureFormat << 3;
		header.width = texture.m_Width;
		header.height = texture.m_Height;
		header.datatypecode = (texture.m_TextureFormat == Texture::ETextureFormat::GREYSCALE ? (rle ? 11 : 3) : (rle ? 10 : 2));
		header.imagedescriptor = 0x20; // top-left origin
		out.write((char*)&header, sizeof(header));
		if (!out.good()) {
			out.close();
			std::cerr << "can't dump the tga file\n";
			return false;
		}
		if (!rle) {
			out.write((char*)texture.m_pData, texture.m_Width * texture.m_Height * (int) texture.m_TextureFormat);
			if (!out.good()) {
				std::cerr << "can't unload raw texture.data\n";
				out.close();
				return false;
			}
		}
		else {
			if (!unload_rle_data(out)) {
				out.close();
				std::cerr << "can't unload rle texture.data\n";
				return false;
			}
		}
		out.write((char*)developer_area_ref, sizeof(developer_area_ref));
		if (!out.good()) {
			std::cerr << "can't dump the tga file\n";
			out.close();
			return false;
		}
		out.write((char*)extension_area_ref, sizeof(extension_area_ref));
		if (!out.good()) {
			std::cerr << "can't dump the tga file\n";
			out.close();
			return false;
		}
		out.write((char*)footer, sizeof(footer));
		if (!out.good()) {
			std::cerr << "can't dump the tga file\n";
			out.close();
			return false;
		}
		out.close();
		return true;
	}

	// TODO: it is not necessary to break a raw chunk for two equal pixels (for the matter of the resulting size)
	bool TGAImage::unload_rle_data(std::ofstream& out) {
		const unsigned char max_chunk_length = 128;
		unsigned long npixels = texture.m_Width * texture.m_Height;
		unsigned long curpix = 0;
		while (curpix < npixels) {
			unsigned long chunkstart = curpix * (int) texture.m_TextureFormat;
			unsigned long curbyte = curpix * (int) texture.m_TextureFormat;
			unsigned char run_length = 1;
			bool raw = true;
			while (curpix + run_length < npixels && run_length < max_chunk_length) {
				bool succ_eq = true;
				for (int t = 0; succ_eq && t < (int) texture.m_TextureFormat; t++) {
					succ_eq = (texture.m_pData[curbyte + t] == texture.m_pData[curbyte + t + (int) texture.m_TextureFormat]);
				}
				curbyte += (int) texture.m_TextureFormat;
				if (1 == run_length) {
					raw = !succ_eq;
				}
				if (raw && succ_eq) {
					run_length--;
					break;
				}
				if (!raw && !succ_eq) {
					break;
				}
				run_length++;
			}
			curpix += run_length;
			out.put(raw ? run_length - 1 : run_length + 127);
			if (!out.good()) {
				std::cerr << "can't dump the tga file\n";
				return false;
			}
			out.write((char*)(texture.m_pData + chunkstart), (raw ? run_length * (int) texture.m_TextureFormat : (int) texture.m_TextureFormat));
			if (!out.good()) {
				std::cerr << "can't dump the tga file\n";
				return false;
			}
		}
		return true;
	}

	TGAColor TGAImage::get(int x, int y) const {
		if (!texture.m_pData || x < 0 || y < 0 || x >= texture.m_Width || y >= texture.m_Height) {
			return TGAColor();
		}
		return TGAColor(texture.m_pData + (x + y * texture.m_Width) * (int) texture.m_TextureFormat, (int) texture.m_TextureFormat);
	}

	bool TGAImage::set(int x, int y, TGAColor c) {
		if (!texture.m_pData || x < 0 || y < 0 || x >= texture.m_Width || y >= texture.m_Height) {
			return false;
		}
		memcpy(texture.m_pData + (x + y * texture.m_Width) * (int) texture.m_TextureFormat, c.raw, (int) texture.m_TextureFormat);
		return true;
	}

	int TGAImage::get_bytespp() const {
		return (int) texture.m_TextureFormat;
	}

	int TGAImage::get_width() const {
		return texture.m_Width;
	}

	int TGAImage::get_height() const {
		return texture.m_Height;
	}

	bool TGAImage::flip_horizontally() {
		if (!texture.m_pData) return false;
		int half = texture.m_Width >> 1;
		for (int i = 0; i < half; i++) {
			for (int j = 0; j < texture.m_Height; j++) {
				TGAColor c1 = get(i, j);
				TGAColor c2 = get(texture.m_Width - 1 - i, j);
				set(i, j, c2);
				set(texture.m_Width - 1 - i, j, c1);
			}
		}
		return true;
	}

	bool TGAImage::flip_vertically() {
		if (!texture.m_pData) return false;
		unsigned long bytes_per_line = texture.m_Width * (int) texture.m_TextureFormat;
		unsigned char* line = new unsigned char[bytes_per_line];
		int half = texture.m_Height >> 1;
		for (int j = 0; j < half; j++) {
			unsigned long l1 = j * bytes_per_line;
			unsigned long l2 = (texture.m_Height - 1 - j) * bytes_per_line;
			memmove((void*)line, (void*)(texture.m_pData + l1), bytes_per_line);
			memmove((void*)(texture.m_pData + l1), (void*)(texture.m_pData + l2), bytes_per_line);
			memmove((void*)(texture.m_pData + l2), (void*)line, bytes_per_line);
		}
		delete[] line;
		return true;
	}

	unsigned char* TGAImage::buffer() {
		return texture.m_pData;
	}

	Texture& TGAImage::GetTexture()
	{
		return texture;
	}

	void TGAImage::clear() {
		memset((void*)texture.m_pData, 0, texture.m_Width * texture.m_Height * (int) texture.m_TextureFormat);
	}

	Vec3i ConvertModelCoordsIntoImageCoords(const Vec3f& vert, float scale, const int width, const int height, const int farPlane)
	{
		return Vec3i
		{
			static_cast<int>((vert.x * scale + 1.) * width * 0.5f),
			static_cast<int>((vert.y * scale + 1.) * height * 0.5f),
			static_cast<int>((vert.z * scale + 1.) * farPlane * 0.5f)
		};
	}

	Vec3f ConvertImageCoordsIntoModelCoords(const Vec3i& vert, const int width, const int height, const int farPlane)
	{
		return Vec3f
		{
			(float)vert.x / (float)width * 2.0f - 1.f,
			(float)vert.y / (float)height * 2.0f - 1.f,
			(float)vert.z / (float)farPlane * 2.0f - 1.f
		};
	}

	bool TGAImage::scale(int w, int h) {
		if (w <= 0 || h <= 0 || !texture.m_pData) return false;
		unsigned char* data = new unsigned char[w * h * (int) texture.m_TextureFormat];
		int nscanline = 0;
		int oscanline = 0;
		int erry = 0;
		unsigned long nlinebytes = w * (int) texture.m_TextureFormat;
		unsigned long olinebytes = texture.m_Width * (int) texture.m_TextureFormat;
		for (int j = 0; j < texture.m_Height; j++) {
			int errx = texture.m_Width - w;
			int nx = -(int) texture.m_TextureFormat;
			int ox = -(int) texture.m_TextureFormat;
			for (int i = 0; i < texture.m_Width; i++) {
				ox += (int) texture.m_TextureFormat;
				errx += w;
				while (errx >= (int)texture.m_Width) {
					errx -= texture.m_Width;
					nx += (int) texture.m_TextureFormat;
					memcpy(data + nscanline + nx, texture.m_pData + oscanline + ox, (int) texture.m_TextureFormat);
				}
			}
			erry += h;
			oscanline += olinebytes;
			while (erry >= (int)texture.m_Height) {
				if (erry >= (int)texture.m_Height << 1) // it means we jump over a scanline
					memcpy(texture.m_pData + nscanline + nlinebytes, texture.m_pData + nscanline, nlinebytes);
				erry -= texture.m_Height;
				nscanline += nlinebytes;
			}
		}
		delete[] texture.m_pData;
		texture.m_pData = data;
		texture.m_Width = w;
		texture.m_Height = h;
		return true;
	}


}
