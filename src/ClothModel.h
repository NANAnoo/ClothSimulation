#ifndef CLOTHMODEL_H
#define CLOTHMODEL_H

#include "Vec3.h"
#include "tiny_obj_loader.h"
#include <string>
#include <unordered_map>
#include <vector>

class ClothModel
{
public:
    ClothModel(std::string file_path);
    ~ClothModel();

// a sturcture that store every thing that needed in simulation
    struct SimulationParameters
    {
        // the time step
        double dt;
        // the mass of each point
        double mass;
        // young's modulus
        double E;
        // the constant gravity
        Vec3 gravity;
        // ground
        Vec3 ground_position;
        // groun normal
        Vec3 ground_normal;
        // TODO: wind
        // TODO: spinning sphere
    };

    // function used for rendering
    void render();
    // save current model as a file;s
    void saveAsFile(std::string file_path);
    // update mass point
    void update(SimulationParameters &params);

private:
    // a map that stores all vertices and its neighbourhood
    std::unordered_map<unsigned int, unsigned int> neighbours_of_vertices;
    // a vector that stores all position of the vertices
    std::vector<Vec3> particles_position;
    // a vector that stores all velocity of each vertices
    std::vector<Vec3> particles_velocity;
    // render model that come from tiny obj
    std::vector<tinyobj::material_t> materials;

};

#endif // CLOTHMODEL_H
