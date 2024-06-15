#pragma once
#include "vector.h"
#include "triangle.h"

#define NUM_CUBE_VERTICES 8
extern vec3_t cube_vertices[NUM_CUBE_VERTICES];

#define NUM_CUBE_FACES (6 * 2) // 6 cube faces, 2 triangles per face
extern face_t cube_faces[NUM_CUBE_FACES];

//defines a struct for dynamic sized meshes
typedef struct {
	vec3_t* vertices; //dynamic array of vertices
	face_t* faces; //dynamic array of faces
	vec3_t rotation; //rotation with xyz values
} mesh_t;

void load_cube_mesh_data(void);
void load_obj_file_data(char* filename);

mesh_t mesh;