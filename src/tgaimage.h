#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <fstream>

#include "geometry.h"
#include "texture.h"
#include "TGAColor.h"

namespace sor
{
#pragma pack(push,1)
	struct TGA_Header {
		char idlength;
		char colormaptype;
		char datatypecode;
		short colormaporigin;
		short colormaplength;
		char colormapdepth;
		short x_origin;
		short y_origin;
		short width;
		short height;
		char  bitsperpixel;
		char  imagedescriptor;
	};
#pragma pack(pop)

	class TGAImage {
	protected:
		Texture texture;
		bool   load_rle_data(std::ifstream& in);
		bool unload_rle_data(std::ofstream& out);
	public:
		TGAImage() {}
		TGAImage(int w, int h, Texture::ETextureFormat format)
			: texture(w, h, format) {}

		bool read_tga_file(const char* filename);
		bool write_tga_file(const char* filename, bool rle = true);
		bool flip_horizontally();
		bool flip_vertically();
		bool scale(int w, int h);
		TGAColor get(int x, int y) const;
		bool set(int x, int y, TGAColor c);
		TGAImage& operator =(const TGAImage& img);
		int get_width() const;
		int get_height() const;
		int get_bytespp() const;
		unsigned char* buffer();
		Texture& GetTexture();
		void clear();
	};

	Vec3i ConvertModelCoordsIntoImageCoords(const Vec3f& vert, float scale, const int width, const int height, const int farPlane);
	Vec3f ConvertImageCoordsIntoModelCoords(const Vec3f& vert, float scale, const int width, const int height, const int farPlane);

}

#endif //__IMAGE_H__
