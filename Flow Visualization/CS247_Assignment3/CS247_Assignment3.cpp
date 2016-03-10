//
// CS247_Assignment4
// AMCS/CS247 Programming Assignment 4
//

#include <stdio.h>
#include <math.h>
#include <string>

#include "CS247_Assignment3.h"

int printOglError(char *file, int line)
{
	//
	// Returns 1 if an OpenGL error occurred, 0 otherwise.
	//
	GLenum glErr;
	int retCode = 0;

	glErr = glGetError();
	while (glErr != GL_NO_ERROR) {
		printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
		retCode = 1;
		glErr = glGetError();
	}
	return retCode;
}


/***************************************************************************/
/* Parse GL_VERSION and return the major and minor numbers in the supplied
* integers.
* If it fails for any reason, major and minor will be set to 0.
* Assumes a valid OpenGL context.
*/

void getGlVersion(int *major, int *minor)
{
	const char* verstr = (const char*)glGetString(GL_VERSION);
	if ((verstr == NULL) || (sscanf(verstr, "%d.%d", major, minor) != 2)) {
		*major = *minor = 0;
		fprintf(stderr, "Invalid GL_VERSION format!!!\n");
	}
}


/*
rotating background color
*/
static void NextClearColor(void)
{
	static int color = 0;

	switch (color++)
	{
	case 0:
		clear_color[0] = 0.0f;
		clear_color[1] = 0.0f;
		clear_color[2] = 0.0f;
		clear_color[3] = 1.0f;
		break;
	case 1:
		clear_color[0] = 0.2f;
		clear_color[1] = 0.2f;
		clear_color[2] = 0.3f;
		clear_color[3] = 1.0f;
		break;
	default:
		clear_color[0] = 0.7f;
		clear_color[1] = 0.7f;
		clear_color[2] = 0.7f;
		clear_color[3] = 1.0f;
		color = 0;
		break;
	}
	glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
}


void draw_arrow_head(float head[2], float direct[2])
{
	float M_PI = 3.1415926;
	glPushMatrix();
	glTranslatef(head[0], head[1], 0);
	glRotatef(atan2(direct[1], direct[0]) * 360 / (2 * M_PI), 0, 0, 1);
	glScalef(0.03, 0.03, 1);
	glBegin(GL_TRIANGLES);
	glVertex2f(0, 0);
	glVertex2f(-0.35, 0.12);
	glVertex2f(-0.35, -0.12);
	glEnd();
	glPopMatrix();
}

/******************************************************************************/
/*
* GLUT glue
*
******************************************************************************/

static
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// TODO:
	// Draw everything here as needed
	// you can use normalized coordinates as the viewport is scaled to 0..1 for x and y
	
	

	


	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, scalar_field_texture);


	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);
	glTexCoord2f(0, 0); glVertex2f(0, 0);
	glTexCoord2f(1, 0); glVertex2f(1, 0);
	glTexCoord2f(1, 1); glVertex2f(1, 1);
	glTexCoord2f(0, 1); glVertex2f(0, 1);
	glEnd();


	glDisable(GL_TEXTURE_2D);

	int data_size = vol_dim[0] * vol_dim[1] * vol_dim[2];

	float scale = 0.4;
	if (draw_arrows == true)
	{
		float Vx, Vy, Vz;
		
		for (int j = 0; j < vol_dim[1]; j += samp_rate)
		{
			for (int i = 0; i < vol_dim[0]; i += samp_rate)
			{
				int index = j * vol_dim[0] + i;

				Vx = vector_array[3 * index + 0 + 3 * loaded_timestep * data_size];
				Vy = vector_array[3 * index + 1 + 3 * loaded_timestep * data_size];
				Vz = vector_array[3 * index + 2 + 3 * loaded_timestep * data_size];
				
				if ((Vx*Vx + Vy*Vy) < 0.000001)
					continue;

				
				float x = i; //-scale*Vx / 2.0;
				float y = j;// -scale*Vy / 2.0;
				x /= vol_dim[0];
				y /= vol_dim[1];

				float x1 = i + scale*Vx;// / 2.0;
				float y1 = j + scale*Vy;// / 2.0;
				x1 /= vol_dim[0];
				y1 /= vol_dim[1];

				float head[2], direct[2];
				head[0] = x1;
				head[1] = y1;

				direct[0] = Vx*scale;
				direct[1] = Vy*scale;

				glColor3f(1.0, 1.0, 0.0);
				if ((x - x1)*(x - x1) + (y - y1)*(y - y1) > 0.000001)
				{
					draw_arrow_head(head, direct);

					glLineWidth(2);
					glBegin(GL_LINES);
					glColor3f(1.0, 1.0, 0.0);
					glVertex2d(x1, y1);
					glVertex2d(x, y);
					glEnd();
				}
				
				
				
			}
		}
	}

	

	for (int i = 0; i < stream_pos.size(); i+=2)
	{
		if (isStreamline == true)
		{
			computeStreamline(stream_pos[i], stream_pos[i+1]);
		}
		if (isPathline == true)
		{
			computePathline(stream_pos[i], stream_pos[i + 1]);
		}
	}

	if (isclickMouse == true)
	{
		stream_pos.push_back(seed_x);
		stream_pos.push_back(seed_y);
	}
	
	if (isStreamline == true && isclickMouse == true)
	{
		computeStreamline(seed_x, seed_y);
		isclickMouse = false;
		
	}
	if (isPathline == true && isclickMouse == true)
	{
		computePathline(seed_x, seed_y);
		isclickMouse = false;
		
	}

	

	glFlush();
	glutSwapBuffers();


}


static
void play(void)
{
	int thisTime = glutGet(GLUT_ELAPSED_TIME);

	glutPostRedisplay();
}


static
void key(unsigned char keyPressed, int x, int y) // key handling
{
	char* status[2];
	status[0] = "disabled";
	status[1] = "enabled";

	switch (keyPressed) {
	case '1':
		toggle_xy = 0;
		LoadData(filenames[0]);
		loaded_file = 0;
		stream_pos.clear();
		fprintf(stderr, "Loading ");
		fprintf(stderr, filenames[0]);
		fprintf(stderr, " dataset.\n");
		break;
	case '2':
		toggle_xy = 0;
		LoadData(filenames[1]);
		loaded_file = 1;
		stream_pos.clear();
		fprintf(stderr, "Loading ");
		fprintf(stderr, filenames[1]);
		fprintf(stderr, " dataset.\n");
		break;
	case '3':
		toggle_xy = 1;
		LoadData(filenames[2]);
		loaded_file = 2;
		stream_pos.clear();
		fprintf(stderr, "Loading ");
		fprintf(stderr, filenames[2]);
		fprintf(stderr, " dataset.\n");
		break;
	case '0':
		if (num_timesteps > 1){
			loadNextTimestep();
			stream_pos.clear();
			fprintf(stderr, "Timestep %d.\n", loaded_timestep);
		}
		break;
	case 's':
		stream_pos.clear();
		current_scalar_field = (current_scalar_field + 1) % num_scalar_fields;
		DownloadScalarFieldAsTexture();
		fprintf(stderr, "Scalar field changed.\n");
		break;
	case 'b':
		stream_pos.clear();
		NextClearColor();
		fprintf(stderr, "Next clear color.\n");
		break;
	case 'i':
		dt += 0.005;
		stream_pos.clear();
		fprintf(stderr, "Increase dt to %f.\n", dt);
		break;
	case 'k':
		dt -= 0.005;
		stream_pos.clear();
		fprintf(stderr, "Decrease dt to %f.\n", dt);
		break;
	case 'q':
	case 27:
		exit(0);
		break;
	case '+':
		samp_rate += 5;
		stream_pos.clear();
		fprintf(stderr, "Increase sample rate to %d.\n", samp_rate);
		break;
	case '-':
		samp_rate -= 5;
		stream_pos.clear();
		samp_rate = samp_rate > 5? samp_rate : 5;
		fprintf(stderr, "Decrease sample rate to %d.\n", samp_rate);
		break;
	case '4':
		isStreamline = !isStreamline;
		stream_pos.clear();
		fprintf(stderr, "%s streamline status\n", isStreamline ? "enable" : "disable");
			
		break;
	case '5':
		isPathline = !isPathline;
		stream_pos.clear();
		fprintf(stderr, "%s pathline status\n", isPathline ? "enable":"disable");
		break;
	case '6':
		isEuler = !isEuler;
		
		fprintf(stderr, "Switch to %s\n", isEuler  ? "Euler" : "RK2");
		break;
	case '7':
		stream_pos.clear();
		if (isStreamline == true)
		{
			for (int i = 0; i < vol_dim[0]; i+=samp_rate)
			{
				stream_pos.push_back(i);
				stream_pos.push_back(seed_y);
			}
		}
		fprintf(stderr, "Release multiple streamline seeds in horizontal rake\n");
		break;
	case '8':
		stream_pos.clear();
		if (isStreamline == true)
		{
			for (int i = 0; i < vol_dim[1]; i += samp_rate)
			{
				stream_pos.push_back(seed_x);
				stream_pos.push_back(i);
			}
		}
		fprintf(stderr, "Release multiple streamline seeds in vertical rake\n");
		break;
	case 'r':
		draw_arrows = !draw_arrows;
		fprintf(stderr, "Switch draw arrows status to %d.\n", draw_arrows);
		break;
	default:
		fprintf(stderr, "\nKeyboard commands:\n\n"
			"1, load %s dataset\n"
			"2, load %s dataset\n"
			"3, load %s dataset\n"
			"4, enable/disable streamline status\n"
			"5, enable/disable pathline status\n"
			"6, switch Euler/RK2 method\n"
			"7£¬Release multiple streamline seeds in horizontal rake\n"
			"8£¬Release multiple streamline seeds in vertical rake\n"
			"0, cycle through timesteps\n"
			"i, increase dt\n"
			"k, decrease dt\n"
			"+, increase sample rate\n"
			"-, decrease sample rate\n"
			"b, switch background color\n"
			"r, enable/disable draw arrows status\n"
			"q, <esc> - Quit\n",
			filenames[0], filenames[1], filenames[2]);
		break;
	}
}



static
void timer(int value)
{
	glutTimerFunc(TIMER_FREQUENCY_MILLIS, timer, 0);
}


static
void mouse(int button, int state, int x, int y)
{
	bmModifiers = glutGetModifiers();

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		// TODO:
		// seed streamlines & pathlines using mouseclicks

		seed_x = x;
		seed_y = y;
		isclickMouse = true;
	
	}
}


static
void motion(int x, int y)
{
}


static
void reshape(int wid, int ht)
{
	view_width = wid;
	view_height = ht;

	// set viewport
	glViewport(0, 0, wid, ht);

	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	gluOrtho2D(0, 1, 0, 1);
}


static
void special(int key, int x, int y)
{
	//handle special keys
	switch (key) {
	case GLUT_KEY_HOME: //return to inital state/view
		break;
	case GLUT_KEY_LEFT:
		break;
	case GLUT_KEY_RIGHT:
		break;
	case GLUT_KEY_UP:
		break;
	case GLUT_KEY_DOWN:
		break;
	default:
		break;
	}
}

float PosTransform(float x, int dim)
{
	//if (dim == 1)
	//	return 1 - x / vol_dim[dim];
	return x/vol_dim[dim];
}

float Vec_Arr(int x, int y, int dim)
{
	int data_size = vol_dim[0] * vol_dim[1] * vol_dim[2];
	return vector_array[3 * (y*vol_dim[0] + x) + dim + 3 * loaded_timestep*data_size];
}

float Vec_Arr(int x, int y, int dim, int loaded_timestep)
{
	int data_size = vol_dim[0] * vol_dim[1] * vol_dim[2];
	return vector_array[3 * (y*vol_dim[0] + x) + dim + 3 * loaded_timestep*data_size];
}

float bilinearInterp(float x1, float y1, int dim, float current_time_steps)
{
	//bilinear interpolation
	float Xs = floor(x1);
	float Xe = ceil(x1);
	float Ys = floor(y1);
	float Ye = ceil(y1);

	if (Xs == Xe && Ys != Ye)
		return Vec_Arr(x1, Ye, dim, current_time_steps)*(y1 - Ys) + Vec_Arr(x1, Ys, dim, current_time_steps)*(Ye - y1);

	if (Ys == Ye && Xs != Xe)
		return Vec_Arr(Xe, y1, dim, current_time_steps)*(x1 - Xs) + Vec_Arr(Xs, y1, dim, current_time_steps)*(Xe - x1);

	if (Xs == Xe && Ys == Ye)
		return Vec_Arr(x1, y1, dim, current_time_steps);
	 

	//bilinear
	return    Vec_Arr(Xs, Ye, dim, current_time_steps)*(Xe - x1)*(y1 - Ys)
		+ Vec_Arr(Xe, Ye, dim, current_time_steps)*(x1 - Xs)*(y1 - Ys)
		+ Vec_Arr(Xs, Ys, dim, current_time_steps)*(Xe - x1)*(Ye - y1)
		+ Vec_Arr(Xe, Ys, dim, current_time_steps)*(x1 - Xs)*(Ye - y1);

}

void trilinearInterp(float x1, float y1, float current_time_steps, float* Vx, float* Vy, float *Vz)
{
	int low_time_steps = floor(current_time_steps);
	int up_time_steps = ceil(current_time_steps);

	//trilinear interpolation

	//low bound bilinear
	float Vx1, Vy1, Vz1;
	Vx1 = bilinearInterp(x1, y1, 0, low_time_steps);
	Vy1 = bilinearInterp(x1, y1, 1, low_time_steps);
	Vz1 = bilinearInterp(x1, y1, 2, low_time_steps);

	//up bound bilinear
	float Vx2, Vy2, Vz2;
	Vx2 = bilinearInterp(x1, y1, 0, up_time_steps);
	Vy2 = bilinearInterp(x1, y1, 1, up_time_steps);
	Vz2 = bilinearInterp(x1, y1, 2, up_time_steps);

	//linear interpolation
	*Vx = Vx1*(up_time_steps - current_time_steps) + Vx2*(current_time_steps - low_time_steps);
	*Vy = Vy1*(up_time_steps - current_time_steps) + Vy2*(current_time_steps - low_time_steps);
	*Vz = Vz1*(up_time_steps - current_time_steps) + Vz2*(current_time_steps - low_time_steps);

}


void Euler(float x, float y, float threshold, float Vx, float Vy)
{


	float reverse_Vx = -Vx;
	float reverse_Vy = -Vy;
	float x1;
	float y1;
	//float scale = 3.0;
	int max_iter = 80000;
	int i = 0;
	while (x < vol_dim[0] && y < vol_dim[1] && (Vx*Vx + Vy*Vy) > threshold && i < max_iter)
	{
		//normalize
		Vx = Vx / sqrt(Vx*Vx + Vy*Vy);
		Vy = Vy / sqrt(Vx*Vx + Vy*Vy);

		
		//next postion
		x1 = x + Vx*dt;
		y1 = y + Vy*dt;


		//draw line
		glLineWidth(2);
		glBegin(GL_LINES);
		glColor3f(0.0, 1.0, 0.0);
		glVertex2d(PosTransform(x, 0), PosTransform(y, 1));
		glVertex2d(PosTransform(x1, 0), PosTransform(y1, 1));
		glEnd();


		//bilinear interpolation

		if (x1 + 1 > vol_dim[0] || y1 + 1 > vol_dim[1])
			break;
		Vx = bilinearInterp(x1, y1, 0, loaded_timestep);
		Vy = bilinearInterp(x1, y1, 1, loaded_timestep);

		x = x1;
		y = y1;
		i++;
		
	}

	i = 0;
	Vx = reverse_Vx;
	Vy = reverse_Vy;
	while (x > 0 && y > 0 && (Vx*Vx + Vy*Vy) > threshold && i < max_iter)
	{

		//normalize
		Vx = Vx / sqrt(Vx*Vx + Vy*Vy);
		Vy = Vy / sqrt(Vx*Vx + Vy*Vy);

		//next postion
		x1 = x + Vx*dt;
		y1 = y + Vy*dt;


		//draw line
		glLineWidth(2);
		glBegin(GL_LINES);
		glColor3f(0.0, 1.0, 0.0);
		glVertex2d(PosTransform(x, 0), PosTransform(y, 1));
		glVertex2d(PosTransform(x1, 0), PosTransform(y1, 1));
		glEnd();


		//bilinear interpolation

		if (x1 - 1 < 0 || y1 - 1 < 0)
			break;
		Vx = -bilinearInterp(x1, y1, 0, loaded_timestep);
		Vy = -bilinearInterp(x1, y1, 1, loaded_timestep);

		x = x1;
		y = y1;
		i++;

	}
	//printf("steps: %d, x = %.2f, y = %.2f\n", i, x, y);
	//printf("vol_dim[0]: %d, vol_dim[1]: %d\n", vol_dim[0], vol_dim[1]);
	
	
}

void RK2(float x, float y, float threshold, float Vx, float Vy)
{


	float reverse_Vx = -Vx;
	float reverse_Vy = -Vy;
	float x1;
	float y1;
	//float scale = 3.0;
	int max_iter = 80000;
	int i = 0;
	while (x < vol_dim[0] && y < vol_dim[1] && (Vx*Vx + Vy*Vy) > threshold && i < max_iter)
	{

		//normalize
		Vx = Vx / sqrt(Vx*Vx + Vy*Vy);
		Vy = Vy / sqrt(Vx*Vx + Vy*Vy);

		//next postion
		x1 = x + Vx*dt / 2;
		y1 = y + Vy*dt / 2;

		if (x1 + 1 > vol_dim[0] || y1 + 1 > vol_dim[1])
			break;

		//bilinear interpolation
		Vx = bilinearInterp(x1, y1, 0, loaded_timestep);
		Vy = bilinearInterp(x1, y1, 1, loaded_timestep);

		//normalize
		Vx = Vx / sqrt(Vx*Vx + Vy*Vy);
		Vy = Vy / sqrt(Vx*Vx + Vy*Vy);

		x1 = x + Vx*dt;
		y1 = y + Vy*dt;
		


		//draw line
		glLineWidth(5);
		glBegin(GL_LINES);
		glColor3f(0.0, 1.0, 0.0);
		glVertex2d(PosTransform(x, 0), PosTransform(y, 1));
		glVertex2d(PosTransform(x1, 0), PosTransform(y1, 1));
		glEnd();

		if (x1 + 1 > vol_dim[0] || y1 + 1 > vol_dim[1])
			break;

		//bilinear interpolation
		Vx = bilinearInterp(x1, y1, 0, loaded_timestep);
		Vy = bilinearInterp(x1, y1, 1, loaded_timestep);

		x = x1;
		y = y1;
		i++;
	}

	i = 0;
	Vx = reverse_Vx;
	Vy = reverse_Vy;
	while (x > 0 && y > 0 && (Vx*Vx + Vy*Vy) > threshold && i < max_iter)
	{

		//normalize
		Vx = Vx / sqrt(Vx*Vx + Vy*Vy);
		Vy = Vy / sqrt(Vx*Vx + Vy*Vy);

		//next postion
		x1 = x + Vx*dt / 2;
		y1 = y + Vy*dt / 2;

		if (x1 - 1 < 0 || y1 - 1 < 0)
			break;

		//bilinear interpolation
		Vx = -bilinearInterp(x1, y1, 0, loaded_timestep);
		Vy = -bilinearInterp(x1, y1, 1, loaded_timestep);

		//normalize
		Vx = Vx / sqrt(Vx*Vx + Vy*Vy);
		Vy = Vy / sqrt(Vx*Vx + Vy*Vy);

		x1 = x + Vx*dt;
		y1 = y + Vy*dt;



		//draw line
		glLineWidth(5);
		glBegin(GL_LINES);
		glVertex2d(PosTransform(x, 0), PosTransform(y, 1));
		glVertex2d(PosTransform(x1, 0), PosTransform(y1, 1));
		glEnd();

		if (x1 - 1 < 0 || y1 - 1 < 0)
			break;

		//bilinear interpolation
		Vx = -bilinearInterp(x1, y1, 0, loaded_timestep);
		Vy = -bilinearInterp(x1, y1, 1, loaded_timestep);

		x = x1;
		y = y1;
		i++;

	}
}

void RK4(float x, float y, float threshold, float Vx, float Vy)
{


	float reverse_Vx = -Vx;
	float reverse_Vy = -Vy;
	float x1;
	float y1;
	//float scale = 3.0;
	int max_iter = 80000;
	int i = 0;
	while (x < vol_dim[0] && y < vol_dim[1] && (Vx*Vx + Vy*Vy) > threshold && i < max_iter)
	{

		//normalize
		Vx = Vx / sqrt(Vx*Vx + Vy*Vy);
		Vy = Vy / sqrt(Vx*Vx + Vy*Vy);

		float ax = Vx*dt;
		float ay = Vy*dt;

		//next postion
		x1 = x + ax / 2;
		y1 = y + ay / 2;

		if (x1 + 1 > vol_dim[0] || y1 + 1 > vol_dim[1])
			break;

		//bilinear interpolation
		Vx = bilinearInterp(x1, y1, 0, loaded_timestep);
		Vy = bilinearInterp(x1, y1, 1, loaded_timestep);

		//normalize
		Vx = Vx / sqrt(Vx*Vx + Vy*Vy);
		Vy = Vy / sqrt(Vx*Vx + Vy*Vy);

		float bx = Vx*dt;
		float by = Vy*dt;
		x1 = x + bx / 2;
		y1 = y + by / 2;

		if (x1 + 1 > vol_dim[0] || y1 + 1 > vol_dim[1])
			break;

		//bilinear interpolation
		Vx = bilinearInterp(x1, y1, 0, loaded_timestep);
		Vy = bilinearInterp(x1, y1, 1, loaded_timestep);

		//normalize
		Vx = Vx / sqrt(Vx*Vx + Vy*Vy);
		Vy = Vy / sqrt(Vx*Vx + Vy*Vy);
		float cx = Vx*dt;
		float cy = Vy*dt;
		x1 = x + cx;
		y1 = y + cy;

		if (x1 + 1 > vol_dim[0] || y1 + 1 > vol_dim[1])
			break;

		//bilinear interpolation
		Vx = bilinearInterp(x1, y1, 0, loaded_timestep);
		Vy = bilinearInterp(x1, y1, 1, loaded_timestep);

		float dx = Vx * dt;
		float dy = Vy * dt;

		x1 = x + (ax + 2 * bx + 2 * cx + dx) / 6.0;
		y1 = y + (ay + 2 * by + 2 * cy + dy) / 6.0;

		//draw line
		glLineWidth(5);
		glBegin(GL_LINES);
		glColor3f(0.0, 1.0, 0.0);
		glVertex2d(PosTransform(x, 0), PosTransform(y, 1));
		glVertex2d(PosTransform(x1, 0), PosTransform(y1, 1));
		glEnd();

		if (x1 + 1 > vol_dim[0] || y1 + 1 > vol_dim[1])
			break;

		//bilinear interpolation
		Vx = bilinearInterp(x1, y1, 0, loaded_timestep);
		Vy = bilinearInterp(x1, y1, 1, loaded_timestep);

		x = x1;
		y = y1;
		i++;
	}

	i = 0;
	Vx = reverse_Vx;
	Vy = reverse_Vy;
	while (x > 0 && y > 0 && (Vx*Vx + Vy*Vy) > threshold && i < max_iter)
	{


		//normalize
		Vx = Vx / sqrt(Vx*Vx + Vy*Vy);
		Vy = Vy / sqrt(Vx*Vx + Vy*Vy);

		float ax = Vx*dt;
		float ay = Vy*dt;

		//next postion
		x1 = x + ax / 2;
		y1 = y + ay / 2;

		if (x1 - 1 < 0 || y1 - 1 < 0)
			break;

		//bilinear interpolation
		Vx = -bilinearInterp(x1, y1, 0, loaded_timestep);
		Vy = -bilinearInterp(x1, y1, 1, loaded_timestep);

		//normalize
		Vx = Vx / sqrt(Vx*Vx + Vy*Vy);
		Vy = Vy / sqrt(Vx*Vx + Vy*Vy);

		float bx = Vx*dt;
		float by = Vy*dt;
		x1 = x + bx / 2;
		y1 = y + by / 2;

		if (x1 - 1 < 0 || y1 - 1 < 0)
			break;

		//bilinear interpolation
		Vx = -bilinearInterp(x1, y1, 0, loaded_timestep);
		Vy = -bilinearInterp(x1, y1, 1, loaded_timestep);

		//normalize
		Vx = Vx / sqrt(Vx*Vx + Vy*Vy);
		Vy = Vy / sqrt(Vx*Vx + Vy*Vy);
		float cx = Vx*dt;
		float cy = Vy*dt;
		x1 = x + cx;
		y1 = y + cy;

		if (x1 - 1 < 0 || y1 - 1 < 0)
			break;

		//bilinear interpolation
		Vx = -bilinearInterp(x1, y1, 0, loaded_timestep);
		Vy = -bilinearInterp(x1, y1, 1, loaded_timestep);

		float dx = Vx * dt;
		float dy = Vy * dt;

		x1 = x + (ax + 2 * bx + 2 * cx + dx) / 6.0;
		y1 = y + (ay + 2 * by + 2 * cy + dy) / 6.0;

		//draw line
		glLineWidth(5);
		glBegin(GL_LINES);
		glVertex2d(PosTransform(x, 0), PosTransform(y, 1));
		glVertex2d(PosTransform(x1, 0), PosTransform(y1, 1));
		glEnd();

		if (x1 - 1 < 0 || y1 - 1 < 0)
			break;

		//bilinear interpolation
		Vx = -bilinearInterp(x1, y1, 0, loaded_timestep);
		Vy = -bilinearInterp(x1, y1, 1, loaded_timestep);

		x = x1;
		y = y1;
		i++;

	}
}

void computeStreamline(float x, float y)
{
	// TODO:
	// compute streamlines starting from x,y position
	// enable switching between euler and runge kutta


	float threshold = 0.000001;

	float Vx = Vec_Arr(x, view_height - y, 0);
	float Vy = Vec_Arr(x, view_height - y, 1);

	if (isEuler == true && (Vx*Vx + Vy*Vy) > threshold)
	{
		Euler(x, view_height - y, threshold, Vx, Vy);
	}
	else if (isEuler == false && (Vx*Vx + Vy*Vy) > threshold)
	{
		//rk2
		//RK2(x, view_height - y, threshold, Vx, Vy);
		RK4(x, view_height - y, threshold, Vx, Vy);
		
	}

}



void Euler3D(float x, float y, float threshold, float Vx, float Vy, float Vz)
{
	float reverse_Vx = -Vx;
	float reverse_Vy = -Vy;
	float reverse_Vz = -Vz;
	
	

	float x1;
	float y1;
	float current_time_steps = loaded_timestep;
	int max_iter = 100000;
	int i = 0;
	while (x < vol_dim[0] && y < vol_dim[1] && (Vx*Vx + Vy*Vy + Vz*Vz) > threshold && i < max_iter)
	{

		//normalize
		Vx = Vx / sqrt(Vx*Vx + Vy*Vy + Vz*Vz);
		Vy = Vy / sqrt(Vx*Vx + Vy*Vy + Vz*Vz);
		Vz = Vz / sqrt(Vx*Vx + Vy*Vy + Vz*Vz);

		//next postion
		x1 = x + Vx*dt;
		y1 = y + Vy*dt;
		current_time_steps += Vz*dt;

		if (x1 + 1 > vol_dim[0] || y1 + 1 > vol_dim[1] || current_time_steps + 1 > num_timesteps)
			break;
		if (x1 - 1 < 0 || y1 - 1 < 0 || current_time_steps - 1 < 0)
			break;
		
		trilinearInterp(x1, y1, current_time_steps, &Vx, &Vy, &Vz);

		//draw line
		glLineWidth(5);
		glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 0.0);
		glVertex2d(PosTransform(x, 0), PosTransform(y, 1));
		glVertex2d(PosTransform(x1, 0), PosTransform(y1, 1));
		glEnd();

		x = x1;
		y = y1;
		i++;
	}

	
	i = 0;
	Vx = reverse_Vx;
	Vy = reverse_Vy;
	Vz = reverse_Vz;

	while (x > 0 && y > 0 && (Vx*Vx + Vy*Vy + Vz*Vz) > threshold && i < max_iter)
	{

		//normalize
		Vx = Vx / sqrt(Vx*Vx + Vy*Vy + Vz*Vz);
		Vy = Vy / sqrt(Vx*Vx + Vy*Vy + Vz*Vz);
		Vz = Vz / sqrt(Vx*Vx + Vy*Vy + Vz*Vz);

		//next postion
		x1 = x + Vx*dt;
		y1 = y + Vy*dt;
		current_time_steps += Vz*dt;

		
		if (x1 + 1 > vol_dim[0] || y1 + 1 > vol_dim[1] || current_time_steps + 1 > num_timesteps)
			break;
		if (x1 - 1 < 0 || y1 - 1 < 0 || current_time_steps - 1 < 0)
			break;

		trilinearInterp(x1, y1, current_time_steps, &Vx, &Vy, &Vz);
		Vx = -Vx;
		Vy = -Vy;
		Vz = -Vz;

		//draw line
		glLineWidth(5);
		glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 0.0);
		glVertex2d(PosTransform(x, 0), PosTransform(y, 1));
		glVertex2d(PosTransform(x1, 0), PosTransform(y1, 1));
		glEnd();

		x = x1;
		y = y1;
		i++;
	}
	
}





void RK2_3D(float x, float y, float threshold, float Vx, float Vy, float Vz)
{


	float reverse_Vx = -Vx;
	float reverse_Vy = -Vy;
	float reverse_Vz = -Vz;

	float x1;
	float y1;
	float current_time_steps = loaded_timestep;
	int max_iter = 100000;
	int i = 0;
	while (x < vol_dim[0] && y < vol_dim[1] && (Vx*Vx + Vy*Vy + Vz*Vz) > threshold && i < max_iter)
	{

		//normalize
		Vx = Vx / sqrt(Vx*Vx + Vy*Vy + Vz*Vz);
		Vy = Vy / sqrt(Vx*Vx + Vy*Vy + Vz*Vz);
		Vz = Vz / sqrt(Vx*Vx + Vy*Vy + Vz*Vz);

		//next postion
		x1 = x + Vx*dt / 2;
		y1 = y + Vy*dt / 2;
		float curr_step_1 = current_time_steps + Vz*dt / 2;

		trilinearInterp(x1, y1, curr_step_1, &Vx, &Vy, &Vz);


		if (x1 + 1 > vol_dim[0] || y1 + 1 > vol_dim[1] || curr_step_1 + 1 > num_timesteps)
			break;
		if (x1 - 1 < 0 || y1 - 1 < 0 || curr_step_1 - 1 < 0)
			break;

		//normalize
		Vx = Vx / sqrt(Vx*Vx + Vy*Vy + Vz*Vz);
		Vy = Vy / sqrt(Vx*Vx + Vy*Vy + Vz*Vz);
		Vz = Vz / sqrt(Vx*Vx + Vy*Vy + Vz*Vz);

		//next postion
		x1 = x + Vx*dt;
		y1 = y + Vy*dt;
		current_time_steps += Vz*dt;

		if (x1 + 1 > vol_dim[0] || y1 + 1 > vol_dim[1] || current_time_steps + 1 > num_timesteps)
			break;
		if (x1 - 1 < 0 || y1 - 1 < 0 || current_time_steps - 1 < 0)
			break;

		trilinearInterp(x1, y1, current_time_steps, &Vx, &Vy, &Vz);

		//draw line
		glLineWidth(5);
		glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 0.0);
		glVertex2d(PosTransform(x, 0), PosTransform(y, 1));
		glVertex2d(PosTransform(x1, 0), PosTransform(y1, 1));
		glEnd();

		x = x1;
		y = y1;
		i++;
	}


	i = 0;
	Vx = reverse_Vx;
	Vy = reverse_Vy;
	Vz = reverse_Vz;

	while (x > 0 && y > 0 && (Vx*Vx + Vy*Vy + Vz*Vz) > threshold && i < max_iter)
	{

		//normalize
		Vx = Vx / sqrt(Vx*Vx + Vy*Vy + Vz*Vz);
		Vy = Vy / sqrt(Vx*Vx + Vy*Vy + Vz*Vz);
		Vz = Vz / sqrt(Vx*Vx + Vy*Vy + Vz*Vz);

		//next postion
		x1 = x + Vx*dt / 2;
		y1 = y + Vy*dt / 2;
		float curr_step_1 = current_time_steps + Vz*dt / 2;

		trilinearInterp(x1, y1, curr_step_1, &Vx, &Vy, &Vz);
		Vx = -Vx;
		Vy = -Vy;
		Vz = -Vz;

		if (x1 + 1 > vol_dim[0] || y1 + 1 > vol_dim[1] || curr_step_1 + 1 > num_timesteps)
			break;
		if (x1 - 1 < 0 || y1 - 1 < 0 || curr_step_1 - 1 < 0)
			break;

		//normalize
		Vx = Vx / sqrt(Vx*Vx + Vy*Vy + Vz*Vz);
		Vy = Vy / sqrt(Vx*Vx + Vy*Vy + Vz*Vz);
		Vz = Vz / sqrt(Vx*Vx + Vy*Vy + Vz*Vz);

		//next postion
		x1 = x + Vx*dt;
		y1 = y + Vy*dt;
		current_time_steps += Vz*dt;

		if (x1 + 1 > vol_dim[0] || y1 + 1 > vol_dim[1] || current_time_steps + 1 > num_timesteps)
			break;
		if (x1 - 1 < 0 || y1 - 1 < 0 || current_time_steps - 1 < 0)
			break;

		trilinearInterp(x1, y1, current_time_steps, &Vx, &Vy, &Vz);
		Vx = -Vx;
		Vy = -Vy;
		Vz = -Vz;
		//draw line
		glLineWidth(5);
		glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 0.0);
		glVertex2d(PosTransform(x, 0), PosTransform(y, 1));
		glVertex2d(PosTransform(x1, 0), PosTransform(y1, 1));
		glEnd();

		x = x1;
		y = y1;
		i++;
	}


}


void computePathline(float x, float y)
{
	// TODO:
	// compute pathlines starting from x,y position and time step t
	// enable switching between euler and runge kutta

	float threshold = 0.000001;

	float Vx = Vec_Arr(x, view_height - y, 0);
	float Vy = Vec_Arr(x, view_height - y, 1);
	float Vz = Vec_Arr(x, view_height - y, 2);
	
	if (isEuler == true && (Vx*Vx + Vy*Vy + Vz*Vz) > threshold)
	{
		Euler3D(x, view_height - y, threshold, Vx, Vy, Vz);
		//Euler3D(x, y, threshold, -Vx, -Vy, -Vz);
		
	}
	else if ((isEuler == false && (Vx*Vx + Vy*Vy + Vz*Vz) > threshold))
	{
		//rk2
		//RK2_3D(x, view_height - y, threshold, Vx, Vy, Vz);
		RK2_3D(x, view_height - y, threshold, Vx, Vy, Vz);
	}
}


void loadNextTimestep(void)
{
	loaded_timestep = (loaded_timestep + 1) % num_timesteps;
	DownloadScalarFieldAsTexture();
}


/*
* load .gri dataset
* This only reads the header information and calls the dat loader
* For now we ignore the grid data and assume a rectangular grid
*/
void LoadData(char* base_filename)
{
	//reset
	reset_rendering_props();

	char filename[80];
	strcpy(filename, base_filename);
	strcat(filename, ".gri");

	fprintf(stderr, "loading grid file %s\n", filename);

	// open grid file, read only, binary mode 
	FILE* fp = fopen(filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open file %s for reading.\n", filename);
		return;
	}

	// read header
	char header[40];
	fread(header, sizeof(char), 40, fp);
	sscanf(header, "SN4DB %d %d %d %d %d %f",
		&vol_dim[0], &vol_dim[1], &vol_dim[2],
		&num_scalar_fields, &num_timesteps, &timestep);

	fprintf(stderr, "dimensions: x: %d, y: %d, z: %d.\n", vol_dim[0], vol_dim[1], vol_dim[2]);
	fprintf(stderr, "additional info: # scalar fields: %d, # timesteps: %d, timestep: %f.\n", num_scalar_fields, num_timesteps, timestep);

	// read data
	/*	if ( vector_array != NULL ) {
	delete[] vector_array;
	}

	int data_size = vol_dim[ 0 ] * vol_dim[ 1 ] * vol_dim[ 2 ];

	// 1D array for storing volume data
	grid_array = new vec[ data_size ];

	// read grid data
	fread( grid_array, sizeof( float ), ( data_size * 3 ), fp );

	// close file
	fclose( fp );
	*/
	char dat_filename[80];
	strcpy(dat_filename, base_filename);

	if (num_timesteps <= 1){

		strcat(dat_filename, ".dat");

	}
	else {

		strcat(dat_filename, ".00000.dat");

	}

	loaded_timestep = 0;
	LoadVectorData(base_filename);

	glutReshapeWindow(vol_dim[0], vol_dim[1]);

	grid_data_loaded = true;
}

/*
* load .dat dataset
* loads vector and scalar fields
*/
void LoadVectorData(const char* filename)
{
	fprintf(stderr, "loading scalar file %s\n", filename);

	// open data file, read only, binary mode 
	char ts_name[80];
	if (num_timesteps > 1)
	{
		sprintf(ts_name, "%s.%.5d.dat", filename, 0);
	}
	else
		sprintf(ts_name, "%s.dat", filename);

	FILE* fp = fopen(ts_name, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open file %s for reading.\n", filename);
		return;
	}
	else
	{
		fclose(fp);
	}

	int data_size = vol_dim[0] * vol_dim[1] * vol_dim[2];

	if (vector_array != NULL) {
		delete[] vector_array;
		vector_array = NULL;
	}
	vector_array = new float[data_size * 3 * num_timesteps];

	// read data
	if (scalar_fields != NULL) {
		delete[] scalar_fields;
		scalar_fields = NULL;
		delete[] scalar_bounds;
		scalar_bounds = NULL;
	}
	scalar_fields = new float[data_size * num_scalar_fields * num_timesteps];
	scalar_bounds = new float[2 * num_scalar_fields * num_timesteps];

	int num_total_fields = num_scalar_fields + 3;
	float *tmp = new float[data_size * num_total_fields * num_timesteps];

	for (int k = 0; k < num_timesteps; k++)
	{
		char times_name[80];
		if (num_timesteps > 1)
		{
			sprintf(times_name, "%s.%.5d.dat", filename, k);
			fp = fopen(times_name, "rb");
		}
		else
			fp = fopen(ts_name, "rb");
		// read scalar data
		fread(&tmp[k*data_size*num_total_fields], sizeof(float), (data_size * num_total_fields), fp);

		// close file
		fclose(fp);

		// copy and scan for min and max values
		for (int i = 0; i < num_scalar_fields; i++){

			float min_val = 99999.9;
			float max_val = 0.0;

			float avg = 0.0;

			int offset = i * data_size * num_timesteps;

			for (int j = 0; j < data_size; j++){

				float val = tmp[j * num_total_fields + 3 + i + k*data_size*num_total_fields];

				scalar_fields[j + k*data_size + offset] = val;

				if (toggle_xy) {
					// overwrite
					if (i == 0){
						vector_array[3 * j + 0 + 3 * k*data_size] = tmp[j * num_total_fields + 1 + k*data_size*num_total_fields];//toggle x and y components in vector field
						vector_array[3 * j + 1 + 3 * k*data_size] = tmp[j * num_total_fields + 0 + k*data_size*num_total_fields];
						vector_array[3 * j + 2 + 3 * k*data_size] = tmp[j * num_total_fields + 2 + k*data_size*num_total_fields];
					}
				}
				else {
					// overwrite 
					if (i == 0){
						vector_array[3 * j + 0 + 3 * k*data_size] = tmp[j * num_total_fields + 0 + k*data_size*num_total_fields];
						vector_array[3 * j + 1 + 3 * k*data_size] = tmp[j * num_total_fields + 1 + k*data_size*num_total_fields];
						vector_array[3 * j + 2 + 3 * k*data_size] = tmp[j * num_total_fields + 2 + k*data_size*num_total_fields];
					}
				}

				min_val = std::min(val, min_val);
				max_val = std::max(val, max_val);

				avg += scalar_fields[offset + j + k*data_size] / data_size;
			}
			scalar_bounds[2 * i + k*num_scalar_fields * 2] = min_val;
			scalar_bounds[2 * i + 1 + k*num_scalar_fields * 2] = max_val;
		}

		// normalize
		for (int i = 0; i < num_scalar_fields; i++){

			int offset = i * data_size * num_timesteps;

			float lower_bound = scalar_bounds[2 * i + k*num_scalar_fields * 2];
			float upper_bound = scalar_bounds[2 * i + 1 + k*num_scalar_fields * 2];

			// scale between [0..1] where 1 is original zero
			// the boundary of the bigger abs border will be used to scale
			// meaning one boundary will likely not be hit i.e real scale
			// for -50..100 will be [0.25..1.0]
			if (lower_bound < 0.0 && upper_bound > 0.0){

				float scale = 0.5f / std::max(-lower_bound, upper_bound);

				for (int j = 0; j < data_size; j++){

					scalar_fields[offset + j + k*data_size] = 0.5f + scalar_fields[offset + j + k*data_size] * scale;
				}
				scalar_bounds[2 * i + k*num_scalar_fields * 2] = 0.5f + scalar_bounds[2 * i + k*num_scalar_fields * 2] * scale;
				scalar_bounds[2 * i + 1 + k*num_scalar_fields * 2] = 0.5f + scalar_bounds[2 * i + 1 + k*num_scalar_fields * 2] * scale;


				// scale between [0..1]
			}
			else {

				float sign = upper_bound <= 0.0 ? -1.0f : 1.0f;

				float scale = 1.0f / (upper_bound - lower_bound) * sign;

				for (int j = 0; j < data_size; j++){

					scalar_fields[offset + j + k*data_size] = (scalar_fields[offset + j + k*data_size] - lower_bound) * scale;
				}
				scalar_bounds[2 * i + k*num_scalar_fields * 2] = (scalar_bounds[2 * i + k*num_scalar_fields * 2] + lower_bound) * scale; //should be 0.0
				scalar_bounds[2 * i + 1 + k*num_scalar_fields * 2] = (scalar_bounds[2 * i + 1 + k*num_scalar_fields * 2] + lower_bound) * scale; //should be 1.0
			}
		}
	}
	delete[] tmp;
	DownloadScalarFieldAsTexture();

	scalar_data_loaded = true;
}


void DownloadScalarFieldAsTexture(void)
{
	fprintf(stderr, "downloading scalar field to 2D texture\n");

	glEnable(GL_TEXTURE_2D);

	// generate and bind 2D texture
	glGenTextures(1, &scalar_field_texture);
	glBindTexture(GL_TEXTURE_2D, scalar_field_texture);

	// set necessary texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	int datasize = vol_dim[0] * vol_dim[1];
	//download texture in correct format
	glTexImage2D(GL_TEXTURE_2D, 0, GL_INTENSITY16, vol_dim[0], vol_dim[1], 0, GL_LUMINANCE, GL_FLOAT, &scalar_fields[loaded_timestep*datasize*num_scalar_fields + current_scalar_field*datasize]);


	glDisable(GL_TEXTURE_2D);
}

char *textFileRead(char *fn){

	FILE *fp;
	char *content = NULL;

	int count = 0;

	if (fn != NULL) {
		fp = fopen(fn, "rt");

		if (fp != NULL) {

			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = (char *)malloc(sizeof(char)* (count + 1));
				count = fread(content, sizeof(char), count, fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}



void initGL(void){

	int gl_major, gl_minor;

	// Initialize the "OpenGL Extension Wrangler" library
	printf("glew init\n");
	GLenum err = glewInit();

	// query and display OpenGL version
	getGlVersion(&gl_major, &gl_minor);
	printf("GL_VERSION major=%d minor=%d\n", gl_major, gl_minor);

	// initialize all the OpenGL extensions 
	if (glewGetExtension("GL_EXT_framebuffer_object"))
		printf("GL_EXT_framebuffer_object support\n");

	if (GL_ARB_multitexture)
		printf("GL_ARB_multitexture support\n");

	if (glewGetExtension("GL_ARB_fragment_shader") != GL_TRUE ||
		glewGetExtension("GL_ARB_shader_objects") != GL_TRUE ||
		glewGetExtension("GL_ARB_shading_language_100") != GL_TRUE)
	{
		printf("Driver does not support OpenGL Shading Language");
		//exit(1);
	}


	// set viewport
	glViewport(0, 0, 512, 512);

	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	gluOrtho2D(0, 1, 0, 1);

	glDisable(GL_DEPTH_TEST);
}

void reset_rendering_props(void)
{
	num_scalar_fields = 0;
}

/******************************************************************************/
/*
* Main
*
******************************************************************************/

int main(int argc, char **argv)
{
	// init variables
	view_width = 0;
	view_height = 0;

	toggle_xy = 0;

	reset_rendering_props();

	vector_array = NULL;
	scalar_fields = NULL;
	scalar_bounds = NULL;
	grid_data_loaded = false;
	scalar_data_loaded = false;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(512, 512);
	window = glutCreateWindow("AMCS/CS247 Scientific Visualization");

	glutIdleFunc(play);
	glutDisplayFunc(display);
	glutKeyboardFunc(key);
	glutReshapeFunc(reshape);
	glutMotionFunc(motion);
	glutMouseFunc(mouse);
	glutSpecialFunc(special);

	glutTimerFunc(TIMER_FREQUENCY_MILLIS, timer, 0);

	// init OpenGL
	initGL();

	NextClearColor();

	filenames[0] = "../../Datasets/block/c_block";
	filenames[1] = "../../Datasets/tube/tube";
	filenames[2] = "../../Datasets/hurricane/hurricane_p_tc";

	LoadData(filenames[0]);
	loaded_file = 0;

	DownloadScalarFieldAsTexture();

	// display help
	key('?', 0, 0);

	glutMainLoop();

	return 0;
}