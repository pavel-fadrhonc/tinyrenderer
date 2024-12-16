#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <fstream>

#include "geometry.h"

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



struct TGAColor {
	union {
		struct {
			unsigned char b, g, r, a;
		};
		unsigned char raw[4];
		unsigned int val;
	};
	int bytespp;

	constexpr TGAColor() : val(0), bytespp(1) {}

	constexpr TGAColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A) : b(B), g(G), r(R), a(A), bytespp(4) {	}

	static inline TGAColor FromFloat(float R, float G, float B, float A)
	{
		return TGAColor(static_cast<unsigned char>(B * 255), static_cast<unsigned char>(G * 255), static_cast<unsigned char>(R * 255), static_cast<unsigned char>(A * 255));
	}

	constexpr TGAColor(int v, int bpp) : val(v), bytespp(bpp) {}

	constexpr TGAColor(const TGAColor &c) : val(c.val), bytespp(c.bytespp) {	}

	constexpr TGAColor(const unsigned char *p, int bpp) : val(0), bytespp(bpp) {
		for (int i=0; i<bpp; i++) {
			raw[i] = p[i];
		}
	}

	TGAColor & operator =(const TGAColor &c) {
		if (this != &c) {
			bytespp = c.bytespp;
			val = c.val;
		}
		return *this;
	}

	void scale(float scalar)
	{
		r = (char)((float)r * scalar);
		g = (char)((float)g * scalar);
		b = (char)((float)b * scalar);
		a = (char)((float)a * scalar);
	}

	TGAColor operator*(const TGAColor& c)
	{
		TGAColor col {*this};
		col *= c;
		return col;
	}

	TGAColor& operator*=(const TGAColor& c)
	{
		r = (char) ((float) r / 255.0f * c.r);
		g = (char) ((float) g / 255.0f * c.g);
		b = (char) ((float) b / 255.0f * c.b);
		a = (char) ((float) a / 255.0f * c.a);
		return *this;
	}
};


class TGAImage {
protected:
	unsigned char* data;
	int width;
	int height;
	int bytespp;

	bool   load_rle_data(std::ifstream &in);
	bool unload_rle_data(std::ofstream &out);
public:
	enum Format {
		GRAYSCALE=1, RGB=3, RGBA=4
	};

	TGAImage();
	TGAImage(int w, int h, int bpp);
	TGAImage(const TGAImage &img);
	bool read_tga_file(const char *filename);
	bool write_tga_file(const char *filename, bool rle=true);
	bool flip_horizontally();
	bool flip_vertically();
	bool scale(int w, int h);
	TGAColor get(int x, int y) const;
	bool set(int x, int y, TGAColor c);
	~TGAImage();
	TGAImage & operator =(const TGAImage &img);
	int get_width() const;
	int get_height() const;
	int get_bytespp() const;
	unsigned char *buffer();
	void clear();
};

Vec3i ConvertModelCoordsIntoImageCoords(const Vec3f& vert, float scale, const int width, const int height, const int farPlane);
Vec3f ConvertImageCoordsIntoModelCoords(const Vec3f& vert, float scale, const int width, const int height, const int farPlane);

#endif //__IMAGE_H__
