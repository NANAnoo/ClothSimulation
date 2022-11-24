#ifndef CLOTHMODEL_H
#define CLOTHMODEL_H

#include "Vec3.h"
#include "tiny_obj_loader.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <QGL>

class ClothModel
{
public:
    ClothModel(std::string &file_path);
    ~ClothModel();

// a sturcture that store every thing that needed in simulation
    struct SimulationParameters
    {
        // the time step
        float dt;
        // the mass of each point
        float mass;
        // young's modulus
        float E;
        // damped variable
        float kd;
        // friction
        float fric;
        // the constant gravity
        Vec3 gravity;
        // ground
        Vec3 ground_position;
        // groun normal
        Vec3 ground_normal;
        // sphere
        Vec3 sphere_center;
        float sphere_r;
        float omega;
        // fix corner
        bool fixed_corner;
        // TODO: wind
        Vec3 wind;
        float fric_w;
    };

    // function used for rendering
    void render();
    // save current model as a file;s
    void saveAsFile(std::string file_path);
    // update mass point
    void update(SimulationParameters &params);
    // transilation
    void transilation(Vec3 movement) 
    {
        unsigned int v_size = inattrib.vertices.size() / 3;
        for (unsigned int v = 0; v < v_size; v ++) {
            inattrib.vertices[3 * v + 0] += movement.x;
            inattrib.vertices[3 * v + 1] += movement.y;
            inattrib.vertices[3 * v + 2] += movement.z;
        }
        current_transilation = current_transilation + movement;
    }
    // reset status
    void reset()
    {
        for (unsigned int v = 0; v < inattrib.vertices.size() / 3; v++)
        {
            inattrib.vertices[3 * v] = origin_vertex[3 * v];
            inattrib.vertices[3 * v + 1] = origin_vertex[3 * v + 1];
            inattrib.vertices[3 * v + 2] = origin_vertex[3 * v + 2];
            particles_velocity[v].x = 0;
            particles_velocity[v].y = 0;
            particles_velocity[v].z = 0;
        }
    }
    // rotation

    // scale

    // valid
    bool isValid() {return is_valid;}

private:
    struct DrawObject
    {
        GLuint vb_id;  // vertex buffer id
        int numTriangles;
        unsigned int material_id;
    };

    struct NeighbourInfo
    {
        int index;
        float rest_length;
        NeighbourInfo():index(0), rest_length(0){}
        NeighbourInfo(int i, float l):index(i), rest_length(l){}
        NeighbourInfo(unsigned int i, float l):index(i), rest_length(l){}
    };
    struct _NeighbourInfo_hashfunc
    {
        size_t operator()(const NeighbourInfo n) const
        {
            return size_t(n.index);
        }	
    };
    struct _NeighbourInfo_eqfunc
    {
        bool operator()(const NeighbourInfo n1, NeighbourInfo n2) const
        {
            return (n1.index == n2.index);
        }	
    };
    // a map that stores all vertices and its neighbours
    std::unordered_map<unsigned int, std::unordered_set<NeighbourInfo, _NeighbourInfo_hashfunc, _NeighbourInfo_eqfunc> > neighbours_of_vertices;
    // a vector that stores all velocity of each vertices
    std::vector<Vec3> particles_velocity;

    // rendering objects:
    std::vector<DrawObject> objects;
    std::vector<tinyobj::material_t> materials;
    tinyobj::attrib_t inattrib;
    std::vector<tinyobj::real_t> origin_vertex; 
    
    std::vector<tinyobj::shape_t> inshapes;
    // textures
    std::unordered_map<std::string, GLuint> textures;
    // texture coords
    std::vector<unsigned int> texture_indices;
    // valid
    bool is_valid;

    // model transform
    Vec3 current_transilation;

    // prepare rendering:
    void preRendering();
    // calculate accelarations
    std::vector<Vec3> getAccelarations(SimulationParameters &params,
			std::vector<tinyobj::real_t> &current_positions,
			std::vector<Vec3> &current_velosity);
    // void update bounday conditions
    void updatePositionAtBounday(SimulationParameters &params, std::vector<tinyobj::real_t> &positions, std::vector<Vec3> &velosity);
};

#endif // CLOTHMODEL_H
