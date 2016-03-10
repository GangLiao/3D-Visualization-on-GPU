
#ifndef __CS247_Assignment4_h__
#define __CS247_Assignment4_h__

#include <stdlib.h>
#include <stdio.h>
//#include <string.h>
#include <fcntl.h>
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

#ifdef WIN32 
#include <io.h>
#endif

#ifndef WIN32
#define GLEW_STATIC 1
#endif
#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/glut.h>

////////////////
// Structures //
////////////////

//////////////////////
//  Global defines  //
//////////////////////
#define TIMER_FREQUENCY_MILLIS  50

//////////////////////
// Global variables //
//////////////////////

// Handle of the window we're rendering to
static GLint window;

char bmModifiers;	// keyboard modifiers (e.g. ctrl,...)

float clear_color[4]; // clearcolor for shader

// data handling
char* filenames[3];
bool grid_data_loaded;
bool scalar_data_loaded;
unsigned short vol_dim[3]; //data dimentions 
float* vector_array;
float* scalar_fields;
float* scalar_bounds;

GLuint scalar_field_texture;
GLuint color_code_scalarfield_shader;
GLuint color_code_scalarfield_program;

int num_scalar_fields;
int num_timesteps; //stores number of time steps

int loaded_file;
int loaded_timestep;
float timestep;

int view_width, view_height; // height and width of entire view

GLuint displayList_idx;

int toggle_xy;

vector<float>stream_pos;
float seed_x;
float seed_y;
bool isclickMouse = false;
bool isEuler = true;
bool isStreamline = false;
bool isPathline = false;
int samp_rate = 20;
bool draw_arrows = false;
float dt = 0.02;
int current_scalar_field;
////////////////
// Prototypes //
////////////////

int printOglError(char *file, int line);

#define printOpenGLError() printOglError((char *)__FILE__, __LINE__)

void reshape(int wid, int ht);

void computeStreamline(float x, float y);

void computePathline(float x, float y);

void loadNextTimestep(void);

void LoadData(char* base_filename);
void LoadVectorData(const char* filename);

void DownloadScalarFieldAsTexture(void);
void initGL(void);

void reset_rendering_props(void);

#endif // __CS247_Assignment4_h__
