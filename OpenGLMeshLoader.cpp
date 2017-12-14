#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include "Car.h"
#include <algorithm>



int WIDTH = 1280;
int HEIGHT = 720;

double first_view = false;
double camera_alpha = -90.3;
bool manHit = false;
double time = 0;
float light = 0.7f;

boolean help_menu_opened = true;

int* lanes_random_number = new int[20];

struct Obstacle {
	double x;
	double z;
	int type;
};

Obstacle obstacles[1000];

int obsIdx = 0;

#define NUM_SCAN_OBS 3
#define CAR_LENGTH 13.0f
#define CAR_WIDTH 5.0f
#define CENTER_LEFT_LANE 0
#define LEFT_LANE 1
#define CENTER_RIGHT_LANE 2
#define RIGHT_LANE 3
#define DEFAULT_CAR_DISP 2.25f


GLuint tex;
GLuint tex_boat;
GLuint tex_help;

char title[] = "Crazy Car !";

// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 1600;

class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	//================================================================================================//
	// Operator Overloading; In C++ you can override the behavior of operators for you class objects. //
	// Here we are overloading the += operator to add a given value to all vector coordinates.        //
	//================================================================================================//
	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;
	}
};

Vector Eye(-10, 5, -10);
Vector At(5, 0, 5);
Vector Up(0, 1, 0);

int cameraZoom = 0;

// Model Variables
Model_3DS model_house;
Model_3DS model_boat;
Model_3DS model_tree;
Model_3DS model_car;
Model_3DS model_wheel;
Model_3DS model_umbrella;
Model_3DS model_road_cone;
Model_3DS model_barrel;
Model_3DS model_building;
Model_3DS model_man;

// Textures
GLTexture tex_farm;
GLTexture tex_city;
GLTexture tex_beach;
GLTexture tex_beach_street;
GLTexture tex_street;
GLTexture tex_race_end;

//variables
double ground;

float MAX_SPEED = 4.0f;
#define ROTATION_MULTIPLIER 8.0f
#define DRAG 0.04f
#define MAX_CAR_ROT_ANGLE 3.0f
#define MAX_CAR_LR_DISP 13.0f
Car car;
float speed = 0;
float carLRDisp = 0.0f;
bool accelerating = false;
bool braking = false;
bool leftTurn = false;
bool rightTurn = false;
float carRotationAngle = 0.0f;
bool lights = false;
//=======================================================================
// Lighting Configuration Function
//=======================================================================

void InitLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources
	glEnable(GL_LIGHT0);


	// Finally, define light source 0 position in World Space
	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	//car headlights
	GLfloat light_ambient2[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat specular2[] = { 1.0f, 1.0f, 1.0f , 1.0f };
	GLfloat light_position2[] = { 5.0, 2.0, 4.0, 1.0 };
	glLightfv(GL_LIGHT1, GL_POSITION, light_position);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 45.0);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.1f);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_ambient2);
}

//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	//*******************************************************************************************//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*******************************************************************************************//

	InitLightSource();

	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

//=======================================================================
// Render Ground Function
//=======================================================================
void RenderGround(GLTexture groundTex)
{
	//glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, groundTex.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -20);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, 20);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, 20);
	glEnd();
	glPopMatrix();

	//glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}


void drawCube(double length, GLTexture texture, double texture_width) {

	//glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	if (texture.texture[0])
		glBindTexture(GL_TEXTURE_2D, texture.texture[0]);	// Bind the ground texture

	static GLfloat n[6][3] =
	{
		{ -1.0, 0.0, 0.0 },
		{ 0.0, 1.0, 0.0 },
		{ 1.0, 0.0, 0.0 },
		{ 0.0, -1.0, 0.0 },
		{ 0.0, 0.0, 1.0 },
		{ 0.0, 0.0, -1.0 }
	};
	static GLint faces[6][4] =
	{
		{ 0, 1, 2, 3 },
		{ 3, 2, 6, 7 },
		{ 7, 6, 5, 4 },
		{ 4, 5, 1, 0 },
		{ 5, 6, 2, 1 },
		{ 7, 4, 0, 3 }
	};
	GLfloat v[8][3];
	GLint i;

	v[0][0] = v[1][0] = v[2][0] = v[3][0] = -length / 2;
	v[4][0] = v[5][0] = v[6][0] = v[7][0] = length / 2;
	v[0][1] = v[1][1] = v[4][1] = v[5][1] = -length / 2;
	v[2][1] = v[3][1] = v[6][1] = v[7][1] = length / 2;
	v[0][2] = v[3][2] = v[4][2] = v[7][2] = -length / 2;
	v[1][2] = v[2][2] = v[5][2] = v[6][2] = length / 2;

	for (i = 5; i >= 0; i--) {

		glBegin(GL_QUADS);
		glNormal3fv(&n[i][0]);
		glTexCoord2f(0, 0);
		glVertex3fv(&v[faces[i][0]][0]);
		glTexCoord2f(texture_width, 0);
		glVertex3fv(&v[faces[i][1]][0]);
		glTexCoord2f(texture_width, texture_width);
		glVertex3fv(&v[faces[i][2]][0]);
		glTexCoord2f(0, texture_width);
		glVertex3fv(&v[faces[i][3]][0]);
		glEnd();
	}



	//glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

void drawSphere(double radius) {
	GLUquadricObj * qobj;
	qobj = gluNewQuadric();
	//glTranslated(50, 0, 0);
	//glRotated(90, 1, 0, 1);
	//glBindTexture(GL_TEXTURE_2D, tex);
	//gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, radius, 100, 100);
	gluDeleteQuadric(qobj);
}

void drawWall(double thickness, GLTexture texture, double texture_width) {
	glPushMatrix();
	glTranslated(0.5, 0.5 * thickness, 0.5);
	glScaled(1.0, thickness, 1.0);
	drawCube(1, texture, texture_width);
	glPopMatrix();
}

//text function
void drawBitmapText(char *string, float x, float y, float z)
{
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(1.0f, 0.0f, 0.0f);//needs to be called before RasterPos
	glRasterPos2i(10, 10);
	char *c;
	glRasterPos3f(x, y, z);

	for (c = string; *c != '\0'; c++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
	}
	glPopMatrix();
	glEnable(GL_LIGHTING);
}
void drawBitmapText2D(char *string, float x, float y)
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, WIDTH, 0.0, HEIGHT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2i(x, y);
	char *c;
	for (c = string; *c != '\0'; c++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
	}
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
}

//=======================================================================
// Obtacles
//=======================================================================


int obstacles_index = 0;
void draw_road_cone(double distance, int lane) {
	glPushMatrix();
	glRotatef(45, 0, 1, 0);
	glTranslated(0, 0, distance);

	obstacles[obstacles_index].z = distance;
	obstacles[obstacles_index].type = 2;
	if (lane == LEFT_LANE) {
		glTranslated(6, 0, 0);//bring house left of the road
		obstacles[obstacles_index].x = 6;
	}
	else if (lane == CENTER_LEFT_LANE) {
		glTranslated(2, 0, 0);//bring house left of the road
		obstacles[obstacles_index].x = 2;
	}
	else if (lane == CENTER_RIGHT_LANE) {
		glTranslated(-2, 0, 0);//bring house left of the road
		obstacles[obstacles_index].x = -2;
	}
	else if (lane == RIGHT_LANE) {
		glTranslated(-6, 0, 0);//bring house left of the road
		obstacles[obstacles_index].x = -6;
	}
	glScaled(0.03, 0.03, 0.03);
	model_road_cone.Draw();
	glPopMatrix();
}

void draw_barrel(double distance, int lane) {
	glPushMatrix();
	glRotatef(45, 0, 1, 0);
	glTranslated(0, 0, distance);
	obstacles[obstacles_index].z = distance;
	obstacles[obstacles_index].type = 1;
	if (lane == LEFT_LANE) {
		glTranslated(6, 0, 0);//bring house left of the road
		obstacles[obstacles_index].x = 6;
	}
	else if (lane == CENTER_LEFT_LANE) {
		glTranslated(2, 0, 0);//bring house left of the road
		obstacles[obstacles_index].x = 2;
	}
	else if (lane == CENTER_RIGHT_LANE) {
		glTranslated(-2, 0, 0);//bring house left of the road
		obstacles[obstacles_index].x = -2;
	}
	else if (lane == RIGHT_LANE) {
		glTranslated(-6, 0, 0);//bring house left of the road
		obstacles[obstacles_index].x = -6;
	}
	glTranslated(0, -1, 0);
	glScaled(0.02, 0.02, 0.02);
	model_barrel.Draw();
	glPopMatrix();
}

void draw_man(double distance, int lane) {
	glPushMatrix();
		glRotatef(45, 0, 1, 0);
		glTranslated(0, 0, distance);

		obstacles[obstacles_index].z = distance;
		obstacles[obstacles_index].type = 3;
		if (lane == LEFT_LANE) {
			glTranslated(6, 0, 0);//bring house left of the road
			obstacles[obstacles_index].x = 6;
		}
		else if (lane == CENTER_LEFT_LANE) {
			glTranslated(2, 0, 0);//bring house left of the road
			obstacles[obstacles_index].x = 2;
		}
		else if (lane == CENTER_RIGHT_LANE) {
			glTranslated(-2, 0, 0);//bring house left of the road
			obstacles[obstacles_index].x = -2;
		}
		else if (lane == RIGHT_LANE) {
			glTranslated(-6, 0, 0);//bring house left of the road
			obstacles[obstacles_index].x = -6;
		}

		glScaled(2, 2, 2);
		if(!manHit)
			model_man.Draw();
	glPopMatrix();
}


//=======================================================================
// Display Function
//=======================================================================

bool obstacles_sorter(Obstacle const& lhs, Obstacle const& rhs) {
	return lhs.z < rhs.z;
}

int level = 1;
int rand_trees_num = rand() % 2 + 2;
int car_status = 5;
double sky_theta = 0;
double random_man_distance = rand() % 600 + 50;
double random_man_lane = rand() % 4;

void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	obstacles_index = 0;


	GLfloat lightSpecular[] = { light, light, light, light };
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	GLfloat lightDiffuse[] = { light, light, light, light };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	GLfloat lightIntensity[] = { light, light, light, light };
	GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);

	//help menu
	if (help_menu_opened){

		glPushMatrix();
			glEnable(GL_TEXTURE_2D);
			glDisable(GL_LIGHTING);
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			gluOrtho2D(0.0, WIDTH, 0.0, HEIGHT);
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();

			glBindTexture(GL_TEXTURE_2D, tex_help);
			//glRotated(180, 0, 0, 1);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0);			glVertex3f(0, 0, 0);
			glTexCoord2f(0.0f, 1.0f);			glVertex3f(0, HEIGHT, 0);
			glTexCoord2f(1, 1.0f);			glVertex3f(WIDTH, HEIGHT, 0);
			glTexCoord2f(1, 0);				glVertex3f(WIDTH, 0, 0);
			glEnd();


			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glEnable(GL_LIGHTING);
			glEnable(GL_TEXTURE_2D);

		glPopMatrix();

	}
	


	//draw Time
	glPushMatrix();

	//glTranslated(-5.5, 0, 5.5);//place it right
	char timestr[512];
	sprintf(timestr, "Time: %g s", time);//converts double to string
	//drawBitmapText(timestr, Eye.x + 10, 0, Eye.z + 10); // moves with the camera
	drawBitmapText2D(timestr, WIDTH-140, 10); // moves with the camera

	glPopMatrix();

	//draw Level
	glPushMatrix();
	//glColor3d(1, 1, 1);
	char levelstr[512];
	sprintf(levelstr, "Level: %d", level);//converts double to string
	drawBitmapText2D(levelstr, (WIDTH / 2)-50, 10); // moves with the camera
	glPopMatrix();


	//draw Car Status
	glPushMatrix();
	//glColor3d(1, 1, 1);
	char statusStr[512];
	sprintf(statusStr, "Car Status: %d Car Speed: %d Km/h", car_status, (int)(speed * 10));//converts double to string
	//sprintf(statusStr, "Light: %f sky_theta: %g Km/h", light, sky_theta);//converts double to string
	drawBitmapText2D(statusStr, 10, 10); // moves with the camera
	glPopMatrix();





	glPushMatrix(); //scenes
	glTranslated(ground, 0, ground);



	//obstacles
	glPushMatrix();

	draw_man((int)(random_man_distance*level) % 650+50, (int)(random_man_lane*level) % 4);
	obstacles_index++;

	for (double i = 50; i < 750; i += 200 / level) {
		/*draw_road_cone(i, LEFT_LANE);
		draw_road_cone(i+100, RIGHT_LANE);
		draw_road_cone(i, CENTER_LEFT_LANE);
		draw_road_cone(i + 100, CENTER_RIGHT_LANE);*/
		draw_road_cone(i, lanes_random_number[(int)obstacles_index % 20]);
		obstacles_index++;
	}

	for (double i = 30; i < 750; i += 250 / level) {
		/*draw_barrel(i, LEFT_LANE);
		draw_barrel(i + 100, RIGHT_LANE);*/
		draw_barrel(i, lanes_random_number[(int)obstacles_index % 19 + 1]);
		obstacles_index++;
	}
	glPopMatrix();

	//sorting obstacles
	std::sort(obstacles, obstacles + obstacles_index, &obstacles_sorter);

	//drawing Beach env
	for (int i = 0; i < 5; i++) {
		glPushMatrix();

		// Draw Ground
		glPushMatrix();
		glRotated(45, 0, 1, 0);
		glTranslated(0, 0, i * 50);
		glScaled(1.3, 1, 2);
		RenderGround(tex_beach);
		glPopMatrix();


		//draw Street
		for (double j = -0.6; j < 1.6; j++) {
			glPushMatrix();
			glRotated(45, 0, 1, 0);
			glTranslated(0, 0, j * 20 + i * 50);
			glScaled(2.5, 1, 2);
			glScaled(7, 1, 10);
			glTranslated(-0.5, 0, -0.5);
			drawWall(0.02, tex_beach_street, 1);//street
			glPopMatrix();
		}



		for (int j = 0; j < rand_trees_num; j++) {
			// Draw umbrella Model
			glPushMatrix();
			glTranslated(i * 40 + -j * 10, 0, i * 40 + -j * 10);
			if (j % 2 == 1)
				glTranslatef(-20, 0, 0);//bring it right
			else
				glTranslatef(20, 0, 0);//bring it right

			glTranslatef(0, 2, 0);
			glScalef(50, 50, 50);
			model_umbrella.Draw();
			glPopMatrix();
		}


		// Draw boat Model
		glPushMatrix();
		glTranslated(i * 30, 0, i * 30);
		glTranslated(10, 0, 0);//bring house left of the road
		glScaled(0.5, 0.5, 0.5);
		model_boat.Draw();
		glPopMatrix();




		glPopMatrix();

	}

	//drawing Farm env
	for (double i = 5.6; i < 9.6; i += 1.6) {
		glPushMatrix();

		// Draw Ground
		glPushMatrix();
		glRotated(45, 0, 1, 0);
		glTranslated(0, 0, i * 50);
		glScaled(1.3, 1, 2);
		RenderGround(tex_farm);
		glPopMatrix();


		//draw Street
		glPushMatrix();
		glRotated(45, 0, 1, 0);
		glTranslated(0, 0, i * 50);
		glScaled(2.5, 1, 2);
		glScaled(7, 1, 40);
		glTranslated(-0.5, 0, -0.5);
		drawWall(0.02, tex_street, 1);//street
		glPopMatrix();


		for (int j = 0; j < rand_trees_num; j++) {
			// Draw Tree Model
			glPushMatrix();
			glTranslated(i * 35 + -j * 10, 0, i * 35 + -j * 10);
			if (j % 2 == 1)
				glTranslatef(-17, 0, 0);//bring it right
			else
				glTranslatef(17, 0, 0);//bring it right
			glScalef(0.7, 0.7, 0.7);
			model_tree.Draw();
			glPopMatrix();
		}


		// Draw house Model
		glPushMatrix();
		glTranslated(i * 30, 0, i * 30);
		glTranslated(20, 0, 0);//bring house left of the road
		glRotatef(250.f, 0, 1, 0);
		glRotatef(90.f, 1, 0, 0);
		glTranslated(0, -8, 0);
		model_house.Draw();
		glPopMatrix();


		glPopMatrix();

	}




	//drawing City env
	for (double i = 10.3; i < 18.3; i += 1.3) {
		glPushMatrix();

		// Draw Ground
		glPushMatrix();
		glRotated(45, 0, 1, 0);
		glTranslated(0, 0, i * 50);
		glScaled(1.3, 1, 2);
		RenderGround(tex_city);
		glPopMatrix();


		//draw Street
		glPushMatrix();
		glRotated(45, 0, 1, 0);
		glTranslated(0, 0, i * 50);
		glScaled(2.5, 1, 2);
		glScaled(7, 1, 40);
		glTranslated(-0.5, 0, -0.5);
		drawWall(0.02, tex_street, 1);//street
		glPopMatrix();


		//drawing buldings
		glPushMatrix();
		glTranslated(i * 32, 0, i * 32);
		glPushMatrix();
		glTranslated(25, 0, 0);//bring building left of the road
		glScaled(1, ((int)(i + 2) % 3) + 1, 1);
		glScaled(1, 0.5, 1);
		glScaled(0.05, 0.05, 0.05);
		glRotated(-45, 0, 1, 0);
		model_building.Draw();
		glPopMatrix();
		glPushMatrix();
		glTranslated(15, 0, 35);//bring building right of the road
		glScaled(1, ((int)i % 3) + 1, 1);
		glScaled(1, 0.5, 1);
		glScaled(0.05, 0.05, 0.05);
		glRotated(-45, 0, 1, 0);
		model_building.Draw();
		glPopMatrix();
		glPopMatrix();


		glPopMatrix();

	}
	//race end 
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glRotated(45, 0, 1, 0);
	glTranslated(0, 0, 760);
	glScaled(7 * 2.5, 1, 20);
	//glScaled(7, 1, 40);
	glTranslated(-0.5, 0.3, -0.5);
	drawWall(0.1, tex_race_end, 1);//street
	glEnable(GL_LIGHTING);
	glPopMatrix();



	////drawing last Beach env
	//for (double i = 15.8; i < 18; i++){
	//	glPushMatrix();

	//	// Draw Ground
	//	glPushMatrix();
	//		glRotated(45, 0, 1, 0);
	//		glTranslated(0, 0, i * 50);
	//		glScaled(1.3, 1, 2);
	//		RenderGround(tex_beach);
	//	glPopMatrix();


	//	//draw Street
	//	for (double j = -0.6; j < 1.6; j++){
	//		glPushMatrix();
	//			glRotated(45, 0, 1, 0);
	//			glTranslated(0, 0,j*20 + i * 50);
	//			glScaled(1, 1, 2);
	//			glScaled(7, 1, 10);
	//			glTranslated(-0.5, 0, -0.5);
	//			drawWall(0.02, tex_beach_street, 1);//street
	//		glPopMatrix();
	//	}
	//


	//	for (int j = 0; j < rand_trees_num; j++){
	//		// Draw umbrella Model
	//		glPushMatrix();
	//		glTranslated(i * 40 + -j * 10, 0, i * 40 + -j * 10);
	//		if (j % 2 == 1)
	//			glTranslatef(-10, 0, 0);//bring it right
	//		else
	//			glTranslatef(10, 0, 0);//bring it right

	//		glTranslatef(0, 2, 0);
	//		glScalef(50, 50, 50);
	//		model_umbrella.Draw();
	//		glPopMatrix();
	//	}


	//	// Draw boat Model
	//	glPushMatrix();
	//		glTranslated(i * 30, 0, i * 30);
	//		glTranslated(10, 0, 0);//bring house left of the road
	//		glScaled(0.5, 0.5, 0.5);
	//		model_boat.Draw();
	//	glPopMatrix();


	//	glPopMatrix();

	//}


	glPopMatrix(); //scenes



				   // Draw car Model
	glPushMatrix();
	glScalef(0.5f, 0.5f, 0.5f);
	glRotated(45, 0, 1, 0);
	glTranslatef(DEFAULT_CAR_DISP + carLRDisp, 0.0f, 0.0f);
	if (speed > 0)
		glRotatef(carRotationAngle, 0, 1, 0);
	car.drawCar();
	glPopMatrix();




	//sky box
	glPushMatrix();

	GLUquadricObj * qobj;
	qobj = gluNewQuadric();
	//glTranslated(50,0,0);
	glRotated(sky_theta, 1, 0, 0);
	glRotated(30, 1, 0, 0);
	glRotated(180, 0, 1, 0);
	//glRotated(90, 1, 0, 1);
	glBindTexture(GL_TEXTURE_2D, tex);
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, zFar / 2, 100, 100);
	gluDeleteQuadric(qobj);


	glPopMatrix();



	glutSwapBuffers();
}

//=======================================================================
// ground animation Function
//=======================================================================
void ground_motion(int val)//timer animation function, allows the user to pass an integer valu to the timer function.
{
	speed -= DRAG;
	if (speed < 0.0f)
		speed = 0.0f;
	else
	{
		car.setWheelRotation(car.getWheelRotation() + speed*ROTATION_MULTIPLIER);
	}
	if (ground < -535) {
		manHit = false;
		ground = 0;
		obsIdx = 0;
		speed = 0;
		level++;
		obstacles_index = 0;
		for (int i = 0; i < 20; i++) {
			lanes_random_number[i] = rand() % 4;
		}
		//glutTimerFunc(50, ground_motion, 0);
	}

	ground -= speed;

	//glutPostRedisplay();						// redraw 		
	glutTimerFunc(15, ground_motion, 0);					//recall the time function after 1000 ms and pass a zero value as an input to the time func.
}

//=======================================================================
// camera animation Function
//=======================================================================
void camera_motion(int val)//timer animation function, allows the user to pass an integer valu to the timer function.
{
	if (Eye.x > zFar)
		return;

	Eye.x += 0.2;
	Eye.z += 0.2;

	glLoadIdentity();	//Clear Model_View Matrix

	glLoadIdentity();	//Clear Model_View Matrix

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters

	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	//glutPostRedisplay();						// redraw 		
	glutTimerFunc(30, camera_motion, 0);					//recall the time function after 1000 ms and pass a zero value as an input to the time func.
}


void reset_camera_position() {
	Eye.x = -20;
	Eye.z = -20;

	glLoadIdentity();	//Clear Model_View Matrix

	glLoadIdentity();	//Clear Model_View Matrix

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters

	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	//glutPostRedisplay();
}

//=======================================================================
// Keyboard Function
//=======================================================================
void firstPersonCamera()
{
	if (first_view)
	{
		float adj = (carLRDisp / 2) / sqrt(2);
		Eye.x = 0.55 + adj; Eye.y = 2; Eye.z = 0.65 - adj;
		At.x = zFar; At.z = zFar;
		glLoadIdentity();	//Clear Model_View Matrix
		gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters
	}
}
void myKeyboard(unsigned char button, int x, int y)
{
	switch (button)
	{

	case '1':
		if (!first_view){
			first_view = true;
			firstPersonCamera();
		}
		break;
	case '0':
	{
		first_view = false;
		At.x = zFar; At.z = zFar;
		Eye.x = -10; Eye.y = 5; Eye.z = -10;
		glLoadIdentity();	//Clear Model_View Matrix
		gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters
	}  break;
	case '3':
	{
		first_view = false;
		At.x = 5; At.z = 5;
		Eye.x = -9; Eye.y = 5; Eye.z = -9;
		glLoadIdentity();	//Clear Model_View Matrix
		gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters
	}  break;
	case '+':
		if (!first_view){

			//At.x = 5; At.z = 5;

			if (camera_alpha > -88.8){
				break;
			}


			int r = 15;
			camera_alpha += 0.02;
			Eye.x = r*sin(camera_alpha) + 1;
			Eye.z = r*cos(camera_alpha) + 1;
			/*Eye.z += 0.2;
			At.x = 5; At.z = 5;*/
			glLoadIdentity();	//Clear Model_View Matrix
			gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters
		}
		break;
	case '-':
		if (!first_view){
			//Eye.x += 0.2;
			//At.x = 5; At.z = 5;
			
			if (camera_alpha < -91.8){
				break;
			}

			int r = 15;
			camera_alpha -= 0.02;
			Eye.x = r*sin(camera_alpha) + 1;
			Eye.z = r*cos(camera_alpha) + 1;

			glLoadIdentity();	//Clear Model_View Matrix
			gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters
		}
		break;
	case 'g':
		glutTimerFunc(0, ground_motion, 0);
		break;
	case 'c':
		glutTimerFunc(0, camera_motion, 0);
		break;
	case 'r':
		reset_camera_position();
		break;
	case 'w':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'b':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 'l':
	{
		lights = !lights;
		if (lights)
			glDisable(GL_LIGHT1);
		else
			glEnable(GL_LIGHT1);
	}	break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}

	//glutPostRedisplay();
}

//=======================================================================
// Motion Function
//=======================================================================
void myMotion(int x, int y)
{
	y = HEIGHT - y;

	if (cameraZoom - y > 0)
	{
		Eye.x += -0.1;
		Eye.z += -0.1;
	}
	else
	{
		Eye.x += 0.1;
		Eye.z += 0.1;
	}

	cameraZoom = y;

	glLoadIdentity();	//Clear Model_View Matrix

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters

	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	//glutPostRedisplay();	//Re-draw scene 
}

//=======================================================================
// Mouse Function
//=======================================================================
void myMouse(int button, int state, int x, int y)
{
	y = HEIGHT - y;

	if (state == GLUT_DOWN)
	{
		cameraZoom = y;
	}
}

//=======================================================================
// Reshape Function
//=======================================================================
void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}

//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	// Loading Model files
	model_house.Load("Models/house/house.3DS");
	model_boat.Load("Models/boat/Cannoe.3ds");
	model_tree.Load("Models/tree/Tree1.3ds");
	model_umbrella.Load("Models/umbrella/Umbrella N040608.3ds");
	model_road_cone.Load("Models/road_cone.3ds");
	model_barrel.Load("Models/barrel.3ds");
	model_building.Load("Models/skyA.3ds");
	model_car.Load("Models/car/car2.3ds");
	model_wheel.Load("Models/wheel/wheel4.3ds");
	model_man.Load("Models/human/man.3ds");
	car = Car(model_car, model_wheel, 3.0f, 0.082f);

	// Loading texture files
	tex_city.Load("Textures/city.bmp");
	tex_farm.Load("Textures/ground.bmp");
	tex_beach.Load("Textures/beach.bmp");
	tex_street.Load("Textures/street4Lanes.bmp");
	tex_beach_street.Load("Textures/beach_street.bmp");
	tex_race_end.Load("Textures/race_end.bmp");
	loadBMP(&tex, "Textures/sky5-jpg.bmp", true);
	loadBMP(&tex_help, "Textures/help.bmp", true);
}

//=======================================================================
// Animation Functions
//=======================================================================
double isGettingDark = 1;
void sky_animation(int val) {
	//if (isGettingDark ==1 && light>=0.1){
	//	light -= 0.0015f;
	//}
	//else if (isGettingDark == 0 &&light <= 0.7){
	//	light += 0.0015f;
	//}

	//if (sky_theta>50 && sky_theta<100){
	//	isGettingDark = 1;
	//}
	//else if (sky_theta>240 && sky_theta<300){
	//	isGettingDark = 0;
	//}
	//else{
	//	isGettingDark = -1;//off
	//}


	sky_theta = (-ground / 530) * 80;
	light = 1 - (-ground / 350) * 1;
	if (light < 0) {
		light = 0;
	}

	//sky_theta += 0.1;
	if (sky_theta > 360)
		sky_theta = 0;
	//glutPostRedisplay();
	glutTimerFunc(100, sky_animation, 0);
}
void time_counter(int val)//timer animation function, allows the user to pass an integer valu to the timer function.
{
	time += 0.1;

	//glutPostRedisplay();						// redraw 		
	glutTimerFunc(100, time_counter, 0);					//recall the time function after 1000 ms and pass a zero value as an input to the time func.
}
void carControlTimer(int val)
{
	if (accelerating)
	{
		if (speed < MAX_SPEED - 0.1f)
			speed += 0.1f;
	}
	else if (braking)
	{
		speed -= 0.2f;
		if (speed < 0)
			speed = 0.0f;
	}
	if (leftTurn)
	{
		carLRDisp += 0.24f * speed;
		if (carLRDisp > MAX_CAR_LR_DISP)
			carLRDisp = MAX_CAR_LR_DISP;
		if (carRotationAngle < MAX_CAR_ROT_ANGLE - 0.8f)
			carRotationAngle += 0.8f;
		firstPersonCamera();
	}
	else if (rightTurn)
	{
		carLRDisp -= 0.24f * speed;
		if (carLRDisp < -MAX_CAR_LR_DISP)
			carLRDisp = -MAX_CAR_LR_DISP;
		if (carRotationAngle > -MAX_CAR_ROT_ANGLE + 0.8f)
			carRotationAngle -= 0.8f;
		firstPersonCamera();
	}
	glutTimerFunc(15, carControlTimer, 0);
}

bool car_motor_sound = false;
bool car_brakes_sound = false;

void SpecialInput(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
	{
		if (!car_motor_sound){
			car_motor_sound = true;
			PlaySound(TEXT("sounds/car_motor.wav"), NULL, SND_ASYNC | SND_LOOP);
		}
		accelerating = true;
	}
	break;
	case GLUT_KEY_DOWN:
	{
		if (!car_brakes_sound){
			car_brakes_sound = true;
			if (speed !=0)
				PlaySound(TEXT("sounds/car_brakes.wav"), NULL, SND_FILENAME | SND_ASYNC);
		}
		braking = true;
	}
	break;
	case GLUT_KEY_LEFT:
	{
		leftTurn = true;
		car.setCarDirection(LEFT);
	}
	break;
	case GLUT_KEY_RIGHT:
	{
		rightTurn = true;
		car.setCarDirection(RIGHT);
	}
	break;
	}

	//glutPostRedisplay();
}
void largeObject()
{
	ground = 0;
	carLRDisp = 0;
	speed = 0;
	obsIdx = 0;
	manHit = false;

	if (car_status == 0)
	{
		//game over
	}
	else
		car_status -= 1;
}
void restoreSpeed(int in)
{
	MAX_SPEED = 4.0f;
}
void smallObject()
{
	MAX_SPEED = 1.0f;
	glutTimerFunc(2500, restoreSpeed, 0);
}
void powerUp()
{
	MAX_SPEED = MAX_SPEED + MAX_SPEED/2.0f;
	glutTimerFunc(2500, restoreSpeed, 0);
	manHit = true;
}

boolean car_crash_sound = false;
void collisionDetection(int in)
{
	float sqr = sqrt(2);
	float carFront = (-ground*sqr) + CAR_LENGTH / 4;
	float carBack = carFront - CAR_LENGTH / 2;
	while (obstacles[obsIdx].z < carBack)
		obsIdx++;
	float carLeft = ((DEFAULT_CAR_DISP + carLRDisp)) / 2;
	float carRight = (carLeft - CAR_WIDTH / 2);

	for (int i = obsIdx; i <= obsIdx + NUM_SCAN_OBS; i++)
	{
		float obsZ = obstacles[i].z;
		if (obsZ == 0.0f)
			continue;
		float obsX = obstacles[i].x;

		if (obsZ <= carFront && obsZ >= carBack && obsX <= carLeft && obsX >= carRight)
		{
			//time = 0;
			if (obstacles[i].type == 3){
				powerUp();
				PlaySound(TEXT("sounds/man_screaming.wav"), NULL, SND_FILENAME | SND_ASYNC);
			}
			else if (obstacles[i].type == 1){
				largeObject();
				PlaySound(TEXT("sounds/car_crash.wav"), NULL, SND_FILENAME | SND_ASYNC);
			}
			else if (obstacles[i].type == 2){
				smallObject();
				PlaySound(TEXT("sounds/small_object.wav"), NULL , SND_FILENAME | SND_ASYNC);
			}
		}
	}
	glutTimerFunc(50, collisionDetection, 0);
}
void specialkeyUpFunc(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
	{
		//PlaySound(TEXT("sounds/car_motor.wav"), NULL, SND_APPLICATION);
		PlaySound(NULL, NULL, 0);
		car_motor_sound = false;
		accelerating = false;
	}
	break;
	case GLUT_KEY_DOWN:
	{
		car_brakes_sound = false;
		braking = false;
	}
	break;
	case GLUT_KEY_LEFT:
	{
		leftTurn = false;
		car.setCarDirection(CENTER);
		carRotationAngle = 0.0f;
	}
	break;
	case GLUT_KEY_RIGHT:
	{
		rightTurn = false;
		car.setCarDirection(CENTER);
		carRotationAngle = 0.0f;
	}
	break;
	}

	//glutPostRedisplay();
}

void animate(int in)
{
	glutPostRedisplay();
	glutTimerFunc(30, animate, in);
}
//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	for (int i = 0; i < 20; i++) {
		lanes_random_number[i] = rand() % 4;
	}

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(50, 50);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);

	glutKeyboardFunc(myKeyboard);

	glutMotionFunc(myMotion);

	glutMouseFunc(myMouse);

	//glutReshapeFunc(myReshape);

	glutSpecialFunc(SpecialInput);

	glutTimerFunc(0, time_counter, 0);
	glutTimerFunc(0, sky_animation, 0);
	glutTimerFunc(10, ground_motion, 0);
	glutTimerFunc(15, carControlTimer, 0);
	glutTimerFunc(50, collisionDetection, 0);
	glutTimerFunc(0, animate, 0);
	glutSpecialUpFunc(specialkeyUpFunc);
	myInit();

	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}