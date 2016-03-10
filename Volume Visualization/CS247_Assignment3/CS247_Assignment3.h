#ifndef __CS247_ASSIGNMENT2_h__
#define __CS247_ASSIGNMENT2_h__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include <vector>
#include <algorithm>
#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/glut.h>

////////////////
// Structures //
////////////////

struct Vertex {
	float	x;
	float	y;
	float	z;
};


//////////////////////
// Global defines   //
//////////////////////

//rotation defines
#define INERTIA_THRESHOLD       1.0f
#define INERTIA_FACTOR          0.5f
#define SCALE_FACTOR            0.01f
#define SCALE_INCREMENT         0.5f
#define TIMER_FREQUENCY_MILLIS  20


//////////////////////
// Global variables //
//////////////////////

// Handle of the window we're rendering to
static GLint window;

//
// Movement variables
//
float fXDiff;		// rotation
float fYDiff;
float fZDiff;

int xLastIncr;		// last difference in mouse movement
int yLastIncr;

float fXInertia;	// inertia for keeping rotating even after mouse movement
float fYInertia;
float fXInertiaOld; // old inertia
float fYInertiaOld;

float fScale;		// scale/zoom factor

int xLast;			// last mouse position
int yLast;

char bmModifiers;	// keyboard modifiers (e.g. ctrl,...)

int Rotate;			// auto-rotate

float clear_color[ 4 ]; // clearcolor for shader

// parameters for raycasting

int enable_lighting;	// lighting parameters
int enable_gm_scaling;
float ambient;

float step_size;	// step size for raycasting

int tf_win_min;		// transfer function parameters
int tf_win_max;

float iso_value;

int use_dvr;


// data handling

bool data_loaded;
unsigned short vol_dim[ 3 ];
unsigned short* data_array;

GLuint vol_texture;

GLuint tf_texture;

int view_width, view_height; // height and width of entire view

// buffers
GLuint framebuffer; 
GLuint backface_buffer;
GLuint frontface_buffer;
GLuint PreIntTable_buffer;

// shaders
GLuint raycast_dvr_fragment_shader;
GLuint raycast_iso_fragment_shader;
GLuint raycast_dvr_shader_program;
GLuint raycast_iso_shader_program;

////////////////
// Prototypes //
////////////////

int printOglError( char *file, int line );

#define printOpenGLError() printOglError((char *)__FILE__, __LINE__)

void reshape( int wid, int ht );

void LoadData( char* filename );
void LoadDataByte( char* filename );
void CreateTestData( void );

void DownloadVolumeAsTexture( void );

void UpdateTransferfunction( void );
void DownloadTransferFunctionTexture( int tf_id );

void RenderBackFaces(float dim_x, float dim_y, float dim_z);
void RenderFrontFaces(float dim_x, float dim_y, float dim_z);

void RenderRaycastPass( void );

void RenderBufferToScreen( GLuint buffer );

void enableRenderToBuffer( GLuint buffer );
void disableRenderToBuffer( void );

void initGL( void );

char *textFileRead( char *fn );
void LoadAndLinkShaders( void );

#endif // __CS247_ASSIGNMENT2_h__