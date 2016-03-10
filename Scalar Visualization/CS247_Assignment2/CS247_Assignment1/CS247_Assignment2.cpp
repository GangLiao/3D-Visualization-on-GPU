// CS247_Assignment2.cpp : Defines the entry point for the console application.

#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#include <algorithm>
#include <iostream>

#include "CS247_Assignment2.h"

// ==========================================================================
// Handle OpenGL errors
int printOglError(char *file, int line)
{
	// Returns 1 if an OpenGL error occurred, 0 otherwise.
	GLenum glErr;
	int    retCode = 0;

	glErr = glGetError();
	while (glErr != GL_NO_ERROR)
	{
		printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
		retCode = 1;
		glErr = glGetError();
	}
	return retCode;
}
#define printOpenGLError() printOglError((char *)__FILE__, __LINE__)

// ==========================================================================
// Parse GL_VERSION and return the major and minor numbers in the supplied
// integers.
// If it fails for any reason, major and minor will be set to 0.
// Assumes a valid OpenGL context.
void getGlVersion(int *major, int *minor)
{
	const char* verstr = (const char*)glGetString(GL_VERSION);
	if ((verstr == NULL) || (sscanf_s(verstr, "%d.%d", major, minor) != 2)) {
		*major = *minor = 0;
		fprintf(stderr, "Invalid GL_VERSION format!!!\n");
	}
}

// ==========================================================================
// Cycle clear colors
static void NextClearColor(void)
{
	clearColor = (++clearColor) % 3;
}

static void SetClearColor(void)
{
	switch (clearColor)
	{
	case 0:
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		break;
	case 1:
		glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
		break;
	default:
		glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
		break;
	}
}

// ==========================================================================
// GLUT
static void display2D(void)
{
	glLoadIdentity();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (data_loaded) {

		glEnable(GL_TEXTURE_3D);

		glActiveTexture(vol_texture);
		glBindTexture(GL_TEXTURE_3D, vol_texture);

		float slice = current_slice[current_axis] / (float)vol_dim[current_axis];

		float w = 1.0f;
		float h = 1.0f;

		glColor3f(1.0f, 1.0f, 1.0f);
		if (current_axis == 0) {

			float aspect_viewport_voldim_x = viewport_x_2D / (float)vol_dim[1];
			float aspect_viewport_voldim_y = viewport_y_2D / (float)vol_dim[2];

			float aspect = (float)vol_dim[1] / (float)vol_dim[2];

			if (aspect_viewport_voldim_x < aspect_viewport_voldim_y) {
				//maximize w
				h /= aspect;
				h /= (viewport_y_2D / (float)viewport_x_2D);

			}
			else {
				//maximize h
				w *= aspect;
				w /= (viewport_x_2D / (float)viewport_y_2D);

			}

			glBegin(GL_QUADS);
			glTexCoord3f(slice, 0.0f, 0.0f);
			glVertex3f(-w, -h, 0.0f);

			glTexCoord3f(slice, 1.0f, 0.0f);
			glVertex3f(w, -h, 0.0f);

			glTexCoord3f(slice, 1.0f, 1.0f);
			glVertex3f(w, h, 0.0f);

			glTexCoord3f(slice, 0.0f, 1.0f);
			glVertex3f(-w, h, 0.0f);
			glEnd();

		}
		else if (current_axis == 1) {

			float aspect_viewport_voldim_x = viewport_x_2D / (float)vol_dim[0];
			float aspect_viewport_voldim_y = viewport_y_2D / (float)vol_dim[2];

			float aspect = (float)vol_dim[0] / (float)vol_dim[2];

			if (aspect_viewport_voldim_x < aspect_viewport_voldim_y) {
				//maximize w
				h /= aspect;
				h /= (viewport_y_2D / (float)viewport_x_2D);

			}
			else {
				//maximize h
				w *= aspect;
				w /= (viewport_x_2D / (float)viewport_y_2D);

			}

			glBegin(GL_QUADS);
			glTexCoord3f(0.0f, slice, 0.0f);
			glVertex3f(-w, -h, 0.0f);

			glTexCoord3f(1.0f, slice, 0.0f);
			glVertex3f(w, -h, 0.0f);

			glTexCoord3f(1.0f, slice, 1.0f);
			glVertex3f(w, h, 0.0f);

			glTexCoord3f(0.0f, slice, 1.0f);
			glVertex3f(-w, h, 0.0f);
			glEnd();

		}
		else {

			float aspect_viewport_voldim_x = viewport_x_2D / (float)vol_dim[0];
			float aspect_viewport_voldim_y = viewport_y_2D / (float)vol_dim[1];

			float aspect = (float)vol_dim[0] / (float)vol_dim[1];

			if (aspect_viewport_voldim_x < aspect_viewport_voldim_y) {
				//maximize w
				h /= aspect;
				h /= (viewport_y_2D / (float)viewport_x_2D);

			}
			else {
				//maximize h
				w *= aspect;
				w /= (viewport_x_2D / (float)viewport_y_2D);

			}

			glBegin(GL_QUADS);
			glTexCoord3f(0.0f, 0.0f, slice);
			glVertex3f(-w, -h, 0.0f);

			glTexCoord3f(1.0f, 0.0f, slice);
			glVertex3f(w, -h, 0.0f);

			glTexCoord3f(1.0f, 1.0f, slice);
			glVertex3f(w, h, 0.0f);

			glTexCoord3f(0.0f, 1.0f, slice);
			glVertex3f(-w, h, 0.0f);
			glEnd();
		}

		glDisable(GL_TEXTURE_3D);

		// Overlay iso contour
		if (show_contour){
			DrawContour(w, h);
		}
	}

	glFlush();
	glutSwapBuffers();
}

static void display3D(void)
{
	glShadeModel(GL_SMOOTH);	// gouraud shading

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_NORMALIZE); // enable OpenGL to normalize the normals

	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	// specify light properties
	GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glLoadIdentity();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// object translation
	glTranslatef(0.0, 0.0, -5.0);

	// object rotation
	glRotatef(fYDiff, 1, 0, 0);
	glRotatef(fXDiff, 0, 1, 0);
	glRotatef(fZDiff, 0, 0, 1);

	float zscale = 1.0;

	if (data_loaded) {
		zscale = (vol_dim[2] / (float)vol_dim[0]);
	}

	// object scaling - assuming that x and y dimensions of dataset are the same, correct scaling of z
	glScalef(fScale, fScale, (fScale * zscale));

	// draw iso-surface that was extracted with marching cubes
	DrawSurface(1.0, 1.0);

	// draw bounding box of volume
	DrawBoundingBox();

	glFlush();
	glutSwapBuffers();

	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_LIGHTING);
}

static void play(void)
{
	int currentWindow = glutGetWindow();

	if (currentWindow == window3D)
		glutSetWindow(window2D);
	else
		glutSetWindow(window3D);

	glutPostRedisplay();

	glutSetWindow(currentWindow);
	glutPostRedisplay();
}

static void key(unsigned char keyPressed, int x, int y)
{
	int currentWindow = glutGetWindow();

	switch (keyPressed) {

	case 'w':
		current_slice[current_axis] = std::min((current_slice[current_axis] + 1), vol_dim[current_axis] - 1);
		fprintf(stderr, "increasing current slice: %i\n", current_slice[current_axis]);
		MarchingSquares();
		break;
	case 's':
		current_slice[current_axis] = std::max((current_slice[current_axis] - 1), 0);
		fprintf(stderr, "decreasing current slice: %i\n", current_slice[current_axis]);
		MarchingSquares();
		break;
	case 'a':
		current_axis = ((current_axis + 1) % 3);
		fprintf(stderr, "toggling viewing axis to: %i\n", current_axis);
		MarchingSquares();
		break;
	case 'c':
		show_contour = !show_contour;
		fprintf(stderr, "toggle showing contour: %i\n", show_contour);
		MarchingSquares();
		break;
	case 'i':
		if (current_iso_value <= 0.99f) {
			current_iso_value += 0.01f;
		}
		else {
			current_iso_value = 1.0f;
		}
		fprintf(stderr, "increasing current iso value to: %f\n", current_iso_value);
		MarchingSquares();
		MarchingCubes();
		break;
	case 'k':
		if (current_iso_value >= 0.01f) {
			current_iso_value -= 0.01f;
		}
		else {
			current_iso_value = 0.0f;
		}
		fprintf(stderr, "decreasing current iso value to: %f\n", current_iso_value);
		MarchingSquares();
		MarchingCubes();
		break;
	case 'o':
		if (current_iso_value <= 0.9f) {
			current_iso_value += 0.1f;
		}
		else {
			current_iso_value = 1.0f;
		}
		fprintf(stderr, "increasing current iso value to: %f\n", current_iso_value);
		MarchingSquares();
		MarchingCubes();
		break;
	case 'l':
		if (current_iso_value >= 0.1f) {
			current_iso_value -= 0.1f;
		}
		else {
			current_iso_value = 0.0f;
		}
		fprintf(stderr, "decreasing current iso value to: %f\n", current_iso_value);
		
		MarchingSquares();
		
		MarchingCubes();
		break;
	case '1':
		isosurface.reserve(95000);
		LoadData("../../Datasets/lobster.dat");
		break;
	case '2':
		isosurface.reserve(820000);
		LoadData("../../Datasets/skewed_head.dat");
		break;
	case 'b':
		NextClearColor();
		glutSetWindow(window2D);
		SetClearColor();
		glutSetWindow(window3D);
		SetClearColor();
		break;
	default:
		fprintf(stderr, "\nKeyboard commands:\n\n"
			"b - Toggle among background clear colors\n"
			"w - Increase current slice\n"
			"s - Decrease current slice\n"
			"a - Toggle viewing axis\n"
			"c - Toggle contour\n"
			"i - Increase iso-value by 0.01\n"
			"o - Increase iso-value by 0.1\n"
			"k - Decrease iso-value by 0.01\n"
			"l - Decrease iso-value by 0.1\n"
			"1 - Load lobster dataset\n"
			"2 - Load head dataset\n");
		break;
	}
}

static void mouse2D(int button, int state, int x, int y){}

static void mouse3D(int button, int state, int x, int y)
{
	bmModifiers = glutGetModifiers();

	if (button == GLUT_LEFT_BUTTON) { //rotate 

		if (state == GLUT_UP) {
			xLast = -1;
			yLast = -1;

			if (xLastIncr > INERTIA_THRESHOLD) {
				fXInertia = (xLastIncr - INERTIA_THRESHOLD) * INERTIA_FACTOR;
			}

			if (-xLastIncr > INERTIA_THRESHOLD) {
				fXInertia = (xLastIncr + INERTIA_THRESHOLD) * INERTIA_FACTOR;
			}

			if (yLastIncr > INERTIA_THRESHOLD) {
				fYInertia = (yLastIncr - INERTIA_THRESHOLD) * INERTIA_FACTOR;
			}

			if (-yLastIncr > INERTIA_THRESHOLD) {
				fYInertia = (yLastIncr + INERTIA_THRESHOLD) * INERTIA_FACTOR;
			}

		}
		else {
			fXInertia = 0;
			fYInertia = 0;
		}

		xLastIncr = 0;
		yLastIncr = 0;
	}
}

static void motion(int x, int y)
{
	if ((xLast != -1) || (yLast != -1)) { //zoom
		xLastIncr = x - xLast;
		yLastIncr = y - yLast;
		if (bmModifiers & GLUT_ACTIVE_CTRL) {
			if (xLast != -1) {
				fZDiff += xLastIncr;
				fScale += yLastIncr * SCALE_FACTOR;
			}
		}
		else {
			if (xLast != -1) { //rotate
				fXDiff += xLastIncr;
				fYDiff += yLastIncr;
			}
		}
	}

	xLast = x;
	yLast = y;
}

static void reshape2D(int w, int h)
{
	float aspect = (float)w / (float)h;

	viewport_x_2D = w;
	viewport_y_2D = h;

	glViewport(0, 0, viewport_x_2D, viewport_y_2D);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, viewport_x_2D, viewport_y_2D);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-1.0, 1.0, -1.0, 1.0, -10.0, 10.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -5.0);
}

static void reshape3D(int w, int h)
{
	viewport_x_3D = w;
	viewport_y_3D = h;

	float vp = 0.8f;
	float aspect = (float)viewport_x_3D / (float)viewport_y_3D;

	// set viewport
	glViewport(0, 0, viewport_x_3D, viewport_y_3D);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// set perspective projection
	glFrustum(-vp, vp, (-vp / aspect), (vp / aspect), 3, 10.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -5.0);
}

static void special(int key, int x, int y)
{
	//handle special keys
	switch (key) {
	case GLUT_KEY_HOME: //return to inital state/view
		fXDiff = 0;
		fYDiff = 0;
		fZDiff = 0;
		xLastIncr = 0;
		yLastIncr = 0;
		fXInertia = 0;
		fYInertia = 0;
		fScale = 1.0;
		break;
	case GLUT_KEY_LEFT:
		fXDiff--;
		break;
	case GLUT_KEY_RIGHT:
		fXDiff++;
		break;
	case GLUT_KEY_UP:
		fYDiff--;
		break;
	case GLUT_KEY_DOWN:
		fYDiff++;
		break;
	default:
		break;
	}
}

// ==========================================================================
// Data
void LoadData(char* filename)
{
	fprintf(stderr, "loading data %s\n", filename);

	FILE* fp;
	fopen_s(&fp, filename, "rb"); // open file, read only, binary mode 
	if (fp == NULL) {
		fprintf(stderr, "Cannot open file %s for reading.\n", filename);
		return;
	}

	memset(vol_dim, 0, sizeof(unsigned short)* 3);

	//read volume dimension
	fread(&vol_dim[0], sizeof(unsigned short), 1, fp);
	fread(&vol_dim[1], sizeof(unsigned short), 1, fp);
	fread(&vol_dim[2], sizeof(unsigned short), 1, fp);

	fprintf(stderr, "volume dimensions: x: %i, y: %i, z:%i \n", vol_dim[0], vol_dim[1], vol_dim[2]);

	if (data_array != NULL) {
		delete[] data_array;
	}

	data_array = new unsigned short[vol_dim[0] * vol_dim[1] * vol_dim[2]]; //for intensity volume

	for (int z = 0; z < vol_dim[2]; z++) {
		for (int y = 0; y < vol_dim[1]; y++) {
			for (int x = 0; x < vol_dim[0]; x++) {

				fread(&data_array[x + (y * vol_dim[0]) + (z * vol_dim[0] * vol_dim[1])], sizeof(unsigned short), 1, fp);
				data_array[x + (y * vol_dim[0]) + (z * vol_dim[0] * vol_dim[1])] = data_array[x + (y * vol_dim[0]) + (z * vol_dim[0] * vol_dim[1])] << 4;
			}
		}
	}

	fclose(fp);

	current_slice[0] = vol_dim[0] / 2;
	current_slice[1] = vol_dim[1] / 2;
	current_slice[2] = vol_dim[2] / 2;

	DownloadVolumeAsTexture();

	data_loaded = true;

	MarchingSquares();
	MarchingCubes();
}

void DownloadVolumeAsTexture()
{
	// Texture only needed in 2D window, so we switch to 2D context
	int currentWindow = glutGetWindow();
	glutSetWindow(window2D);

	fprintf(stderr, "downloading volume to 3D texture\n");

	glEnable(GL_TEXTURE_3D);

	glGenTextures(1, &vol_texture);
	glBindTexture(GL_TEXTURE_3D, vol_texture);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_INTENSITY16, vol_dim[0], vol_dim[1], vol_dim[2], 0, GL_LUMINANCE, GL_UNSIGNED_SHORT, data_array);

	// Switch back to original context
	glutSetWindow(currentWindow);
}

int main(int argc, char* argv[])
{
	data_array = NULL;

	viewport_x_2D = 0;
	viewport_y_2D = 0;

	viewport_x_3D = 0;
	viewport_y_3D = 0;

	current_slice[0] = 0;
	current_slice[1] = 0;
	current_slice[2] = 0;
	current_axis = 0;

	fXDiff = 0;
	fYDiff = 0;
	fZDiff = 0;
	xLastIncr = 0;
	yLastIncr = 0;
	fXInertia = 0;
	fYInertia = 0;
	fXInertiaOld;
	fYInertiaOld;
	fScale = 1.0;
	xLast = -1;
	yLast = -1;
	bmModifiers = 0;
	Rotate = 1;

	data_loaded = false;

	clearColor = 0;

	show_contour = true;
	current_iso_value = 0.5f;

	vol_texture = 0;
	displayList_idx = 0;

	int gl_major, gl_minor;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(500, 500);
	window2D = glutCreateWindow("AMCS/CS247 Scientific Visualization - Assignment 2 - 2D");
	glutPositionWindow(40, 40);

	glutIdleFunc(play);
	glutDisplayFunc(display2D);
	glutKeyboardFunc(key);
	glutReshapeFunc(reshape2D);
	glutMouseFunc(mouse2D);

	window3D = glutCreateWindow("AMCS/CS247 Scientific Visualization - Assignment 2 - 3D");
	glutPositionWindow(560, 40);

	glutIdleFunc(play);
	glutDisplayFunc(display3D);
	glutKeyboardFunc(key);
	glutSpecialFunc(special);
	glutReshapeFunc(reshape3D);
	glutMouseFunc(mouse3D);
	glutMotionFunc(motion);

	// Initialize the "OpenGL Extension Wrangler" library
	glewInit();

	// query and display OpenGL version
	getGlVersion(&gl_major, &gl_minor);
	printf("GL_VERSION major=%d minor=%d\n", gl_major, gl_minor);

	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

	NextClearColor();

	// display help
	key('?', 0, 0);

	glutMainLoop();

	return 0;
}








void GetLinePoints(int cell, int i, int j, int dimx, int dimy, double a, double b, double c, double d)
{
	double x1, y1, x2, y2, x3, y3, x4, y4;
	switch (edgeTable2D[cell])
	{
	case 1:	
		x1 = i + (current_iso_value - a) / (d - a);
		y1 = j;

		x2 = i+1;
		y2 = j + (current_iso_value - d) / (c - d);

		break;

	case 2:
		x1 = i + (current_iso_value - b) / (c - b);
		y1 = j+1;

		x2 = i + 1;
		y2 = j + (current_iso_value - d) / (c - d);
		break;

	case 3:
		x1 = i + (current_iso_value - a) / (d - a);
		y1 = j;

		x2 = i + (current_iso_value - b) / (c - b);
		y2 = j + 1;
		break;

	case 4:
		y1 = j + (current_iso_value - a) / (b - a);
		x1 = i;

		x2 = i + (current_iso_value - b) / (c - b);
		y2 = j + 1;
		break;

	case 5:
		x1 = i + (current_iso_value - a) / (d - a);
		y1 = j;

		x2 = i + 1;
		y2 = j + (current_iso_value - d) / (c - d);

		y3 = j + (current_iso_value - a) / (b - a);
		x3 = i;

		x4 = i + (current_iso_value - b) / (c - b);
		y4 = j + 1;

		x3 = x3 * 2 / (dimx - 1) - 1;
		y3 = y3 * 2 / (dimy - 1) - 1;

		x4 = x4 * 2 / (dimx - 1) - 1;
		y4 = y4 * 2 / (dimy - 1) - 1;
			
		contour.push_back(x3);
		contour.push_back(y3);
		contour.push_back(x4);
		contour.push_back(y4);
		
		break;

	case 6:
		y1 = j + (current_iso_value - a) / (b - a);
		x1 = i;

		x2 = i + 1;
		y2 = j + (current_iso_value - d) / (c - d);

		break;

	case 7:
		x1 = i;
		y1 = j + (current_iso_value - a) / (b - a);

		x2 = i + (current_iso_value - a) / (d - a);
		y2 = j;

		break;

	}

	if (edgeTable2D[cell] != 0 && edgeTable2D[cell] != 15)
	{
		
		x1 = x1 * 2 / (dimx - 1) - 1;
		y1 = y1 * 2 / (dimy - 1) - 1;

		x2 = x2 * 2 / (dimx - 1) - 1;
		y2 = y2 * 2 / (dimy - 1) - 1;
		

		contour.push_back(x1);
		contour.push_back(y1);
		contour.push_back(x2);
		contour.push_back(y2);

	}
	
	
}

double Data(int x, int y, int z)
{
	return data_array[x + (y * vol_dim[0]) + (z * vol_dim[0] * vol_dim[1])];
}

void MarchingSquares()
{
	// =============================================================================
	// TODO: Extract 2D surface using marching squares algorithm
	// =============================================================================
	contour.clear();
	current_iso_value = 15000;
	float epsilon = 20.0;
	int cell = 0;
	double a, b, c, d;
	if (current_axis == 0)
	{
		int x = current_slice[current_axis];
			for (int y = 0; y < vol_dim[1] - 1; y+= 1) {
				for (int z = 0; z < vol_dim[2] - 1 ; z+= 1) {
					cell = 0;
					if (Data(x, y, z) < current_iso_value - epsilon) cell |= 8;
					if (Data(x, y + 1, z) < current_iso_value - epsilon) cell |= 4;
					if (Data(x, y + 1, z + 1) < current_iso_value - epsilon) cell |= 2;
					if (Data(x, y, z + 1) < current_iso_value - epsilon) cell |= 1;

					GetLinePoints(cell, z, y, vol_dim[2], vol_dim[1], Data(x, y, z), Data(x, y + 1, z), Data(x, y + 1, z + 1), Data(x, y, z + 1));
					
				}
			}

	}
	else if (current_axis == 1)
	{
		int y = current_slice[current_axis];
		
			for (int x = 0; x < vol_dim[0] - 1; x++) {
				for (int z = 0; z < vol_dim[2] - 1; z++) {
				cell = 0;
				if (Data(x, y, z) < current_iso_value - epsilon) cell |= 8;
				if (Data(x + 1, y, z) < current_iso_value - epsilon) cell |= 4;
				if (Data(x + 1, y, z + 1) < current_iso_value - epsilon) cell |= 2;
				if (Data(x, y, z + 1) < current_iso_value - epsilon) cell |= 1;

				GetLinePoints(cell, z, x, vol_dim[2], vol_dim[0], Data(x, y, z), Data(x + 1, y, z), Data(x + 1, y, z + 1), Data(x, y, z + 1));

			}
		}

	}
	else if (current_axis == 2)
	{
		int z = current_slice[current_axis];
		for (int x = 0; x < vol_dim[0] - 1; x++) {
			for (int y = 0; y < vol_dim[1] - 1; y++) {
				cell = 0;
				if (Data(x, y, z) < current_iso_value - epsilon) cell |= 8;
				if (Data(x + 1, y, z) < current_iso_value - epsilon) cell |= 4;
				if (Data(x + 1, y + 1, z) < current_iso_value - epsilon) cell |= 2;
				if (Data(x, y + 1, z) < current_iso_value - epsilon) cell |= 1;

				GetLinePoints(cell, y, x, vol_dim[1], vol_dim[0], Data(x, y, z), Data(x + 1, y, z), Data(x + 1, y + 1, z), Data(x, y + 1, z));

			}
		}

	}
	
}

void MarchingCubes()
{
	// =============================================================================
	// TODO: Extract 3D surface using marching cubes algorithm
	// =============================================================================
	isosurface.clear();
	
	//Polygonise(GRIDCELL grid, double isolevel, TRIANGLE *triangles)
	//grid - is a set of eight data points(one for each of the eight corners)
	//	p = datapoint xyz location
	//	val = isolevel at that point
	for (int z = 0; z < vol_dim[2] - 1; z++) {
	
		for (int y = 0; y < vol_dim[1] - 1; y++) {

			for (int x = 0; x < vol_dim[0] - 1; x++) {
			
				int cell = 0;

				GRIDCELL grid;

				grid.p[0].x = x; grid.p[0].y = y; grid.p[0].z = z;
				grid.val[0] = Data(x, y, z);

				grid.p[1].x = x; grid.p[1].y = y+1; grid.p[1].z = z;
				grid.val[1] = Data(x, y+1, z);

				grid.p[2].x = x; grid.p[2].y = y + 1; grid.p[2].z = z+1;
				grid.val[2] = Data(x, y+1, z+1);

				grid.p[3].x = x; grid.p[3].y = y; grid.p[3].z = z+1;
				grid.val[3] = Data(x, y, z+1);

				grid.p[4].x = x+1; grid.p[4].y = y; grid.p[4].z = z;
				grid.val[4] = Data(x+1, y, z);

				grid.p[5].x = x + 1; grid.p[5].y = y+1; grid.p[5].z = z;
				grid.val[5] = Data(x+1, y+1, z);

				grid.p[6].x = x + 1; grid.p[6].y = y + 1; grid.p[6].z = z+1;
				grid.val[6] = Data(x+1, y+1, z+1);

				grid.p[7].x = x+1; grid.p[7].y = y; grid.p[7].z = z + 1;
				grid.val[7] = Data(x+1, y, z+1);

				Polygonise(grid, current_iso_value, NULL);
			}
		}
	}

}

void DrawContour(float w, float h)
{
	// =============================================================================
	// TODO: add drawing code for your 2D contours
	// =============================================================================
	
	double x1, y1, x2, y2;
	for (int i = 0; i < contour.size(); i += 4)
	{
		x1 = contour[i] * h;
		y1 = contour[i + 1] * w;
		x2 = contour[i + 2] * h;
		y2 = contour[i + 3] * w;

		

		glColor3f(1.0, 0.0, 0.0);
		glLineWidth(5);
		glBegin(GL_LINES);
		glVertex2d(y1, x1);
		glVertex2d(y2, x2);
		glEnd();
	}
}

void DrawSurface(float w, float h)
{
	// =============================================================================
	// TODO: Replace this nice teapot that has nothing to do with your
	//		 assignment, with actual drawing code for your 3D surface.
	// =============================================================================
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(0.2, 0.5, 0.7);
	if (!data_loaded) glutSolidTeapot(current_iso_value);
	else
	{
		for (int i = 0; i < isosurface.size(); i++)
		{
			float x1 = isosurface[i][0] ;
			float y1 = isosurface[i][1] ;
			float z1 = isosurface[i][2] ;

			float x2 = isosurface[i][3] ;
			float y2 = isosurface[i][4] ;
			float z2 = isosurface[i][5] ;

			float x3 = isosurface[i][6] ;
			float y3 = isosurface[i][7] ;
			float z3 = isosurface[i][8] ;

			XYZ n = normal[i*3];
			glBegin(GL_TRIANGLES);
			glNormal3f(n.x, n.y, n.z);
			glVertex3f(x1, y1, z1);//triangle one first vertex
			
			n = normal[i*3+1];
			glNormal3f(n.x, n.y, n.z);
			glVertex3f(x2, y2, z2);//triangle one second vertex
			
			n = normal[i*3 + 2];
			glNormal3f(n.x, n.y, n.z);
			glVertex3f(x3, y3, z3);
			glEnd();
		}
	}

}

void DrawBoundingBox(void)
{
	if (!data_loaded) {
		return;
	}

	float size = 1.0f;

	glColor3f(1.0, 1.0, 1.0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	float A[3] = { size, size, size };
	float B[3] = { size, size, -size };
	float C[3] = { size, -size, -size };
	float D[3] = { size, -size, size };
	float E[3] = { -size, size, size };
	float F[3] = { -size, size, -size };
	float G[3] = { -size, -size, -size };
	float H[3] = { -size, -size, size };

	glBegin(GL_QUADS);

	glVertex3fv(D);
	glVertex3fv(C);
	glVertex3fv(B);
	glVertex3fv(A);

	glVertex3fv(G);
	glVertex3fv(H);
	glVertex3fv(E);
	glVertex3fv(F);

	glVertex3fv(C);
	glVertex3fv(G);
	glVertex3fv(F);
	glVertex3fv(B);

	glVertex3fv(H);
	glVertex3fv(D);
	glVertex3fv(A);
	glVertex3fv(E);

	glVertex3fv(E);
	glVertex3fv(A);
	glVertex3fv(B);
	glVertex3fv(F);

	glVertex3fv(G);
	glVertex3fv(C);
	glVertex3fv(D);
	glVertex3fv(H);

	glEnd();
}