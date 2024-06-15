#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <stdint.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"

triangle_t* triangles_to_render = NULL;

vec3_t camera_position = { .x = 0, .y = 0, .z = -5 };
//vec3_t mesh.rotation = { .x = 0, .y = 0, .z = 0 };

float fov_factor = 680;

bool is_running = false;
int previous_frame_time = 0;

void setup(void) {
	//allocate the memory in bytes to hold the color buffer
	color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);

	//creating a SDL texture that is used to display the color buffer
	color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);

	//loads the cube values in the mesh data structure
	//load_cube_mesh_data();
	load_obj_file_data("./assets/f22.obj");
}

void process_input(void) {
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type){
	case SDL_QUIT:
		is_running = false;
		break;
	case SDL_KEYDOWN:
		if (event.key.keysym.sym == SDLK_ESCAPE){
			is_running = false;
			break;
		}
	}
}

vec2_t project(vec3_t point) {
	vec2_t projected_point = {
		.x = (fov_factor * point.x) / point.z,
		.y = (fov_factor * point.y) / point.z
	};
	return projected_point;
}

void update(void) {
	previous_frame_time = SDL_GetTicks();

	//initialize the array of triangles to render
	triangles_to_render = NULL;

	int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
	if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
		SDL_Delay(time_to_wait);
	}

	mesh.rotation.x += 0.01;
	//mesh.rotation.y += 0.01;
	//mesh.rotation.z += 0.01;

	//loops all triangle faces
	int num_faces = array_length(mesh.faces);
	for (int i = 0; i < num_faces; i++) {
		face_t mesh_face = mesh.faces[i];

		vec3_t face_vertices[3];
		face_vertices[0] = mesh.vertices[mesh_face.a - 1];
		face_vertices[1] = mesh.vertices[mesh_face.b - 1];
		face_vertices[2] = mesh.vertices[mesh_face.c - 1];

		triangle_t projected_triangle;

		//loop all 3 vertices of this current face and apply transformations
		for (int j = 0; j < 3; j++) {
			vec3_t transformed_vertex = face_vertices[j];
			transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
			transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
			transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

			//translate the vertex away from the camera
			transformed_vertex.z -= camera_position.z;

			//Project the current vertex
			vec2_t projected_point = project(transformed_vertex);

			//Scale and translate projected points to the middle of the screen
			projected_point.x += window_width / 2;
			projected_point.y += window_height / 2;

			projected_triangle.points[j] = projected_point;
		}
		//save the projected triangle in the array of triangles to render
		array_push(triangles_to_render, projected_triangle);
	}
}
unsigned int counter = 0;

void render(void) {

	draw_grid();

	//loop all projected triangles and render them
	int number_of_triangles = array_length(triangles_to_render);
	for (int i = 0; i < number_of_triangles; i++) {
		triangle_t triangle = triangles_to_render[i];
		
		//draw vertex points
		draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFFFF00);
		draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFFFF00);
		draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFFFF00);

		//draw unfilled triangle
		draw_triangle(triangle.points[0].x,
			triangle.points[0].y,
			triangle.points[1].x,
			triangle.points[1].y,
			triangle.points[2].x,
			triangle.points[2].y,
			0xFF00FF00);
	}

	//clear array of triangles to render every frame loop
	array_free(triangles_to_render);

	render_color_buffer();
	clear_color_buffer(0xFF000000);

	SDL_RenderPresent(renderer);
}

//free dynamically allocated memory
void free_resources(void) {
	free(color_buffer);
	array_free(mesh.faces);
	array_free(mesh.vertices);
}

int main(int argc, char *args[]) {
	is_running = initialize_window();

	setup();


	while (is_running){
		process_input();
		update();
		render();
	}

	destroy_window();
	free_resources();

	return 0;
}