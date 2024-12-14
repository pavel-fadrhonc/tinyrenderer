#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts_(), faces_() {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    std::vector<int> uvidxs;
    std::vector<Vec2f> uvs;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v.raw[i];
            verts_.push_back(v);
        } else if (!line.compare(0, 2, "f ")) {
            std::vector<int> f;
            int itrash, idx, uvidx;
            iss >> trash;
            while (iss >> idx >> trash >> uvidx >> trash >> itrash) {
                idx--; // in wavefront obj all indices start at 1, not zero
                uvidx--;
                f.push_back(idx);
                f.push_back(uvidx);
            }
            faces_.push_back(f);
        } else if (!line.compare(0, 3, "vt "))
        {
            float u, v;
            float ftrash;
            iss >> trash >> trash;
            iss >> u >> v >> ftrash;
            uvs_.emplace_back(u, v);
        }
    }

    //for (int uvidx: uvidxs)
    //    uvs_.push_back(uvs[uvidx-1]);

    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << std::endl;
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

std::vector<int> Model::face(int idx) {
    return faces_[idx];
}

Vec3f Model::vert(int i) {
    return verts_[i];
}

Vec2f Model::uv(int i){
    return uvs_[i];
}

