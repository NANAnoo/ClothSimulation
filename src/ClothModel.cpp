#include "ClothModel.h"

#include <QtOpenGL>
#include <iostream>

#ifndef TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#endif

static std::string GetBaseDir(const std::string& filepath) 
{
    if (filepath.find_last_of("/\\") != std::string::npos)
        return filepath.substr(0, filepath.find_last_of("/\\"));
    return "";
}

ClothModel::ClothModel(std::string file_path)
{
    is_valid = false;
    // build model from obj file
    tinyobj::attrib_t inattrib;
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
    tinyobj::shape_t shape = inshapes[0];
    shape.mesh.num_face_vertices;
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
