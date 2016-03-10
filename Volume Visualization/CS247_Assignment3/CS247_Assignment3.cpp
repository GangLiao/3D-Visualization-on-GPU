//
// CS247_prog3
// AMCS/CS247 Programming Assignment 3
//


#include "CS247_Assignment3.h"

#include "tf_tables.h"

#include <stdio.h>
#include <math.h>

#define BUFFER_WIDTH 1024
#define BUFFER_HEIGHT 1024

int printOglError( char *file, int line )
{
	//
	// Returns 1 if an OpenGL error occurred, 0 otherwise.
	//
	GLenum glErr;
	int retCode = 0;

	glErr = glGetError();
	while ( glErr != GL_NO_ERROR ) {
		printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString( glErr ) );
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

void getGlVersion( int *major, int *minor )
{
	const char* verstr = (const char*)glGetString( GL_VERSION );
	if ( ( verstr == NULL ) || ( sscanf( verstr, "%d.%d", major, minor ) != 2 ) ) {
		*major = *minor = 0;
		fprintf( stderr, "Invalid GL_VERSION format!!!\n" );
	}
}


/*
rotating background color
*/
static void NextClearColor( void )
{
    static int color = 0;

    switch( color++ )
    {
        case 0:
			clear_color[ 0 ] = 0.0f;
			clear_color[ 1 ] = 0.0f;
			clear_color[ 2 ] = 0.0f;
			clear_color[ 3 ] = 1.0f;
			break;
        case 1:
			clear_color[ 0 ] = 0.2f;
			clear_color[ 1 ] = 0.2f;
			clear_color[ 2 ] = 0.3f;
			clear_color[ 3 ] = 1.0f;
			break;
        default:
			clear_color[ 0 ] = 0.7f;
			clear_color[ 1 ] = 0.7f;
			clear_color[ 2 ] = 0.7f;
			clear_color[ 3 ] = 1.0f;
			color = 0;
			break;
    }
	glClearColor( clear_color[ 0 ], clear_color[ 1 ], clear_color[ 2 ], clear_color[ 3 ] );
}


/******************************************************************************/
/*
 * GLUT glue
 *
 ******************************************************************************/

static
void display( void )
{
	// reshape for offscreen render pass
	// set viewport
	glViewport( 0, 0, BUFFER_WIDTH, BUFFER_HEIGHT );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	// set perspective projection
	glFrustum( -1.1, 1.1, -1.1, 1.1, 3.0, 10.0 );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// object translation
	glTranslatef( 0.0, 0.0, -5.0 );

	// object rotation
    glRotatef( fYDiff, 1, 0, 0 );
    glRotatef( fXDiff, 0, 1, 0 );
    glRotatef( fZDiff, 0, 0, 1 );
	
	// =============================================================
	// TODO: perform raycasting steps here
	// i.e. render frontfaces and backfaces, then cast
	// =============================================================
	
	

	RenderFrontFaces(1.0, 1.0, 1.0);
	RenderBackFaces(1.0, 1.0, 1.0);
	
	RenderRaycastPass();

	//disableRenderToBuffer();
	//RenderBufferToScreen(frontface_buffer);

    glFlush();
    glutSwapBuffers();

}


static
void play( void )
{
    int thisTime = glutGet( GLUT_ELAPSED_TIME );

    glutPostRedisplay();
}


static
void key( unsigned char keyPressed, int x, int y ) // key handling
{
	switch ( keyPressed ) {
		case '1':
			LoadData( "../../Datasets/lobster.dat" );
			break;
		case '2':
			LoadData( "../../Datasets/skewed_head.dat" );
			break;
		case 'o':
			enable_lighting = 1 - enable_lighting;
			if( enable_lighting ) fprintf( stderr, "lighting is now enabled\n" );
			else fprintf( stderr, "lighting is now disabled\n" );
			break;
		case 'g':
			enable_gm_scaling = 1 - enable_gm_scaling;
			if( enable_gm_scaling ) fprintf( stderr, "gradient magnitude scaled shading is now enabled\n" );
			else fprintf( stderr, "gradient magnitude scaled shading is now disabled\n" );
			break;
		case 'f':
			use_dvr = 1 - use_dvr;
			if( use_dvr ) fprintf( stderr, "DVR Rendering\n" );
			else fprintf( stderr, "Iso Value Rendering\n" );
			break;
		case 'i':
			if ( iso_value < 1.0f ) {
				iso_value += 0.05f;
			}
			fprintf( stderr, "increasing iso value to: %f\n", iso_value );
			break;
		case 'k':
			if ( iso_value > 0.0f ) {
				iso_value -= 0.05f;
			}
			fprintf( stderr, "decreasing iso value to: %f\n", iso_value );
			break;
		case 'a':
			if ( ambient < 1.0f ) {
				ambient += 0.05f;
			}
			fprintf( stderr, "increasing ambient brightness value to: %f\n", ambient );
			break;
		case 'z':
			if ( ambient > 0.0f ) {
				ambient -= 0.05f;
			}
			fprintf( stderr, "decreasing ambient brightness value to: %f\n", ambient );
			break;
		case 'b':
			NextClearColor();
			break;
		 case ' ':
            Rotate = !Rotate;

            if ( !Rotate ) {
                fXInertiaOld = fXInertia;
                fYInertiaOld = fYInertia;
            } else {
                fXInertia = fXInertiaOld;
                fYInertia = fYInertiaOld;

                // to prevent confusion, force some rotation
				if ( ( fXInertia == 0.0 ) && ( fYInertia == 0.0 ) ) {
                    fXInertia = -0.5;
				}
            }
            break;
		case 'r': //return to inital state/view
			fXDiff    = 0;
			fYDiff    = 35;
			fZDiff    = 0;
			xLastIncr = 0;
			yLastIncr = 0;
			fXInertia = -0.5;
			fYInertia = 0;
			break;
        case '+':
            step_size /= 1.5f;
			fprintf( stderr, "stepsize is now: %f\n", step_size );
            break;
        case '-':
            step_size *= 1.5f;
			fprintf( stderr, "stepsize is now: %f\n", step_size );
            break;
		case 'w':
			tf_win_min = std::min( tf_win_min + 1, tf_win_max );
			UpdateTransferfunction();
			fprintf( stderr, "lower window boundary increased to: %d\n", tf_win_min );
            break;
		case 's':
			tf_win_min = std::max( tf_win_min - 1, 0 );
			UpdateTransferfunction();
			fprintf( stderr, "lower window boundary decreased to: %d\n", tf_win_min );
            break;
		case 'e':
			tf_win_max = std::min( tf_win_max + 1, 127 );
			UpdateTransferfunction();
			fprintf( stderr, "lower window boundary increased to: %d\n", tf_win_max );
            break;
		case 'd':
			tf_win_max = std::max( tf_win_max - 1, tf_win_min );
			UpdateTransferfunction();
			fprintf( stderr, "lower window boundary decreased to: %d\n", tf_win_max );
            break;
        case '5':
			DownloadTransferFunctionTexture( 0 );
            break;
        case '6':
			DownloadTransferFunctionTexture( 1 );
            break;
        case '7':
			DownloadTransferFunctionTexture( 2 );
            break;
        case '8':
			DownloadTransferFunctionTexture( 3 );
            break;
        case '9':
			DownloadTransferFunctionTexture( 4 );
            break;
        case '0':
			DownloadTransferFunctionTexture( 5 );
            break;
		case 'q':
        case 27:
            exit( 0 );
            break;
		default:
			fprintf( stderr, "\nKeyboard commands:\n\n"
			"1 - Load lobster dataset\n"
			"2 - Load head dataset\n" 
			"p - Load test dataset\n"
			"b - Toggle among background clear colors\n"
			"o - en-/disable lighting\n"
			"g - use gradient magnitude for shading (highlight edges)\n"
			"f - switch between DVR and Iso value rendering\n"
			"i, k - in-/decrease iso value\n"
			"a, z - in-/decrease ambient brightness\n"
			"w, s - in-/decrease lower tf window value\n"
			"e, d - in-/decrease upper tf window value\n"
			"5,6,7,8,9,0 - select transferfunctions\n"
			"<home>	 - reset zoom and rotation\n"
			"r - reset rotation\n"
            "<space> or <click> - stop rotation\n"
            "<+>, <-> - in-/decrease sampling rate\n"
			"<ctrl + drag> - lock rotation around z-axis\n"
            "<arrow keys> or <drag> - rotate model\n"
			"q, <esc> - Quit\n" );
			break;
	}
}



static
void timer( int value )
{

    /* Increment wrt inertia */
    if ( Rotate )
    {
        fXDiff = fXDiff + fXInertia;
        fYDiff = fYDiff + fYInertia;
    }

    /* Callback */
    glutTimerFunc( TIMER_FREQUENCY_MILLIS , timer, 0 ); 
}


static
void mouse( int button, int state, int x, int y )
{
	bmModifiers = glutGetModifiers();

	if ( button == GLUT_LEFT_BUTTON ) { //rotate 
      
		if ( state == GLUT_UP ) {
			xLast = -1;
			yLast = -1;

			if ( xLastIncr > INERTIA_THRESHOLD ) {
				fXInertia = ( xLastIncr - INERTIA_THRESHOLD ) * INERTIA_FACTOR;
			}

			if ( -xLastIncr > INERTIA_THRESHOLD ) {
				fXInertia = ( xLastIncr + INERTIA_THRESHOLD ) * INERTIA_FACTOR;
			}

			if ( yLastIncr > INERTIA_THRESHOLD ) { 
				fYInertia = ( yLastIncr - INERTIA_THRESHOLD ) * INERTIA_FACTOR;
			}

			if ( -yLastIncr > INERTIA_THRESHOLD ) { 
				fYInertia = ( yLastIncr + INERTIA_THRESHOLD ) * INERTIA_FACTOR;
			}

		} else {
			fXInertia = 0;
			fYInertia = 0;
		}

		xLastIncr = 0;
		yLastIncr = 0;
	}
}


static
void motion( int x, int y )
{
	if ( ( xLast != -1 ) || ( yLast != -1 ) ) { //zoom
		xLastIncr = x - xLast;
		yLastIncr = y - yLast;
		if ( bmModifiers & GLUT_ACTIVE_CTRL ) {
			if ( xLast != -1 ) {
				fZDiff += xLastIncr;
				fScale += yLastIncr * SCALE_FACTOR;
			}
		} else {
			if ( xLast != -1 ) { //rotate
				fXDiff += xLastIncr;
				fYDiff += yLastIncr;
			}
		}
	}

   xLast = x;
   yLast = y;
}


static
void reshape( int wid, int ht )
{	
	view_width = wid;
	view_height = ht;

	float vp = 0.8f;
    float aspect = (float)wid / (float)ht;

	// set viewport
	glViewport( 0, 0, wid, ht );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	// set perspective projection
	glFrustum( -vp, vp, ( -vp / aspect ), ( vp / aspect ), 3, 10.0 );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef( 0.0, 0.0, -5.0 );
}


static
void special( int key, int x, int y )
{
	//handle special keys
	switch ( key ) {
		case GLUT_KEY_HOME: //return to inital state/view
			fXDiff    = 0;
			fYDiff    = 35;
			fZDiff    = 0;
			xLastIncr = 0;
			yLastIncr = 0;
			fXInertia = -0.5;
			fYInertia = 0;
			fScale    = 1.0;
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


/*
 * load .dat dataset
 */
void LoadData( char* filename )
{
	fprintf( stderr, "loading data %s\n", filename );

	// open file, read only, binary mode 
	FILE* fp = fopen( filename, "rb" );
	if ( fp == NULL ) {
		fprintf( stderr, "Cannot open file %s for reading.\n", filename );
		return;
	}
	
	memset( vol_dim, 0, sizeof( unsigned short ) * 3 );
	
	//read volume dimensions from file
	fread( &vol_dim[ 0 ], sizeof( unsigned short ), 1, fp );
	fread( &vol_dim[ 1 ], sizeof( unsigned short ), 1, fp );
	fread( &vol_dim[ 2 ], sizeof( unsigned short ), 1, fp );
	
	fprintf(stderr, "volume dimensions: x: %i, y: %i, z:%i \n", vol_dim[ 0 ], vol_dim[ 1 ], vol_dim[ 2 ] );
	
	if ( data_array != NULL ) {
		delete[] data_array;
	}

	// 1D array for storing volume data
	data_array = new unsigned short[ vol_dim[ 0 ] * vol_dim[ 1 ] * vol_dim[ 2 ] ]; 
	
	// read volume data
	fread( data_array, sizeof( unsigned short), ( vol_dim[ 0 ] * vol_dim[ 1 ] * vol_dim[ 2 ] ), fp );
	
	// shift volume data by 4 bit (converting 12 bit data to 16 bit data)
	for( int z = 0; z < vol_dim[ 2 ]; z++ ) {
		for( int y = 0; y < vol_dim[ 1 ]; y++ ) { 	
			for( int x = 0; x < vol_dim[ 0 ]; x++ ) { 
				int current_idx = x + ( y * vol_dim[ 0 ] ) + ( z * vol_dim[ 0 ] * vol_dim[ 1 ] );

				if ( strcmp( filename, "../hydrogen.dat" ) ) {
					data_array[ current_idx ] = data_array[ current_idx ] << 4; 
				} else {
					data_array[ current_idx ] = data_array[ current_idx ] << 8;
				}

			} 	
		} 	
	} 

	// close file
	fclose( fp );

	// download data into texture
	DownloadVolumeAsTexture();

	data_loaded = true;
}


/*
 * download data to 3D texture
 */
void DownloadVolumeAsTexture()
{
	fprintf( stderr, "downloading volume to 3D texture\n" );

	glEnable( GL_TEXTURE_3D );

	// generate and bind 3D texture
	glGenTextures( 1, &vol_texture );
	glBindTexture( GL_TEXTURE_3D, vol_texture );
	
	// set necessary texture parameters
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP );

	//download texture in correct format (our data only has one channel of unsigned short data, therefore we use GL_INTENSITY16)
	glTexImage3D( GL_TEXTURE_3D, 0,  GL_INTENSITY16, vol_dim[ 0 ], vol_dim[ 1 ], vol_dim[ 2 ], 0, GL_LUMINANCE, GL_UNSIGNED_SHORT, data_array ); 

	glDisable( GL_TEXTURE_3D );
}


/*
 * update tf based on defined window
 */
void UpdateTransferfunction( void ) 
{

	// =============================================================
	// TODO: update custom transferfunction tf0 here
	// ===========================================

	for (int i = 0; i < tf_win_min; i++)
	{
		tf0[i * 4 + 0] = 0.0f;
		tf0[i * 4 + 1] = 0.0f;
		tf0[i * 4 + 2] = 0.0f;
		tf0[i * 4 + 3] = 0.0f;
	}

	float len = tf_win_max - tf_win_min;
	for (int i = 0; i < len; i++)
	{
		tf0[(i+tf_win_min) * 4 + 0] = (float)(i) / len;
		tf0[(i+tf_win_min) * 4 + 1] = (float)(i) / len;
		tf0[(i+tf_win_min) * 4 + 2] = (float)(i) / len;
		tf0[(i+tf_win_min) * 4 + 3] = (float)(i) / len;
	}

	for (int i = tf_win_max; i < 128; i++)
	{
		tf0[i * 4 + 0] = 1.0f;
		tf0[i * 4 + 1] = 1.0f;
		tf0[i * 4 + 2] = 1.0f;
		tf0[i * 4 + 3] = 1.0f;
	}

	
	DownloadTransferFunctionTexture( 0 );
}


int clamp(int a, int min, int max)
{
	int x = a > min ? a : min;

	return x > max ? max : x;
}

/*
void preintegrationTable(GLubyte * Table)
{
	double r = 0., g = 0., b = 0., a = 0.; int rcol, gcol, bcol, acol;
	double rInt[256], gInt[256], bInt[256], aInt[256];
	GLubyte lookupImg[256 * 256 * 4]; int smin, smax, preintName;
	double factor; int lookupindex = 0;
	rInt[0] = 0.; gInt[0] = 0.; bInt[0] = 0.; aInt[0] = 0.;
	// compute integral functions
	for (int i = 1; i<256; i++) {
		double tauc = (Table[(i - 1) * 4 + 3] + Table[i * 4 + 3]) / 2.;
		r = r + (Table[(i - 1) * 4 + 0] + Table[i * 4 + 0]) / 2.*tauc / 255.;
		g = g + (Table[(i - 1) * 4 + 1] + Table[i * 4 + 1]) / 2.*tauc / 255.;
		b = b + (Table[(i - 1) * 4 + 2] + Table[i * 4 + 2]) / 2.*tauc / 255.;
		a = a + tauc;
		rInt[i] = r; gInt[i] = g; bInt[i] = b; aInt[i] = a;
	}
	// compute look-up table from integral functions
	for (int sb = 0; sb<256; sb++)
	for (int sf = 0; sf<256; sf++) {
		if (sb < sf) { smin = sb; smax = sf; }
		else { smin = sf; smax = sb; }
		if (smax != smin) {
			factor = 1. / (double)(smax - smin);
			rcol = (rInt[smax] - rInt[smin]) * factor;
			gcol = (gInt[smax] - gInt[smin]) * factor;
			bcol = (bInt[smax] - bInt[smin]) * factor;
			acol = 256.*
				(1. - exp(-(aInt[smax] - aInt[smin])*factor / 255.));
		}
		else {
			factor = 1. / 255.;
			rcol = Table[smin * 4 + 0] * Table[smin * 4 + 3] * factor;
			gcol = Table[smin * 4 + 1] * Table[smin * 4 + 3] * factor;
			bcol = Table[smin * 4 + 2] * Table[smin * 4 + 3] * factor;
			acol = (1. - exp(-Table[smin * 4 + 3] * 1. / 255.))*256.;
		}
		

		lookupImg[lookupindex++] = clamp(rcol, 0, 255);
		lookupImg[lookupindex++] = clamp(gcol, 0, 255);
		lookupImg[lookupindex++] = clamp(bcol, 0, 255);
		lookupImg[lookupindex++] = clamp(acol, 0, 255);
	}
	// create texture
	glGenTextures(1, &PreIntTable_buffer);
	glBindTexture(GL_TEXTURE_2D, PreIntTable_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, &lookupImg);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
*/
/*
 * download tf data to 1D texture
 */
void DownloadTransferFunctionTexture( int tf_id )
{
	fprintf( stderr, "downloading transfer function to 1D texture\n" );

	glEnable( GL_TEXTURE_1D );

	// generate and bind 1D texture
	glGenTextures( 1, &tf_texture );
	glBindTexture( GL_TEXTURE_1D, tf_texture );
	
	// set necessary texture parameters
	glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP );

	//download texture in correct format
	switch( tf_id ){
		case 0:
			glTexImage1D( GL_TEXTURE_1D, 0,  GL_RGBA, 128, 0, GL_RGBA, GL_FLOAT, tf0 ); 
			break;
		case 1:
			glTexImage1D( GL_TEXTURE_1D, 0,  GL_RGBA, 2, 0, GL_RGBA, GL_FLOAT, tf1 ); 
			break;
		case 2:
			glTexImage1D( GL_TEXTURE_1D, 0,  GL_RGBA, 4, 0, GL_RGBA, GL_FLOAT, tf2 ); 
			break;
		case 3:
			glTexImage1D( GL_TEXTURE_1D, 0,  GL_RGBA, 12, 0, GL_RGBA, GL_FLOAT, tf3 ); 
			break;
		case 4:
			glTexImage1D( GL_TEXTURE_1D, 0,  GL_RGBA, 12, 0, GL_RGBA, GL_FLOAT, tf4 ); 
			break;
		case 5:
			glTexImage1D( GL_TEXTURE_1D, 0,  GL_RGBA, 12, 0, GL_RGBA, GL_FLOAT, tf5 ); 
			break;
		default:
			glTexImage1D( GL_TEXTURE_1D, 0,  GL_RGBA, 128, 0, GL_RGBA, GL_FLOAT, tf0 ); 
			break;
	}

	glDisable( GL_TEXTURE_1D );
}




void vertex(float* V)
{
	glColor3f(V[0], V[1], V[2]);
	glMultiTexCoord3fARB(GL_TEXTURE1_ARB, V[0], V[1], V[2]);
	glVertex3f(V[0], V[1], V[2]);
}

// this method is used to draw the front and backside of the volume
void drawQuads(float x, float y, float z)
{
	glColor3f(1.0, 1.0, 1.0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	int size = x;
	float A[3] = { size, size, size };
	float B[3] = { size, size, -size };
	float C[3] = { size, -size, -size };
	float D[3] = { size, -size, size };
	float E[3] = { -size, size, size };
	float F[3] = { -size, size, -size };
	float G[3] = { -size, -size, -size };
	float H[3] = { -size, -size, size };

	glBegin(GL_QUADS);

	vertex(D);
	vertex(C);
	vertex(B);
	vertex(A);

	vertex(G);
	vertex(H);
	vertex(E);
	vertex(F);

	vertex(C);
	vertex(G);
	vertex(F);
	vertex(B);

	vertex(H);
	vertex(D);
	vertex(A);
	vertex(E);

	vertex(E);
	vertex(A);
	vertex(B);
	vertex(F);

	vertex(G);
	vertex(C);
	vertex(D);
	vertex(H);

	glEnd();

}

/*
* render backfaces to start raycasting
*/
void RenderBackFaces(float dim_x, float dim_y, float dim_z)
{
	enableRenderToBuffer(backface_buffer);

	// =============================================================
	// TODO: render your backfaces here
	// result gets piped to backface_buffer automatically
	// =============================================================
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	drawQuads(dim_x, dim_y, dim_z);
	glDisable(GL_CULL_FACE);
	disableRenderToBuffer();
}

/*
 * render frontfaces to start raycasting
 */
void RenderFrontFaces(float dim_x, float dim_y, float dim_z)
{
	enableRenderToBuffer( frontface_buffer );

	// =============================================================
	// TODO: render your backfaces here
	// result gets piped to frontface_buffer automatically
	// =============================================================
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	drawQuads(dim_x, dim_y, dim_z);
	glDisable(GL_CULL_FACE);
	disableRenderToBuffer();
}


void RenderRaycastPass( void )
{
	// switch for dvr and iso shader programs
	GLuint shader_program = use_dvr ? raycast_dvr_shader_program : raycast_iso_shader_program;

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glLoadIdentity();

	// enable shader
	glUseProgram( shader_program );

	// example for a texture uniform
	int tf_location = glGetUniformLocation( shader_program, "transferfunction" );
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_1D, tf_texture );
	glUniform1i( tf_location, 0 );

	// example for a scalar/vector uniform
	int params_location = glGetUniformLocation( shader_program, "params" );
	glUniform4f( params_location, 0.0, 0.5, 1.0, 1.0 );

	// =============================================================
	// TODO: add additional uniforms as needed
	// i.e. front and backface buffers ( frontface_buffer, backface_buffer )
	// =============================================================
	int size_step_loc = glGetUniformLocation(shader_program, "step_size");
	glUniform1f(size_step_loc, step_size);

	int abi_loc = glGetUniformLocation(shader_program, "ambient");
	glUniform1f(abi_loc, ambient);

	int isoval_loc = glGetUniformLocation(shader_program, "iso_value");
	glUniform1f(isoval_loc, iso_value);
	
	int vol_location = glGetUniformLocation(shader_program, "vol_texture");
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, vol_texture);
	glUniform1i(vol_location, 1);

	
	int front_location = glGetUniformLocation(shader_program, "front_texture");
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, frontface_buffer);
	glUniform1i(front_location, 2);
	
	int back_location = glGetUniformLocation(shader_program, "back_texture");
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, backface_buffer);
	glUniform1i(back_location, 3);

	// Render screen filling quad
	// use squared centered quad to not disturb ratio
	int x[2] = { 0, 0 };
	int y[2] = { 0, 0 };
	int dim, off;
	if( view_width < view_height ){

		dim = view_width;
		off = 0.5 * ( view_height - view_width );
	
		x[ 0 ] = 0;
		x[ 1 ] = dim;
		y[ 0 ] = off;
		y[ 1 ] = dim + off;

	} else {

		dim = view_height;
		off = 0.5 * ( view_width - view_height );
	
		x[ 0 ] = off;
		x[ 1 ] = dim + off;
		y[ 0 ] = 0;
		y[ 1 ] = dim;
	}

	glViewport( 0, 0, view_width, view_height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D( 0, view_width, 0, view_height );
	glMatrixMode( GL_MODELVIEW );
	
	//draw_fullscreen_quad
	glDisable(GL_DEPTH_TEST);
	glBegin(GL_QUADS);

	// red screen means your shader is broken
	glColor3f( 1.0, 0.0, 0.0 );

	glTexCoord2f(0, 0); glVertex2f(x[ 0 ], y[ 0 ] );
	glTexCoord2f(1, 0); glVertex2f(x[ 1 ], y[ 0 ] );
	glTexCoord2f(1, 1); glVertex2f(x[ 1 ], y[ 1 ]);
	glTexCoord2f(0, 1); glVertex2f(x[ 0 ], y[ 1 ]);

	glEnd();
	glEnable(GL_DEPTH_TEST);

	//disable shader
	glUseProgram( 0 );
}


// for debugging purposes
void RenderBufferToScreen( GLuint buffer ){

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glLoadIdentity();
	
	glEnable( GL_TEXTURE_2D );
	glBindTexture (GL_TEXTURE_2D, buffer );

	// use squared centered quad to not disturb ratio
	int x[2] = { 0, 0 };
	int y[2] = { 0, 0 };
	int dim, off;
	if( view_width < view_height ){

		dim = view_width;
		off = 0.5 * ( view_height - view_width );
	
		x[ 0 ] = 0;
		x[ 1 ] = dim;
		y[ 0 ] = off;
		y[ 1 ] = dim + off;

	} else {

		dim = view_height;
		off = 0.5 * ( view_width - view_height );
	
		x[ 0 ] = off;
		x[ 1 ] = dim + off;
		y[ 0 ] = 0;
		y[ 1 ] = dim;
	}
	
	glViewport( 0, 0, view_width, view_height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D( 0, view_width, 0, view_height );
	glMatrixMode( GL_MODELVIEW );
	
	//draw_fullscreen_quad
	glDisable(GL_DEPTH_TEST);
	glBegin(GL_QUADS);

	glTexCoord2f(0, 0); /*glColor3f( 0.0f, 0.0f, 1.0f );*/ glVertex2f( x[ 0 ], y[ 0 ] );
	glTexCoord2f(1, 0); /*glColor3f( 1.0f, 0.0f, 1.0f );*/ glVertex2f( x[ 1 ], y[ 0 ] );
	glTexCoord2f(1, 1); /*glColor3f( 1.0f, 1.0f, 1.0f );*/ glVertex2f( x[ 1 ], y[ 1 ] );
	glTexCoord2f(0, 1); /*glColor3f( 0.0f, 1.0f, 1.0f );*/ glVertex2f( x[ 0 ], y[ 1 ] );

	glEnd();
	glEnable(GL_DEPTH_TEST);
	
	glDisable(GL_TEXTURE_2D);
}

/*
 * enable buffer to render to
 */
void enableRenderToBuffer( GLuint buffer )
{
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, framebuffer );

	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, buffer, 0 );
}

/*
 * unbinds buffer
 */
void disableRenderToBuffer()
{
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}


void initGL( void ){

	int gl_major, gl_minor;

	// Initialize the "OpenGL Extension Wrangler" library
	printf("glew init\n");
	GLenum err = glewInit();

	// query and display OpenGL version
	getGlVersion( &gl_major, &gl_minor );
	printf( "GL_VERSION major=%d minor=%d\n", gl_major, gl_minor );

	// initialize all the OpenGL extensions 
	if( glewGetExtension("GL_EXT_framebuffer_object") )
		printf("GL_EXT_framebuffer_object support\n");
	
	if( GL_ARB_multitexture )
		printf("GL_ARB_multitexture support\n");
	
	if( glewGetExtension("GL_ARB_fragment_shader")      != GL_TRUE ||
		glewGetExtension("GL_ARB_shader_objects")       != GL_TRUE ||
		glewGetExtension("GL_ARB_shading_language_100") != GL_TRUE ) {

		printf( "Driver does not support OpenGL Shading Language" );
		exit( 1 );
	}

	// Create the to FBO's one for the backsides and one for the frontsides
	glGenFramebuffersEXT( 1, &framebuffer );
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT ,framebuffer );

	glGenTextures( 1, &backface_buffer);
	glBindTexture( GL_TEXTURE_2D, backface_buffer );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
	glTexImage2D( GL_TEXTURE_2D, 0,GL_RGBA16F_ARB, BUFFER_WIDTH, BUFFER_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL );
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, backface_buffer, 0 );


	glGenTextures( 1, &frontface_buffer );
	glBindTexture( GL_TEXTURE_2D, frontface_buffer );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
	glTexImage2D( GL_TEXTURE_2D, 0,GL_RGBA16F_ARB, BUFFER_WIDTH, BUFFER_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL );
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, frontface_buffer, 0 );

	/*
	//Add pre-integration Table
	for (int i = 0; i < 256; i++){

		tf00[4 * i + 0] = (float)(i) / 256.0f;
		tf00[4 * i + 1] = (float)(i) / 256.0f;
		tf00[4 * i + 2] = (float)(i) / 256.0f;
		tf00[4 * i + 3] = (float)(i) / 256.0f;

	}
	preintegrationTable(tf00);
	*/
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

	// Create Shader Program
	LoadAndLinkShaders();

	// download initial transfer function
	DownloadTransferFunctionTexture( 0 );
}

char *textFileRead( char *fn ) {

	FILE *fp;
	char *content = NULL;

	int count=0;

	if ( fn != NULL ) {
		fp = fopen( fn,"rt" );

		if ( fp != NULL ) {
			fseek( fp, 0, SEEK_END );
			count = ftell( fp );
			rewind( fp );

			if ( count > 0 ) {
				content = (char *)malloc( sizeof( char ) * ( count + 1 ) );
				count = fread( content, sizeof(char), count, fp );
				content[ count ] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}


void LoadAndLinkShaders( void ){

	char *fs = NULL;

	raycast_dvr_fragment_shader = glCreateShader( GL_FRAGMENT_SHADER );
	raycast_iso_fragment_shader = glCreateShader( GL_FRAGMENT_SHADER );

	fs = textFileRead( "raycast_dvr.glsl" );

	if ( fs == NULL  ){
		
		printf( "DVR shader file not found\n" );
		exit( 1 );
	}

	const char *f1 = fs;

	glShaderSource( raycast_dvr_fragment_shader, 1, &f1, NULL );

	free( fs );

	fs = textFileRead( "raycast_iso.glsl" );

	if ( fs == NULL ) {
		
		printf( "ISO shader file not found\n" );
		exit( 1 );
	}

	const char *f2 = fs;

	glShaderSource( raycast_iso_fragment_shader, 1, &f2, NULL );

	free( fs );

	glCompileShader( raycast_dvr_fragment_shader );
	glCompileShader( raycast_iso_fragment_shader );

	raycast_dvr_shader_program = glCreateProgram();
	raycast_iso_shader_program = glCreateProgram();
	glAttachShader( raycast_dvr_shader_program, raycast_dvr_fragment_shader );
	glAttachShader( raycast_iso_shader_program, raycast_iso_fragment_shader );

	glLinkProgram( raycast_dvr_shader_program );
	glLinkProgram( raycast_iso_shader_program );
}


/******************************************************************************/
/*
 * Main
 *
 ******************************************************************************/

int main( int argc, char **argv )
{
	// init variables
	fXDiff = 206;		//rotation
	fYDiff = 16;
	fZDiff = 10;
	xLastIncr = 0;		//last difference in mouse movement
	yLastIncr = 0;
	fXInertia = -0.5;	//inertia for keeping rotating even after mouse movement
	fYInertia = 0;
	fXInertiaOld;		//old inertia
	fYInertiaOld;
	fScale = 1.0;		//scale/zoom factor
	xLast = -1;			//last mouse position
	yLast = -1;
	bmModifiers;		//keyboard modifiers (e.g. ctrl,...)
	Rotate = 1;			//auto-rotate
	view_width = 0;
	view_height = 0;

	use_dvr = 1;

	iso_value = 0.5f;

	enable_lighting = 0;
	enable_gm_scaling = 0;
	ambient = 0.5;
	step_size = 0.0025f;

	tf_win_min = 0;
	tf_win_max = 127;

	for( int i = 0; i < 128; i++ ){
	
		tf0[ 4 * i + 0 ] = (float)(i) / 128.0f;
		tf0[ 4 * i + 1 ] = (float)(i) / 128.0f;
		tf0[ 4 * i + 2 ] = (float)(i) / 128.0f;
		tf0[ 4 * i + 3 ] = (float)(i) / 128.0f;
	}

	data_array = NULL;
	data_loaded = false;
	
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE );
	glutInitWindowSize( 512, 512 );
	window = glutCreateWindow( "AMCS/CS247 Scientific Visualization" );

	glutIdleFunc( play );
	glutDisplayFunc( display );
	glutKeyboardFunc( key );
	glutReshapeFunc( reshape );
	glutMotionFunc( motion );
	glutMouseFunc( mouse );
	glutSpecialFunc( special );

    glutTimerFunc( TIMER_FREQUENCY_MILLIS, timer, 0 );

	// init OpenGL
	initGL();
	
	glDepthFunc( GL_LESS );
    glEnable( GL_DEPTH_TEST );

	NextClearColor();

	// display help
	key( '?', 0, 0 );

	glutMainLoop();

	return 0;
}

