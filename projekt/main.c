/*
* The code is released under the GNU General Public License.
* Developed by Mark Williams
* A guide to this code can be found here; http://ozzmaker.com/2013/04/22/845/
* Created 28th April 2013
*/


#include <unistd.h>
#include <math.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include "L3G.h"
#include "LSM303.h"
#include "sensor.c"
#include "i2c-dev.h"

#include <stdarg.h>
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


#define X   0
#define Y   1
#define Z   2

#define DT 0.02         // [s/loop] loop period. 20ms
#define AA 0.98         // complementary filter constant

#define A_GAIN 0.0573      // [deg/LSB]
#define G_GAIN 0.070     // [deg/s/LSB]
#define RAD_TO_DEG 57.29578
#define M_PI 3.14159265358979323846

//------------------ OPENGL ------------------ \\
// ----------------------------------------------------------
// Function Prototypes
// ----------------------------------------------------------
void display();
void specialKeys();
 
// ----------------------------------------------------------
// Global Variables
// ----------------------------------------------------------
double rotate_y=0; 
double rotate_x=0;
 
// ----------------------------------------------------------
// display() Callback function
// ----------------------------------------------------------
void display(){
 
  //  Clear screen and Z-buffer
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
 
  // Reset transformations
  glLoadIdentity();
 
  // Other Transformations
  // glTranslatef( 0.1, 0.0, 0.0 );      // Not included
  // glRotatef( 180, 0.0, 1.0, 0.0 );    // Not included
 
  // Rotate when user changes rotate_x and rotate_y
  glRotatef( rotate_x, 1.0, 0.0, 0.0 );
  glRotatef( rotate_y, 0.0, 1.0, 0.0 );
 
  // Other Transformations
  // glScalef( 2.0, 2.0, 0.0 );          // Not included
 
  //Multi-colored side - FRONT
  glBegin(GL_POLYGON);
 
  glColor3f( 1.0, 0.0, 0.0 );     glVertex3f(  0.5, -0.5, -0.5 );      // P1 is red
  glColor3f( 0.0, 1.0, 0.0 );     glVertex3f(  0.5,  0.5, -0.5 );      // P2 is green
  glColor3f( 0.0, 0.0, 1.0 );     glVertex3f( -0.5,  0.5, -0.5 );      // P3 is blue
  glColor3f( 1.0, 0.0, 1.0 );     glVertex3f( -0.5, -0.5, -0.5 );      // P4 is purple
 
  glEnd();
 
  // White side - BACK
  glBegin(GL_POLYGON);
  glColor3f(   1.0,  1.0, 1.0 );
  glVertex3f(  0.5, -0.5, 0.5 );
  glVertex3f(  0.5,  0.5, 0.5 );
  glVertex3f( -0.5,  0.5, 0.5 );
  glVertex3f( -0.5, -0.5, 0.5 );
  glEnd();
 
  // Purple side - RIGHT
  glBegin(GL_POLYGON);
  glColor3f(  1.0,  0.0,  1.0 );
  glVertex3f( 0.5, -0.5, -0.5 );
  glVertex3f( 0.5,  0.5, -0.5 );
  glVertex3f( 0.5,  0.5,  0.5 );
  glVertex3f( 0.5, -0.5,  0.5 );
  glEnd();
 
  // Green side - LEFT
  glBegin(GL_POLYGON);
  glColor3f(   0.0,  1.0,  0.0 );
  glVertex3f( -0.5, -0.5,  0.5 );
  glVertex3f( -0.5,  0.5,  0.5 );
  glVertex3f( -0.5,  0.5, -0.5 );
  glVertex3f( -0.5, -0.5, -0.5 );
  glEnd();
 
  // Blue side - TOP
  glBegin(GL_POLYGON);
  glColor3f(   0.0,  0.0,  1.0 );
  glVertex3f(  0.5,  0.5,  0.5 );
  glVertex3f(  0.5,  0.5, -0.5 );
  glVertex3f( -0.5,  0.5, -0.5 );
  glVertex3f( -0.5,  0.5,  0.5 );
  glEnd();
 
  // Red side - BOTTOM
  glBegin(GL_POLYGON);
  glColor3f(   1.0,  0.0,  0.0 );
  glVertex3f(  0.5, -0.5, -0.5 );
  glVertex3f(  0.5, -0.5,  0.5 );
  glVertex3f( -0.5, -0.5,  0.5 );
  glVertex3f( -0.5, -0.5, -0.5 );
  glEnd();
 
  glFlush();
  glutSwapBuffers();
 
}
 
// ----------------------------------------------------------
// specialKeys() Callback Function
// ----------------------------------------------------------
void specialKeys( int key, int x, int y ) {
 
  //  Right arrow - increase rotation by 5 degree
  if (key == GLUT_KEY_RIGHT)
    rotate_y += 5;
 
  //  Left arrow - decrease rotation by 5 degree
  else if (key == GLUT_KEY_LEFT)
    rotate_y -= 5;
 
  else if (key == GLUT_KEY_UP)
    rotate_x += 5;
 
  else if (key == GLUT_KEY_DOWN)
    rotate_x -= 5;
 
  //  Request display update
  glutPostRedisplay();
 
}
 
// ----------------------------------------------------------
// main() function
// ----------------------------------------------------------
// NOPE
//------------------ OPENGL ------------------ \\


void  INThandler(int sig)
{
        signal(sig, SIG_IGN);
        exit(0);
}

int mymillis()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec) * 1000 + (tv.tv_usec)/1000;
}

int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1)
{
    long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
    result->tv_sec = diff / 1000000;
    result->tv_usec = diff % 1000000;
    return (diff<0);
}


float rate_gyr_y;   // [deg/s]
	float rate_gyr_x;    // [deg/s]
	float rate_gyr_z;     // [deg/s]


	int  *Pgyr_raw;
	int  gyr_raw[3];



	float gyroXangle;
	float gyroYangle;
	float gyroZangle;

	int startInt;
	struct  timeval tvBegin, tvEnd,tvDiff;

	signed int gyr_x;
	signed int gyr_y;
	signed int gyr_z;

	float xData[5], yData[5]
	int counter;
void readData(){
	startInt = mymillis();

	readGYR(Pgyr_raw);

	//Convert Gyro raw to degrees per second
	rate_gyr_x = (float) *gyr_raw * G_GAIN;
	rate_gyr_y = (float) *(gyr_raw+1) * G_GAIN;
	//rate_gyr_z = (float) *(gyr_raw+2) * G_GAIN;


/*
	//Calculate the angles from the gyro
	if(rate_gyr_x > 2 || rate_gyr_x < -2){
		gyroXangle+=rate_gyr_x*DT;
		rotate_x += rate_gyr_x*DT;
	}
	if (rate_gyr_y > 2 || rate_gyr_y < -2){
		gyroYangle+=rate_gyr_y*DT;
		rotate_y += rate_gyr_y*DT;
	}
	*/
	xData[counter] = rate_gyr_x*DT;
	yData[counter] = rate_gyr_y*DT;
	
	
	/*if (rate_gyr_z > 2 || rate_gyr_z < -2)
		gyroZangle+=rate_gyr_z*DT;*/

	if(counter == 4){
		counter = 0;
		float buffX = xData[0]+xData[1]+xData[2]+xData[3]+xData[4])/5.0;
		float buffY = yData[0]+yData[1]+yData[2]+yData[3]+yData[4])/5.0;
		printf("   GyroX  %7.3f \t GyroY  %7.3f \n", buffX, buffY);
	
		rotate_x += buffX;
		rotate_y += buffY;
		glutPostRedisplay();
	}
	//printf("   GyroX  %7.3f \t GyroY  %7.3f \t GyroZ  %7.3f \t X %7.3f \t Y %7.3f \t Z %7.3f \n", gyroXangle, gyroYangle, gyroZangle, rate_gyr_x, rate_gyr_y, rate_gyr_z);
	
	//Each loop should be at least 20ms.
        while(mymillis() - startInt < 20)
        {
            usleep(100);
        }

	//printf("Loop Time %d\t", mymillis()- startInt);
	
	
}

int main(int argc, char *argv[])
{
	rate_gyr_y = 0.0;   // [deg/s]
	rate_gyr_x = 0.0;    // [deg/s]
	rate_gyr_z = 0.0;     // [deg/s]


	Pgyr_raw = gyr_raw;


	gyroXangle = 0.0;
	gyroYangle = 0.0;
	gyroZangle = 0.0;

	startInt  = mymillis();
	

	gyr_x = 0;
	gyr_y = 0;
	gyr_z = 0;
	counter = 0;

    signal(SIGINT, INThandler);

	enableIMU();

	gettimeofday(&tvBegin, NULL);
	
//OGL
//  Initialize GLUT and process user parameters
  glutInit(&argc,argv);
 
  //  Request double buffered true color window with Z-buffer
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
 
  // Create window
  glutCreateWindow("Awesome Cube");
  //glutReshapeWindow(600, 600);
  //  Enable Z-buffer depth test
  glEnable(GL_DEPTH_TEST);
 
  // Callback functions
  glutDisplayFunc(display);
  glutSpecialFunc(specialKeys);
  glutIdleFunc(readData);
  //  Pass control to GLUT for events
  glutMainLoop();
//OGL

	



	/*while(1)
	{
	
    }*/
}

