#include "ClothModel.h"

#include <QtOpenGL>
#include <iostream>
#include <fstream>
#include <limits>

#ifndef TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static void CalcNormal(float N[3], float v0[3], float v1[3], float v2[3])
{
	float v10[3];
	v10[0] = v1[0] - v0[0];
	v10[1] = v1[1] - v0[1];
	v10[2] = v1[2] - v0[2];

	float v20[3];
	v20[0] = v2[0] - v0[0];
	v20[1] = v2[1] - v0[1];
	v20[2] = v2[2] - v0[2];

	N[0] = v10[1] * v20[2] - v10[2] * v20[1];
	N[1] = v10[2] * v20[0] - v10[0] * v20[2];
	N[2] = v10[0] * v20[1] - v10[1] * v20[0];

	float len2 = N[0] * N[0] + N[1] * N[1] + N[2] * N[2];
	if (len2 > 0.0f)
	{
		float len = sqrtf(len2);

		N[0] /= len;
		N[1] /= len;
		N[2] /= len;
	}
}

void computeSmoothingNormals(const tinyobj::attrib_t &attrib, const tinyobj::shape_t &shape,
							 std::unordered_map<int, Vec3> &smoothVertexNormals)
{
	smoothVertexNormals.clear();
	std::unordered_map<int, Vec3>::iterator iter;

	for (size_t f = 0; f < shape.mesh.indices.size() / 3; f++)
	{
		// Get the three indexes of the face (all faces are triangular)
		tinyobj::index_t idx0 = shape.mesh.indices[3 * f + 0];
		tinyobj::index_t idx1 = shape.mesh.indices[3 * f + 1];
		tinyobj::index_t idx2 = shape.mesh.indices[3 * f + 2];

		// Get the three vertex indexes and coordinates
		int vi[3];	   // indexes
		float v[3][3]; // coordinates

		for (int k = 0; k < 3; k++)
		{
			vi[0] = idx0.vertex_index;
			vi[1] = idx1.vertex_index;
			vi[2] = idx2.vertex_index;
			assert(vi[0] >= 0);
			assert(vi[1] >= 0);
			assert(vi[2] >= 0);

			v[0][k] = attrib.vertices[3 * vi[0] + k];
			v[1][k] = attrib.vertices[3 * vi[1] + k];
			v[2][k] = attrib.vertices[3 * vi[2] + k];
		}

		// Compute the normal of the face
		float normal[3];
		CalcNormal(normal, v[0], v[1], v[2]);

		// Add the normal to the three vertexes
		for (size_t i = 0; i < 3; ++i)
		{
			iter = smoothVertexNormals.find(vi[i]);
			if (iter != smoothVertexNormals.end())
			{
				// add
				iter->second.x += normal[0];
				iter->second.y += normal[1];
				iter->second.z += normal[2];
			}
			else
			{
				smoothVertexNormals[vi[i]].x = normal[0];
				smoothVertexNormals[vi[i]].y = normal[1];
				smoothVertexNormals[vi[i]].z = normal[2];
			}
		}

	} // f

	// Normalize the normals, that is, make them unit vectors
	for (iter = smoothVertexNormals.begin(); iter != smoothVertexNormals.end();
		 iter++)
	{
		iter->second = iter->second.normalize();
	}
}

ClothModel::ClothModel(std::string &file_path)
{
	is_valid = false;

	std::string base_dir = ".";
	if (file_path.find_last_of("/\\") != std::string::npos) {
		base_dir = file_path.substr(0, file_path.find_last_of("/\\"));
	}
	base_dir += "/";

	std::string warn;
	std::string err;

	bool ret = tinyobj::LoadObj(&inattrib, &inshapes, &materials, &warn, &err,
								file_path.c_str(), base_dir.c_str());

	if (!warn.empty())
	{
		std::cout << "WARN: " << warn << std::endl;
	}
	if (!err.empty())
	{
		std::cerr << err << std::endl;
	}
	if (!ret)
	{
		std::cerr << "Failed to load " << file_path << std::endl;
	}
	bool is_square = file_path.find_last_of("square") > 0;
	for (auto shape : inshapes)
	{
		// load texture if needed, check if material id is available
		if (shape.mesh.material_ids.size() > 0 && shape.mesh.material_ids[0] < materials.size())
		{
			tinyobj::material_t *mp = &materials[shape.mesh.material_ids[0]];
			if (mp->diffuse_texname.length() > 0)
			{
				// Only load the texture if it is not already loaded
				if (textures.find(mp->diffuse_texname) == textures.end())
				{
					GLuint texture_id;
					int w, h;
					int comp;
					std::string texture_filepath = mp->diffuse_texname;
					if (mp->diffuse_texname.find('/') == std::string::npos) {
						texture_filepath = base_dir + mp->diffuse_texname;
					}
					texture_files.insert(std::make_pair(mp->diffuse_texname, texture_filepath));
					unsigned char *image = stbi_load(texture_filepath.c_str(), &w, &h, &comp, STBI_default);
					if (image)
					{
						// bind texture
						glGenTextures(1, &texture_id);
						glBindTexture(GL_TEXTURE_2D, texture_id);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						if (comp == 3)
						{
							glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB,
										 GL_UNSIGNED_BYTE, image);
						}
						else
						{
							glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA,
										 GL_UNSIGNED_BYTE, image);
						}
						glBindTexture(GL_TEXTURE_2D, 0);
						stbi_image_free(image);
						textures.insert(std::make_pair(mp->diffuse_texname, texture_id));
					}
					else
					{
						std::cerr << "Failed to load texture: " << texture_filepath
								  << std::endl;
					}
				}
			}
		}

		// update neighbour of every vertex
		if (is_square)
		{ // update for squre
			unsigned int quad_num = shape.mesh.indices.size() / 6;
			for (unsigned int q = 0; q < quad_num; q++)
			{
				// get index from a triangle
				std::unordered_set<unsigned int> quad_vertex_indeices;
				for (unsigned int k = 0; k < 6; k++)
				{
					tinyobj::index_t idx = shape.mesh.indices[6 * q + k];
					quad_vertex_indeices.insert(idx.vertex_index);
				}
				if (quad_vertex_indeices.size() == 4)
				{
					// pair each vertex with other three
					std::vector<unsigned int> v_indexs(quad_vertex_indeices.begin(), quad_vertex_indeices.end());
					for (unsigned int m = 0; m < 4; m++)
					{
						float x = inattrib.vertices[3 * v_indexs[m] + 0];
						float y = inattrib.vertices[3 * v_indexs[m] + 1];
						float z = inattrib.vertices[3 * v_indexs[m] + 2];
						for (unsigned int n = 0; n < 4; n++)
						{
							if (m != n && neighbours_of_vertices[v_indexs[m]].find({v_indexs[n], 0}) == neighbours_of_vertices[v_indexs[m]].end())
							{
								float dx = inattrib.vertices[3 * v_indexs[n] + 0] - x;
								float dy = inattrib.vertices[3 * v_indexs[n] + 1] - y;
								float dz = inattrib.vertices[3 * v_indexs[n] + 2] - z;
								// calculate rest length
								float l = sqrtf(dx * dx + dy * dy + dz * dz);
								neighbours_of_vertices[v_indexs[m]].insert({v_indexs[n], l});
							}
						}
					}
				}
				else
				{
					std::cout << "Error squre is not regular !!!" << std::endl;
				}
			}
		}
		else
		{
			// unregular object, use triangle as string info
			unsigned int face_num = shape.mesh.indices.size() / 6;
			for (unsigned int f = 0; f < face_num; f++)
			{
				tinyobj::index_t idx0 = shape.mesh.indices[3 * f + 0];
				tinyobj::index_t idx1 = shape.mesh.indices[3 * f + 1];
				tinyobj::index_t idx2 = shape.mesh.indices[3 * f + 2];

				Vec3 a(inattrib.vertices[3 * idx0.vertex_index],
					   inattrib.vertices[3 * idx0.vertex_index + 1],
					   inattrib.vertices[3 * idx0.vertex_index + 2]);
				Vec3 b(inattrib.vertices[3 * idx1.vertex_index],
					   inattrib.vertices[3 * idx1.vertex_index + 1],
					   inattrib.vertices[3 * idx1.vertex_index + 2]);
				Vec3 c(inattrib.vertices[3 * idx2.vertex_index],
					   inattrib.vertices[3 * idx2.vertex_index + 1],
					   inattrib.vertices[3 * idx2.vertex_index + 2]);

				float l_01 = (a - b).lenth();
				float l_02 = (a - c).lenth();
				float l_12 = (b - c).lenth();

				// pair each edge with neighbour
				neighbours_of_vertices[idx0.vertex_index].insert({idx1.vertex_index, l_01});
				neighbours_of_vertices[idx0.vertex_index].insert({idx2.vertex_index, l_02});
				neighbours_of_vertices[idx1.vertex_index].insert({idx0.vertex_index, l_01});
				neighbours_of_vertices[idx1.vertex_index].insert({idx2.vertex_index, l_12});
				neighbours_of_vertices[idx2.vertex_index].insert({idx0.vertex_index, l_02});
				neighbours_of_vertices[idx2.vertex_index].insert({idx1.vertex_index, l_12});
			}
		}
	}
	float x_max = -std::numeric_limits<float>::infinity(), z_max = -std::numeric_limits<float>::infinity(),
		  x_min = std::numeric_limits<float>::infinity(), z_min = std::numeric_limits<float>::infinity();
	corner_1 = 0, corner_2 = 0; corner_3 = 0; corner_4 = 0;
	// find corner
	for (auto pair : neighbours_of_vertices)
	{
		float x = inattrib.vertices[3 * pair.first];
		float z = inattrib.vertices[3 * pair.first + 2];
		if (x > inattrib.vertices[3 * corner_1] || z > inattrib.vertices[3 * corner_1 + 2])
		{
			corner_1 = pair.first;
		}
		if (x > inattrib.vertices[3 * corner_2] || z < inattrib.vertices[3 * corner_2 + 2])
		{
			corner_2 = pair.first;
		}
		if (x < inattrib.vertices[3 * corner_3] || z > inattrib.vertices[3 * corner_3 + 2])
		{
			corner_3 = pair.first;
		}
		if (x < inattrib.vertices[3 * corner_4] || z < inattrib.vertices[3 * corner_4 + 2])
		{
			corner_4 = pair.first;
		}
		x_max = std::max(x_max, x);
		z_max = std::max(z_max, z);
		x_min = std::min(x_min, x);
		z_min = std::min(z_min, z);
	}
	// move to center
	transilation({-(x_max + x_min)/2, 0, -(z_max + z_min) / 2} );
	// init vertors
	particles_velocity.resize(inattrib.vertices.size() / 3);
	origin_vertex = std::vector<tinyobj::real_t>(inattrib.vertices.begin(), inattrib.vertices.end());

	is_valid = true;
}

void ClothModel::preRendering()
{
	// create data buffer
	unsigned int shape_count = 0;
	for (auto shape : inshapes)
	{
		shape_count++;
		unsigned int face_num = shape.mesh.indices.size() / 3;
		float *buffer = new float[face_num * 3 * 11]; // pos(3float), normal(3float), color(3float), texture(2float)
		int m_id = shape.mesh.material_ids.size() > 0 ? shape.mesh.material_ids[0] : -1;
		unsigned int index_buffer = 0;
		std::unordered_map<int, Vec3> smooth_normals;
		computeSmoothingNormals(inattrib, shape, smooth_normals);
		for (unsigned int f = 0; f < face_num; f++)
		{
			// get index from a triangle
			tinyobj::index_t idx0 = shape.mesh.indices[3 * f + 0];
			tinyobj::index_t idx1 = shape.mesh.indices[3 * f + 1];
			tinyobj::index_t idx2 = shape.mesh.indices[3 * f + 2];

			// get base color
			float color[3];
			if (m_id >= 0 && m_id < materials.size())
			{
				for (size_t i = 0; i < 3; i++)
				{
					color[i] = materials[m_id].diffuse[i];
				}
			}
			else
			{
				color[0] = 0.5f;
				color[1] = 0.6f;
				color[2] = 0.7f;
			}
			// set up texture coords
			float tc[3][2];
			if (inattrib.texcoords.size() > 0)
			{
				tc[0][0] = inattrib.texcoords[2 * idx0.texcoord_index];
				tc[0][1] = inattrib.texcoords[2 * idx0.texcoord_index + 1];
				tc[1][0] = inattrib.texcoords[2 * idx1.texcoord_index];
				tc[1][1] = inattrib.texcoords[2 * idx1.texcoord_index + 1];
				tc[2][0] = inattrib.texcoords[2 * idx2.texcoord_index];
				tc[2][1] = inattrib.texcoords[2 * idx2.texcoord_index + 1];
			}
			else
			{
				tc[0][0] = 0.0f;
				tc[0][1] = 0.0f;
				tc[1][0] = 0.0f;
				tc[1][1] = 0.0f;
				tc[2][0] = 0.0f;
				tc[2][1] = 0.0f;
			}
			// set up vertex coords
			float v[3][3];
			for (int k = 0; k < 3; k++)
			{
				int f0 = idx0.vertex_index;
				int f1 = idx1.vertex_index;
				int f2 = idx2.vertex_index;

				v[0][k] = inattrib.vertices[3 * f0 + k];
				v[1][k] = inattrib.vertices[3 * f1 + k];
				v[2][k] = inattrib.vertices[3 * f2 + k];
			}
			// set up normal
			float n[3][3];
			Vec3 norm = Vec3(v[1][0] - v[0][0], v[1][1] - v[0][1], v[1][2] - v[0][2])
							.Cross(Vec3(v[2][0] - v[1][0], v[2][1] - v[1][1], v[2][2] - v[1][2]));
			norm = norm.normalize();
			n[0][0] = smooth_normals[idx0.vertex_index].x;
			n[0][1] = smooth_normals[idx0.vertex_index].y;
			n[0][2] = smooth_normals[idx0.vertex_index].z;
			n[1][0] = smooth_normals[idx1.vertex_index].x;
			n[1][1] = smooth_normals[idx1.vertex_index].y;
			n[1][2] = smooth_normals[idx1.vertex_index].z;
			n[2][0] = smooth_normals[idx2.vertex_index].x;
			n[2][1] = smooth_normals[idx2.vertex_index].y;
			n[2][2] = smooth_normals[idx2.vertex_index].z;
			// insert into buffer
			for (int k = 0; k < 3; k++)
			{
				buffer[index_buffer++] = v[k][0];
				buffer[index_buffer++] = v[k][1];
				buffer[index_buffer++] = v[k][2];
				buffer[index_buffer++] = n[k][0];
				buffer[index_buffer++] = n[k][1];
				buffer[index_buffer++] = n[k][2];
				buffer[index_buffer++] = color[0];
				buffer[index_buffer++] = color[1];
				buffer[index_buffer++] = color[2];

				buffer[index_buffer++] = tc[k][0];
				buffer[index_buffer++] = tc[k][1];
			}
		}

		// binding array data
		if (objects.size() < shape_count)
		{
			DrawObject obj;
			obj.material_id = shape.mesh.material_ids.front();
			obj.numTriangles = face_num;
			obj.vb_id = 0;
			if (index_buffer > 0)
			{
				// generate new buffer
				glGenBuffers(1, &obj.vb_id);
				glBindBuffer(GL_ARRAY_BUFFER, obj.vb_id);
				glBufferData(GL_ARRAY_BUFFER, index_buffer * sizeof(float),
							 buffer, GL_STATIC_DRAW);
			}
			objects.push_back(obj);
		}
		else
		{
			DrawObject obj = objects[shape_count - 1];
			glBindBuffer(GL_ARRAY_BUFFER, obj.vb_id);
			glBufferData(GL_ARRAY_BUFFER, index_buffer * sizeof(float),
						 buffer, GL_STATIC_DRAW);
		}
		delete buffer;
		buffer = nullptr;
	}
}

void ClothModel::render()
{
	// sent all data to GPU
	preRendering();
	// render it
	// stetup polygen
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);

	// draw every objects
	for (auto obj : objects)
	{
		// bind buffer
		glBindBuffer(GL_ARRAY_BUFFER, obj.vb_id);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		// bind texture
		glBindTexture(GL_TEXTURE_2D, 0);
		tinyobj::material_t *material = nullptr;
		if (obj.material_id < materials.size())
		{
			material = &materials[obj.material_id];
			std::string texname = material->diffuse_texname;
			if (textures.find(texname) != textures.end())
			{
				glBindTexture(GL_TEXTURE_2D, textures[texname]);
			}
		}
		// set up pointer
		GLsizei stride = (3 + 3 + 3 + 2) * sizeof(float);
		glVertexPointer(3, GL_FLOAT, stride, (const void *)0);
		glNormalPointer(GL_FLOAT, stride, (const void *)(sizeof(float) * 3));
		glColorPointer(3, GL_FLOAT, stride, (const void *)(sizeof(float) * 6));
		glTexCoordPointer(2, GL_FLOAT, stride, (const void *)(sizeof(float) * 9));

		if (material != nullptr)
		{
			GLfloat mat_ambient[] = {material->ambient[0], material->ambient[1], material->ambient[2], 1.0f};
			GLfloat mat_diffuse[] = {material->diffuse[0], material->diffuse[1], material->diffuse[2], 1.0f};
			GLfloat mat_specular[] = {material->specular[0], material->specular[1], material->specular[2], 1.0f};
			GLfloat mat_emission[] = {material->emission[0], material->emission[1], material->emission[2], 1.0f};
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emission);
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material->shininess);
		}

		glDrawArrays(GL_TRIANGLES, 0, 3 * obj.numTriangles);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// check error
		GLenum e = glGetError();
		if (e != GL_NO_ERROR)
		{
			fprintf(stderr, "Opengl error, code : %d", e);
			exit(20);
		}
	}
}

bool ClothModel::saveAsFile(std::string file_path)
{
	// get file name
	std::string file_name = file_path.substr(0, file_path.find_last_of(".obj") - 3);
	std::string mtl_file = file_name + ".mtl";
	if (file_name.find('/') != std::string::npos) {
		file_name = file_name.substr(file_name.find_last_of('/')+1);
	}
	std::cout << mtl_file << std::endl;
	std::cout << file_name << std::endl;

	// write .obj file
	std::ofstream file;
    file.open(file_path, std::ios::out);
    if (!file.is_open())
        return false;
    file.setf(std::ios::showpoint);
    file.precision(6);
	if (materials.size() > 0) {
		// has material file
		file << "mtllib ./" << file_name << ".mtl" << std::endl;
	}
	file << std::endl;
	for (unsigned int v = 0; v < inattrib.vertices.size() / 3; v ++ )
	{
		file << "v " << inattrib.vertices[3 * v] << " " << 
						inattrib.vertices[3 * v + 1] << " " << 
						inattrib.vertices[3 * v + 2] << std::endl;
	}
	for (unsigned int v = 0; v < inattrib.normals.size() / 3; v ++ )
	{
		file << "vn " << inattrib.normals[3 * v] << " " << 
						inattrib.normals[3 * v + 1] << " " << 
						inattrib.normals[3 * v + 2]<< std::endl;
	}
	for (unsigned int v = 0; v < inattrib.texcoords.size() / 2; v ++ )
	{
		file << "vt " << inattrib.texcoords[2 * v] << " " << 
						inattrib.texcoords[2 * v + 1]<< std::endl;
	}
	file << std::endl;
	// write all shapes
	for (auto shape : inshapes)
	{
		// try to write name
		if (shape.name.size() > 0) {
			file << "o " << shape.name << std::endl;
		}
		// try to write material
		if (shape.mesh.material_ids.size() > 0 && materials.size() > shape.mesh.material_ids[0]) {
			std::string m_name = materials[shape.mesh.material_ids[0]].name;
			file << "usemtl " << m_name << std::endl;
		}
		// write all faces
		for (unsigned int f = 0; f < shape.mesh.indices.size() / 3; f ++)
		{
			file << "f ";
			for (unsigned int k = 0; k < 3; k ++) {
				auto idx = shape.mesh.indices[3 * f + k];
				file << idx.vertex_index + 1<< "/" 
				 << idx.texcoord_index + 1 << "/" 
				 << idx.normal_index + 1 << " ";
			}
			file << std::endl;
		}
		file << std::endl;
	}
	file.close();

	// write mtl file
	if (materials.size() > 0)
	{
		file.open(mtl_file);
		if (!file.is_open())
        	return false;
		for (auto m : materials)
		{
			file << "newmtl " << m.name << std::endl;
			// ambient
			file << "Ka " << m.ambient[0] << " " 
						 << m.ambient[1] << " " 
						 << m.ambient[2] << std::endl;
			// diffuse
			file << "Kd " << m.diffuse[0] << " " 
						 << m.diffuse[1] << " " 
						 << m.diffuse[2] << std::endl;
			// specular
			file << "Ks " << m.specular[0] << " " 
						 << m.specular[1] << " " 
						 << m.specular[2] << std::endl;
			// transmittance
			file << "Kt " << m.transmittance[0] << " " 
						 << m.transmittance[1] << " " 
						 << m.transmittance[2] << std::endl;
			// emission
			file << "Kt " << m.emission[0] << " " 
						 << m.emission[1] << " " 
						 << m.emission[2] << std::endl;
			// ior
			file << "Ni " << m.ior << std::endl;

			// shininess
			file << "Ns " << m.shininess << std::endl;

			// illum
			file << "illum " << m.illum << std::endl;

			// map_Kd
			if (texture_files.find(m.diffuse_texname) != texture_files.end()) {
				file << "map_Kd " <<  texture_files[m.diffuse_texname] << std::endl;
			}
			file << std::endl;
		}
	}
	file.close();
	return true;
}

static void boundryUpdate(Vec3 &pos, Vec3 &vel, Vec3 surf_o, Vec3 sur_n, float coe)
{
	Vec3 o_p = pos - surf_o;
	float res = o_p * sur_n;
	if (res < 0)
	{
		// move the point out
		pos = (o_p - sur_n * res) + surf_o;
		// remove the velosity along normal
		res = vel * sur_n;
		vel = (vel - sur_n * res) * coe;
	}
}

static void friction(Vec3 &pos, Vec3 &vel, Vec3 surf_o, Vec3 sur_n, Vec3 surf_v, Vec3 &total_force, float k)
{
	Vec3 o_p = pos - surf_o;
	float res = o_p * sur_n;
	if (abs(res) < std::numeric_limits<float>::epsilon())
	{
		// on surface
		float pressure = total_force * sur_n;
		if (pressure < 0)
		{
			res = vel * sur_n;
			// transform vel to surface
			vel = vel - sur_n * res;
			// realative velosity on surface
			Vec3 relative_v = vel - surf_v;
			// maxmum friction
			float max_friction = abs(k * pressure);
			// keep force on surface
			total_force = total_force - sur_n * pressure;
			if (relative_v.lenth() < std::numeric_limits<float>::epsilon())
			{
				// not move
				float against_friction = total_force.lenth();
				if (against_friction > max_friction)
				{
					total_force = total_force.normalize() * (against_friction - max_friction);
				}
				else
				{
					// force is too small to against friction
					total_force = {};
				}
			}
			else
			{
				// negative friction direction
				Vec3 n_friction_direction = relative_v.normalize();
				Vec3 max_friction_force = n_friction_direction * (-max_friction);
				// move on surface, add friction
				total_force = total_force + max_friction_force;
			}
		}
	}
}

void ClothModel::update(SimulationParameters &params)
{
	// get accelaration at current state
	auto accs = getAccelarations(params, inattrib.vertices, particles_velocity);
	// caculate the positions and velosities in next frame;
	std::vector<tinyobj::real_t> next_positions = std::vector<tinyobj::real_t>(inattrib.vertices.size());
	std::vector<Vec3> next_velocity = std::vector<Vec3>(particles_velocity.size());
	for (auto pair : neighbours_of_vertices)
	{
		unsigned int index = pair.first;
		next_positions[index * 3 + 0] = inattrib.vertices[index * 3 + 0] + next_velocity[index].x;
		next_positions[index * 3 + 1] = inattrib.vertices[index * 3 + 1] + next_velocity[index].y;
		next_positions[index * 3 + 2] = inattrib.vertices[index * 3 + 2] + next_velocity[index].z;
	}
	for (unsigned int i = 0; i < next_velocity.size(); i++)
	{
		next_velocity[i] = particles_velocity[i] + accs[i] * params.dt;
	}
	// update boundary
	updatePositionAtBounday(params, next_positions, next_velocity);
	// semi implcit Euler, get accs in next frame
	accs = getAccelarations(params, next_positions, next_velocity);
	// update velosity
	for (unsigned int i = 0; i < next_velocity.size(); i++)
	{
		particles_velocity[i] = particles_velocity[i] + accs[i] * params.dt;
	}
	// update position
	for (auto pair : neighbours_of_vertices)
	{
		unsigned int index = pair.first;
		inattrib.vertices[index * 3 + 0] += particles_velocity[index].x * params.dt;
		inattrib.vertices[index * 3 + 1] += particles_velocity[index].y * params.dt;
		inattrib.vertices[index * 3 + 2] += particles_velocity[index].z * params.dt;
	}
	// update boundary
	updatePositionAtBounday(params, inattrib.vertices, particles_velocity);
}

std::vector<Vec3> ClothModel::getAccelarations(SimulationParameters &params,
											   std::vector<tinyobj::real_t> &current_positions,
											   std::vector<Vec3> &current_velosity)
{
	std::vector<Vec3> accs = std::vector<Vec3>(current_velosity.size());
	// mass spring model
	// calculate acceleration first
	for (auto pair : neighbours_of_vertices)
	{
		// get current position
		unsigned int index = pair.first;
		if (params.fixed_corner && (index == corner_2 || index == corner_4))
		{
			continue;
		}
		Vec3 point(
			current_positions[3 * index + 0],
			current_positions[3 * index + 1],
			current_positions[3 * index + 2]);
		// add gravity
		Vec3 force(params.gravity * params.mass);
		// add accleration from springs
		for (auto ne : pair.second)
		{
			// for each neighbour
			Vec3 neighbour(
				current_positions[3 * ne.index + 0],
				current_positions[3 * ne.index + 1],
				current_positions[3 * ne.index + 2]);
			// calulate current distacne
			Vec3 o_n = neighbour - point;
			Vec3 delta_v = current_velosity[ne.index] - current_velosity[index];
			float d_length = o_n.lenth() - ne.rest_length;
			d_length = (d_length > 0 ? d_length : 0);
			o_n = o_n.normalize();
			// a = (E * delta_length + delta_v * d_p) * d_p / mass
			force = force + o_n * (d_length * params.E + params.kd * (delta_v * o_n));
		}
		// acc from wind
		Vec3 rel_vel = current_velosity[index] - params.wind;
		force = force + rel_vel * (-params.fric_w);

		// friction on ground
		friction(point, current_velosity[index], params.ground_position, params.ground_normal, {}, force, params.fric);
		// friction on sphere
		if (params.sphere_r > 0)
		{
			Vec3 c_p = point - params.sphere_center;
			if (c_p.lenth() <= params.sphere_r + std::numeric_limits<float>::epsilon())
			{
				// inside the sphere
				Vec3 surf_n = c_p.normalize();
				Vec3 c_surf_p = surf_n * params.sphere_r;
				Vec3 surf_p = c_surf_p + params.sphere_center;
				float res = c_surf_p * params.ground_normal;
				Vec3 xz_radius = c_surf_p - c_surf_p * res;
				float r_on_XZ = xz_radius.lenth();
				float angle_velosity = r_on_XZ * params.omega;
				Vec3 angle_velosity_dir = (params.ground_normal.Cross(xz_radius)).normalize();
				Vec3 surf_v = angle_velosity_dir * angle_velosity;
				friction(point, current_velosity[index], surf_p, surf_n, surf_v, force, params.fric);
			}
		}
		accs[index] = force * (1.0 / params.mass);
	}
	return accs;
}

void ClothModel::updatePositionAtBounday(SimulationParameters &params, std::vector<tinyobj::real_t> &positions, std::vector<Vec3> &velosity)
{
	// Timer timer("updatePositionAtBounday");
	unsigned int v_num = positions.size() / 3;
	for (unsigned int index = 0; index < velosity.size(); index++)
	{
		Vec3 position(positions[3 * index], positions[3 * index + 1], positions[3 * index + 2]);
		if (params.sphere_r > 0)
		{
			Vec3 c_p = position - params.sphere_center;
			if (c_p.lenth() < params.sphere_r)
			{
				// inside the sphere
				Vec3 surf_n = c_p.normalize();
				Vec3 c_surf_p = surf_n * params.sphere_r;
				Vec3 surf_p = c_surf_p + params.sphere_center;
				float res = c_surf_p * params.ground_normal;
				Vec3 xz_radius = c_surf_p - c_surf_p * res;
				float r_on_XZ = xz_radius.lenth();
				float angle_velosity = r_on_XZ * params.omega;
				Vec3 angle_velosity_dir = (params.ground_normal.Cross(xz_radius)).normalize();
				Vec3 surf_v = angle_velosity_dir * angle_velosity;
				boundryUpdate(position, velosity[index], surf_p, surf_n, 1);
			}
		}
		// ground test:
		boundryUpdate(position, velosity[index], params.ground_position, params.ground_normal, 0);
		positions[3 * index + 0] = position.x;
		positions[3 * index + 1] = position.y;
		positions[3 * index + 2] = position.z;
	}
}

ClothModel::~ClothModel()
{
}
