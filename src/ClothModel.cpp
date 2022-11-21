#include "ClothModel.h"

#include <QtOpenGL>
#include <iostream>

#ifndef TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static std::string GetBaseDir(const std::string& filepath) 
{
    if (filepath.find_last_of("/\\") != std::string::npos)
        return filepath.substr(0, filepath.find_last_of("/\\"));
    return "";
}

ClothModel::ClothModel(std::string &file_path)
{
    is_valid = false;
    // build model from obj file
    std::vector<tinyobj::shape_t> inshapes;

    std::string base_dir = GetBaseDir(file_path);
    if (base_dir.empty()) {
      base_dir = ".";
    }
#ifdef _WIN32
    base_dir += "\\";
#else
    base_dir += "/";
#endif

    std::string warn;
    std::string err;

    bool ret = tinyobj::LoadObj(&inattrib, &inshapes, &materials, &warn, &err,
                                file_path.c_str(), base_dir.c_str());

    if (!warn.empty()) {
      std::cout << "WARN: " << warn << std::endl;
    }
    if (!err.empty()) {
      std::cerr << err << std::endl;
    }
    if (!ret) {
      std::cerr << "Failed to load " << file_path << std::endl;
    }
    shape = inshapes[0];
    // load texture if needed
    tinyobj::material_t *mp = &materials[shape.mesh.material_ids[0]];
    if (mp->diffuse_texname.length() > 0) {
      // Only load the texture if it is not already loaded
      if (textures.find(mp->diffuse_texname) == textures.end()) {
        GLuint texture_id;
        int w, h;
        int comp;
        std::string texture_filepath = base_dir + mp->diffuse_texname;
        unsigned char* image = stbi_load(texture_filepath.c_str(), &w, &h, &comp, STBI_default);
        if (image) {
            // bind texture
            glGenTextures(1, &texture_id);
            glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            if (comp == 3) {
              glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB,
                           GL_UNSIGNED_BYTE, image);
            } else if (comp == 4) {
              glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA,
                           GL_UNSIGNED_BYTE, image);
            } else {
              assert(0);  // TODO
            }
            glBindTexture(GL_TEXTURE_2D, 0);
            stbi_image_free(image);
            textures.insert(std::make_pair(mp->diffuse_texname, texture_id));
        } else {
            std::cerr << "Failed to load texture: " << texture_filepath
                      << std::endl;
        }
      }
    }


    is_valid = true;
    preRendering();
}


void ClothModel::preRendering()
{
    // create data buffer
    std::vector<float> buffer;  // pos(3float), normal(3float), color(3float), texture(2float)
    unsigned int face_num = shape.mesh.indices.size() / 3;
    int m_id = shape.mesh.material_ids.size() > 0 ? shape.mesh.material_ids[0] : -1;
    for (unsigned int f = 0; f < face_num; f ++) {
        // get index from a triangle
        tinyobj::index_t idx0 = shape.mesh.indices[3 * f + 0];
        tinyobj::index_t idx1 = shape.mesh.indices[3 * f + 1];
        tinyobj::index_t idx2 = shape.mesh.indices[3 * f + 2];

        // get base color
        float color[3];
        if (m_id >=0 && m_id < materials.size()) {
            for (size_t i = 0; i < 3; i++) {
              color[i] = materials[m_id].diffuse[i];
            }
        } else {
            color[0] = 0.5f;
            color[1] = 0.6f;
            color[2] = 0.7f;
        }
        // set up texture coords
        float tc[3][2];
        if (inattrib.texcoords.size() > 0) {
            tc[0][0] = inattrib.texcoords[2 * idx0.texcoord_index];
            tc[0][1] = inattrib.texcoords[2 * idx0.texcoord_index + 1];
            tc[1][0] = inattrib.texcoords[2 * idx1.texcoord_index];
            tc[1][1] = inattrib.texcoords[2 * idx1.texcoord_index + 1];
            tc[2][0] = inattrib.texcoords[2 * idx2.texcoord_index];
            tc[2][1] = inattrib.texcoords[2 * idx2.texcoord_index + 1];
        } else {
          tc[0][0] = 0.0f;
          tc[0][1] = 0.0f;
          tc[1][0] = 0.0f;
          tc[1][1] = 0.0f;
          tc[2][0] = 0.0f;
          tc[2][1] = 0.0f;
        }
        // set up vertex coords
        float v[3][3];
        for (int k = 0; k < 3; k++) {
          int f0 = idx0.vertex_index;
          int f1 = idx1.vertex_index;
          int f2 = idx2.vertex_index;

          v[0][k] = inattrib.vertices[3 * f0 + k];
          v[1][k] = inattrib.vertices[3 * f1 + k];
          v[2][k] = inattrib.vertices[3 * f2 + k];
        }
        // set up normal
        float n[3][3];
        if (inattrib.normals.size() > 0)
        {
            int nf0 = idx0.normal_index;
            int nf1 = idx1.normal_index;
            int nf2 = idx2.normal_index;
            for (int k = 0; k < 3; k ++) {
                // read normals
                n[0][k] = inattrib.normals[3 * nf0 + k];
                n[1][k] = inattrib.normals[3 * nf1 + k];
                n[2][k] = inattrib.normals[3 * nf2 + k];
            }
        } else {
            // calculate normals:
            Vec3 norm = Vec3(v[1][0] - v[0][0], v[1][1] - v[0][1], v[1][2] -v[0][2])
                        .Cross(Vec3(v[2][0] - v[1][0], v[2][1] - v[1][1], v[2][2] - v[1][2]));
            for (int k = 0; k < 3; k ++) {
                n[k][0] = norm.x();
                n[k][1] = norm.x();
                n[k][2] = norm.z();
            }
        }
        // insert into buffer
        for (int k = 0; k < 3; k++) {
            buffer.push_back(v[k][0]);
            buffer.push_back(v[k][1]);
            buffer.push_back(v[k][2]);
            buffer.push_back(n[k][0]);
            buffer.push_back(n[k][1]);
            buffer.push_back(n[k][2]);
            buffer.push_back(color[0] * 0.5f + 0.5f);
            buffer.push_back(color[1] * 0.5f + 0.5f);
            buffer.push_back(color[2] * 0.5f + 0.5f);

            buffer.push_back(tc[k][0]);
            buffer.push_back(tc[k][1]);
        }
    }

    // binding array data
    material_id = shape.mesh.material_ids.front();
    numTriangles = face_num;
    vb_id = 0;
    if (buffer.size() > 0) {
        glGenBuffers(1, &vb_id);
        glBindBuffer(GL_ARRAY_BUFFER, vb_id);
        glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float),
                     &buffer.at(0), GL_STATIC_DRAW);
      }
}


void ClothModel::render()
{

}

void ClothModel::saveAsFile(std::string file_path)
{

}

// update mass point
void ClothModel::update(SimulationParameters &params)
{

}


ClothModel::~ClothModel()
{

}
