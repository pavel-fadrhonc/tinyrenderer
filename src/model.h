#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"
#include "types.h"

namespace sor
{
	class Model {
	private:
		std::vector<Vec3f> verts_;
		std::vector<Vec3f> vnormals_;
		std::vector<Vec2f> uvs_;
		std::vector<std::vector<int> > faces_; // interleaved indices into verts_ and uvs_ array
	public:
		Model();
		~Model();
		int nverts() const;
		int nfaces() const;
		Vec3f vert(int i) const;
		Vec3f vnormal(int i) const;
		Vec2f uv(int i) const; // corresponds to the vertex
		std::vector<int> face(int idx) const;

		void Load(const char* filename);

		Vec3f VertexForFace(int faceIdx, u8 vertexIndex) const;
		Vec3f NormalForFaceAndVertex(int faceIdx, u8 vertexIndex) const;
		Vec2f UVForFaceAndVertex(int faceIdx, u8 vertexIndex) const;
	};

}


#endif //__MODEL_H__
