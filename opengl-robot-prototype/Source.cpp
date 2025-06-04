#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <math.h>
#include <iostream>

#include "robotHead.h"
#include "robotArm.h"
#include "robotTorso.h"
#include "robotLeg.h"

#include "gun.h"
#include "shield.h"

using namespace std;

#pragma comment (lib, "OpenGL32.lib")
#pragma comment (lib, "GLU32.lib")

#define WINDOW_TITLE "OpenGL Window"

// Model type
int modelNo = 1;

// Rotation Variables 
int form = 0;
float x = 1.0, y = 1.0, z = 1.0;
float rotationAngle[3] = { 0, 0, 0 };
float rotationSpeed[3] = { 15.0f, 15.0f, 15.0f };

float leftLowerLegRotationSpeed = 0;
float rightLowerLegRotationSpeed = 0;
float leftLegRotationSpeed[3] = { 0, 0, 0 };
float rightLegRotationSpeed[3] = { 0, 0, 0 };
float headRotationSpeed[2] = { 0, 0 };
float leftArmRotationSpeed[3] = { 0, 0, 0 };
float rightArmRotationSpeed[3] = { 0, 0, 0 };
float lowerLeftArmRotationSpeed[3] = { 0, 0, 0 };
float lowerRightArmRotationSpeed[3] = { 0, 0, 0 };

float sunRotation = 0;
float sunRevolutionIncrement = 0.0005f;
float lightRotationAngle = 0.0f;

bool isPaused = false;
bool rotationMode = true;


// Animation Testing 
boolean atOrigin = true;
float changeRate = 0.2;
float robotMovement[8] = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 };
float runningRate = 0.2;

// Translation
float modelTranslation[3] = { 0, 0, 0 };
float jetpackSpeed = 0.1;

// Camera Variables
float const ORIGINALCAMERALEFT = -3.0f;
float const ORIGINALCAMERARIGHT = 3.0f;
float const ORIGINALCAMERABOTTOM = -3.0f;
float const ORIGINALCAMERATOP = 3.0f;
float const ORIGINALCAMERANEAR = -5.1f;
float const ORIGINALCAMERAFAR = 100.1f;

float cameraLeft = ORIGINALCAMERALEFT;
float cameraRight = ORIGINALCAMERARIGHT;
float cameraBottom = ORIGINALCAMERABOTTOM;
float cameraTop = ORIGINALCAMERATOP;
float cameraNear = ORIGINALCAMERANEAR;
float cameraFar = ORIGINALCAMERAFAR;

float const ORIGINALCAMERAX = 1;
float const ORIGINALCAMERAY = 0;
float const ORIGINALCAMERAZ = 10;

float cameraX = ORIGINALCAMERAX;
float cameraY = ORIGINALCAMERAY;
float cameraZ = ORIGINALCAMERAZ;

float lookX = 0;
float lookY = 0;
float lookZ = 0;

float pAngle = 45;
float cameraChange = 0.1;

bool isOrtho = true;

// Light
GLfloat lightAmbient[] = { 1.0f, 1.0f, 1.0f };
GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { 5.0f, 2.0f, 0.0f };

GLfloat jetpackVal = 0;
GLfloat jetpackAmbient[] = { jetpackVal, jetpackVal, jetpackVal };
GLfloat jetpackDiffuse[] = { jetpackVal, jetpackVal, jetpackVal };
GLfloat jetpackLightPosition[] = { 0.005f, 0.222f, 0.364f };

GLfloat gunVal = 1;
GLfloat gunAmbient[] = { gunVal, gunVal, gunVal };
GLfloat gunDiffuse[] = { gunVal, gunVal, gunVal };
GLfloat gunLightPosition[] = { 0.04f, -0.65f, -0.08f };

bool lightsOn = true;

// Textures
GLuint blackTexture = 0;
GLuint grayTexture = 0;
GLuint gunTexture = 0;
GLuint silverTexture = 0;
GLuint shieldSpikesTexture = 0;
GLuint explosionTexture = 0;
GLuint sunTexture = 0;

GLuint robotEyeTexture = 0;
GLuint robotArmorTexture = 0;
GLuint robotInteriorTexture = 0;
GLuint robotAttireTexture = 0;

BITMAP BMP;
HBITMAP hBMP = NULL;

// Materials
GLfloat shinyAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat shinyDiffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat shinySpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat shinyShininess[] = { 100.0f };

// Default Values
// Source: https://stackoverflow.com/questions/10181201/opengl-light-changes-ambient-to-diffuse-or-specular-works-but-not-the-opposite#:~:text=If%20you%20don%27t%20want,1.0)%20(only%20for%20light0.
GLfloat defaultAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat defaultDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat defaultSpecular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat defaultShininess[] = { 0.0f };

// Skybox
GLuint skyFront = 0;
GLuint skyBack = 0;
GLuint skyTop = 0;
GLuint skyBottom = 0;
GLuint skyLeft = 0;
GLuint skyRight = 0;

// Gun
float bulletSpeed = 0;
float rateOfChange = 0;
boolean gunSwitch = false;

// Jetpack
float jetPackExplosionSize = 0;
float jetPackExplosionLength = 0;
float jetPackExplosionROC = 0;
int jetPackAnimationForm = 0;
int explosionX = 0;
int explosionY = 0;
int explosionZ = 0;
boolean jetPackSwitch = false;

// GLU Objects
GLUquadricObj* sphere = gluNewQuadric();
GLUquadricObj* cylinder = gluNewQuadric();
GLUquadricObj* sun = gluNewQuadric();

// Perspective / Orthographic Method
void enablePerspective()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(pAngle, 0.9, 1.0, 1000);
	gluLookAt(cameraX, cameraY, cameraZ, lookX, lookY, lookZ, 0.0f, 1.0f, 0.0f);
}

void enableOrthographic()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(cameraLeft, cameraRight, cameraBottom, cameraTop, cameraNear, cameraFar);
}

// Viewing Methods
void rotateCClockwise(int x)
{
	rotationAngle[x] += rotationSpeed[x];
}

void rotateClockwise(int x)
{
	rotationAngle[x] -= rotationSpeed[x];
}

// Toggles
void toggleView()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (isOrtho)
	{
		isOrtho = false;
	}
	else
	{
		isOrtho = true;
	}

	if (isOrtho)
	{
		enableOrthographic();
	}
	if (!isOrtho)
	{
		enablePerspective();
	}
}

void toggleRotation()
{
	if (rotationMode) rotationMode = false;
	else rotationMode = true;
}

void toggleLights() {
	if (lightsOn)
	{
		lightsOn = false;
	}
	else
	{
		lightsOn = true;
	}
}

void toggleRobotMovement(int form, int axis, float rotateSpeed) {
	if (form == 1) { // lower left leg
		leftLowerLegRotationSpeed += rotateSpeed;
	}
	else if (form == 2) { // lower right leg
		rightLowerLegRotationSpeed += rotateSpeed;
	}
	else if (form == 3) { // left leg
		leftLegRotationSpeed[axis] += rotateSpeed;
	}
	else if (form == 4) { // right leg
		rightLegRotationSpeed[axis] += rotateSpeed;
	}
	else if (form == 5) { // head
		headRotationSpeed[axis] += rotateSpeed;
	}
	else if (form == 6) { // left arm
		leftArmRotationSpeed[axis] += rotateSpeed;
	}
	else if (form == 7) { // right arm
		rightArmRotationSpeed[axis] += rotateSpeed;
	}
	else if (form == 8) { // lower left arm
		lowerLeftArmRotationSpeed[axis] += rotateSpeed;
	}
	else if (form == 9) { // lower right arm
		lowerRightArmRotationSpeed[axis] += rotateSpeed;
	}
}

// Reset Rotation
void resetRotation()
{
	for (int i = 0; i < 3; i++)
	{
		rotationAngle[i] = 0;

		leftLegRotationSpeed[i] = 0;
		rightLegRotationSpeed[i] = 0;
		leftArmRotationSpeed[i] = 0;
		rightArmRotationSpeed[i] = 0;
		lowerLeftArmRotationSpeed[i] = 0;
		lowerRightArmRotationSpeed[i] = 0;
	}

	leftLowerLegRotationSpeed = 0;
	rightLowerLegRotationSpeed = 0;
	headRotationSpeed[0] = 0;
	headRotationSpeed[1] = 0;
}

// Reset Orthographic Dimension
void resetOrthoDimension()
{
	cameraLeft = ORIGINALCAMERALEFT;
	cameraRight = ORIGINALCAMERARIGHT;
	cameraBottom = ORIGINALCAMERABOTTOM;
	cameraTop = ORIGINALCAMERATOP;
	cameraNear = ORIGINALCAMERANEAR;
	cameraFar = ORIGINALCAMERAFAR;
}

// Reset Look At
void resetLookAt()
{
	cameraX = ORIGINALCAMERAX;
	cameraY = ORIGINALCAMERAY;
	cameraZ = ORIGINALCAMERAZ;

	lookX = 0;
	lookY = 0;
	lookZ = 0;
}

void resetTranslation()
{
	for (int i = 0; i < 3; i++)
	{
		modelTranslation[i] = 0;
	}
}

// Reset light position
void pauseLight()
{
	if (!isPaused)
	{
		isPaused = true;
	}
	else
	{
		isPaused = false;
	}

	if (isPaused)
	{
		sunRevolutionIncrement = 0;
	}
	else
	{
		sunRevolutionIncrement = 0.0005f;
	}
}

// Reset Position
void resetPos()
{
	resetRotation();
	resetOrthoDimension();
	resetLookAt();
	resetTranslation();

	if (isOrtho)
	{
		enableOrthographic();
	}
	if (!isOrtho)
	{
		enablePerspective();
	}
}

void drawQuad(float x1, float y1, float z1,
	float x2, float y2, float z2,
	float x3, float y3, float z3,
	float x4, float y4, float z4)
{
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x1, y1, z1);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x2, y2, z2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x3, y3, z3);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x4, y4, z4);
	glEnd();
}

void drawSkyBox(float length, float height, float width)
{
	// Face 1 (Front)
	glBindTexture(GL_TEXTURE_2D, skyFront);
	drawQuad(-length, -height, width,   // Bottom Left
		-length, height, width,   // Top Left
		length, height, width,    // Top Right
		length, -height, width);  // Bottom Right

	// Face 2 (Right)
	glBindTexture(GL_TEXTURE_2D, skyRight);
	drawQuad(length, -height, width,   // Bottom Left
		length, height, width,   // Top Left
		length, height, -width,  // Top Right
		length, -height, -width); // Bottom Right

	// Face 3 (Back)
	glBindTexture(GL_TEXTURE_2D, skyLeft);
	drawQuad(length, -height, -width,  // Bottom Left
		length, height, -width,  // Top Left
		-length, height, -width, // Top Right
		-length, -height, -width); // Bottom Right

	// Face 4 (Left)
	glBindTexture(GL_TEXTURE_2D, skyBack);
	drawQuad(-length, -height, -width,  // Bottom Left
		-length, height, -width,  // Top Left
		-length, height, width,   // Top Right
		-length, -height, width); // Bottom Right

	// Face 5 (Top)
	glBindTexture(GL_TEXTURE_2D, skyTop);
	drawQuad(-length, height, -width,  // Bottom Left
		-length, height, width,  // Top Left
		length, height, width,   // Top Right
		length, height, -width); // Bottom Right

	// Face 6 (Bottom)
	glBindTexture(GL_TEXTURE_2D, skyBottom);
	drawQuad(-length, -height, width,  // Bottom Left
		-length, -height, -width, // Top Left
		length, -height, -width,  // Top Right
		length, -height, width); // Bottom Right
}

void sunRevolution()
{
	lightPosition[0] = 5.0 * cos(lightRotationAngle);
	lightPosition[2] = 5.0 * sin(lightRotationAngle);

	lightRotationAngle += sunRevolutionIncrement;
}

void applyMaterial(GLfloat ambient[], GLfloat diffuse[], GLfloat specular[], GLfloat shininess[]) {
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

bool checkAtOrigin()
{
	// Check if all rotation speeds are zero
	return (leftLowerLegRotationSpeed == 0 &&
		rightLowerLegRotationSpeed == 0 &&
		leftLegRotationSpeed[0] == 0 && leftLegRotationSpeed[1] == 0 && leftLegRotationSpeed[2] == 0 &&
		rightLegRotationSpeed[0] == 0 && rightLegRotationSpeed[1] == 0 && rightLegRotationSpeed[2] == 0 &&
		headRotationSpeed[0] == 0 && headRotationSpeed[1] == 0 &&
		leftArmRotationSpeed[0] == 0 && leftArmRotationSpeed[1] == 0 && leftArmRotationSpeed[2] == 0 &&
		rightArmRotationSpeed[0] == 0 && rightArmRotationSpeed[1] == 0 && rightArmRotationSpeed[2] == 0 &&
		lowerLeftArmRotationSpeed[0] == 0 && lowerLeftArmRotationSpeed[1] == 0 && lowerLeftArmRotationSpeed[2] == 0 &&
		lowerRightArmRotationSpeed[0] == 0 && lowerRightArmRotationSpeed[1] == 0 && lowerRightArmRotationSpeed[2] == 0);
}

void shieldPoseAnimation()
{
	if (leftLegRotationSpeed[1] > -40)
	{
		leftLegRotationSpeed[1] -= changeRate;
	}

	if (leftLegRotationSpeed[2] < 10)
	{
		leftLegRotationSpeed[2] += changeRate;
	}

	if (rightLegRotationSpeed[1] < 40)
	{
		rightLegRotationSpeed[1] += changeRate;
	}

	if (rightLegRotationSpeed[2] < 10)
	{
		rightLegRotationSpeed[2] += changeRate;
	}

	if (rightLowerLegRotationSpeed > -90)
	{
		rightLowerLegRotationSpeed -= changeRate;
	}

	if (rightArmRotationSpeed[1] < 60)
	{
		rightArmRotationSpeed[1] += changeRate;
	}

	if (rightArmRotationSpeed[2] < 20)
	{
		rightArmRotationSpeed[2] += changeRate;
	}

	if (rightArmRotationSpeed[0] < 40)
	{
		rightArmRotationSpeed[0] += changeRate;
	}

	if (lowerRightArmRotationSpeed[0] < 70)
	{
		lowerRightArmRotationSpeed[0] += changeRate;
	}

	if (lowerRightArmRotationSpeed[1] > -20)
	{
		lowerRightArmRotationSpeed[1] -= changeRate;
	}

	if (lowerRightArmRotationSpeed[2] > -40)
	{
		lowerRightArmRotationSpeed[2] -= changeRate;
	}

	if (leftArmRotationSpeed[1] < 90)
	{
		leftArmRotationSpeed[1] += changeRate;
	}

	if (leftArmRotationSpeed[2] < 10)
	{
		leftArmRotationSpeed[2] += changeRate;
	}
}

void gunPoseAnimation()
{
	if (leftArmRotationSpeed[0] < 70)
	{
		leftArmRotationSpeed[0] += changeRate;
	}

	if (leftArmRotationSpeed[1] < 100)
	{
		leftArmRotationSpeed[1] += changeRate;
	}

	if (headRotationSpeed[0] > -90)
	{
		headRotationSpeed[0] -= changeRate;
	}
}

void victoryPoseAnimation()
{
	if (rightArmRotationSpeed[1] < 180)
	{
		rightArmRotationSpeed[1] += 2 * changeRate;
	}

	if (rightArmRotationSpeed[2] < 30)
	{
		rightArmRotationSpeed[2] += 2 * changeRate;
	}

	if (lowerRightArmRotationSpeed[0] < 20)
	{
		lowerRightArmRotationSpeed[0] += 2 * changeRate;
	}

	if (leftArmRotationSpeed[1] < 180)
	{
		leftArmRotationSpeed[1] += 2 * changeRate;
	}

	if (leftArmRotationSpeed[2] < 30)
	{
		leftArmRotationSpeed[2] += 2 * changeRate;
	}

	if (lowerLeftArmRotationSpeed[0] > -20)
	{
		lowerLeftArmRotationSpeed[0] -= 2 * changeRate;
	}
}

void runningAnimation()
{
	if (leftLowerLegRotationSpeed > -30)
	{
		leftLowerLegRotationSpeed -= runningRate;
	}

	if (leftLegRotationSpeed[1] < -45 || leftLegRotationSpeed[1] > 45)
	{
		leftLegRotationSpeed[1] = (leftLegRotationSpeed[1] < -45) ? -45 : 45;
		robotMovement[1] = (leftLegRotationSpeed[1] < 0) ? -runningRate : runningRate;
	}

	if (rightLowerLegRotationSpeed > -30)
	{
		rightLowerLegRotationSpeed -= runningRate;
	}

	if (rightLegRotationSpeed[1] < -45 || rightLegRotationSpeed[1] > 45)
	{
		rightLegRotationSpeed[1] = (rightLegRotationSpeed[1] < -45) ? -45 : 45;
		robotMovement[3] = (rightLegRotationSpeed[1] < 0) ? runningRate : -runningRate;
	}

	// Arm
	if (leftArmRotationSpeed[1] < -45 || leftArmRotationSpeed[1] > 45)
	{
		leftArmRotationSpeed[1] = (leftArmRotationSpeed[1] < -45) ? -45 : 45;
		robotMovement[4] = (leftArmRotationSpeed[1] < 0) ? -runningRate : runningRate;
	}

	if (lowerLeftArmRotationSpeed[1] < 45)
	{
		lowerLeftArmRotationSpeed[1] += runningRate;
	}

	if (rightArmRotationSpeed[1] < -45 || rightArmRotationSpeed[1] > 45)
	{
		rightArmRotationSpeed[1] = (rightArmRotationSpeed[1] < -45) ? -45 : 45;
		robotMovement[6] = (rightArmRotationSpeed[1] < 0) ? runningRate : -runningRate;
	}

	if (lowerRightArmRotationSpeed[1] < 45)
	{
		lowerRightArmRotationSpeed[1] += runningRate;
	}

	// Leg
	leftLegRotationSpeed[1] -= robotMovement[1];
	rightLegRotationSpeed[1] += robotMovement[3];

	// Arm
	leftArmRotationSpeed[1] -= robotMovement[4];
	rightArmRotationSpeed[1] += robotMovement[6];
}

void returnToOGPos()
{
	// Left Lower Leg
	if (leftLowerLegRotationSpeed < -0.1)
	{
		leftLowerLegRotationSpeed += changeRate;
	}
	else if (leftLowerLegRotationSpeed > 0.1)
	{
		leftLowerLegRotationSpeed -= changeRate;
	}
	else
	{
		leftLowerLegRotationSpeed = 0;
	}

	// Right Lower Leg
	if (rightLowerLegRotationSpeed < -0.1)
	{
		rightLowerLegRotationSpeed += changeRate;
	}
	else if (rightLowerLegRotationSpeed > 0.1)
	{
		rightLowerLegRotationSpeed -= changeRate;
	}
	else
	{
		rightLowerLegRotationSpeed = 0;
	}

	for (int i = 0; i < 3; ++i)
	{
		// Left Leg
		if (leftLegRotationSpeed[i] < -0.1)
		{
			leftLegRotationSpeed[i] += changeRate;
		}
		else if (leftLegRotationSpeed[i] > 0.1)
		{
			leftLegRotationSpeed[i] -= changeRate;
		}
		else
		{
			leftLegRotationSpeed[i] = 0;
		}

		// Right Leg
		if (rightLegRotationSpeed[i] < -0.1)
		{
			rightLegRotationSpeed[i] += changeRate;
		}
		else if (rightLegRotationSpeed[i] > 0.1)
		{
			rightLegRotationSpeed[i] -= changeRate;
		}
		else
		{
			rightLegRotationSpeed[i] = 0;
		}

		// Left Arm
		if (leftArmRotationSpeed[i] < -0.1)
		{
			leftArmRotationSpeed[i] += changeRate;
		}
		else if (leftArmRotationSpeed[i] > 0.1)
		{
			leftArmRotationSpeed[i] -= changeRate;
		}
		else
		{
			leftArmRotationSpeed[i] = 0;
		}

		// Right Arm
		if (rightArmRotationSpeed[i] < -0.1)
		{
			rightArmRotationSpeed[i] += changeRate;
		}
		else if (rightArmRotationSpeed[i] > 0.1)
		{
			rightArmRotationSpeed[i] -= changeRate;
		}
		else
		{
			rightArmRotationSpeed[i] = 0;
		}

		// Lower Left Arm
		if (lowerLeftArmRotationSpeed[i] < -0.1)
		{
			lowerLeftArmRotationSpeed[i] += changeRate;
		}
		else if (lowerLeftArmRotationSpeed[i] > 0.1)
		{
			lowerLeftArmRotationSpeed[i] -= changeRate;
		}
		else
		{
			lowerLeftArmRotationSpeed[i] = 0;
		}

		// Lower Right Arm
		if (lowerRightArmRotationSpeed[i] < -0.1)
		{
			lowerRightArmRotationSpeed[i] += changeRate;
		}
		else if (lowerRightArmRotationSpeed[i] > 0.1)
		{
			lowerRightArmRotationSpeed[i] -= changeRate;
		}
		else
		{
			lowerRightArmRotationSpeed[i] = 0;
		}
	}

	// Head
	for (int i = 0; i < 2; ++i)
	{
		if (headRotationSpeed[i] < -0.1)
		{
			headRotationSpeed[i] += changeRate;
		}
		else if (headRotationSpeed[i] > 0.1)
		{
			headRotationSpeed[i] -= changeRate;
		}
		else
		{
			headRotationSpeed[i] = 0;
		}
	}

	atOrigin = checkAtOrigin();
}

void flyTowards(int x)
{
	modelTranslation[x] += jetpackSpeed;
}

void flyAway(int x)
{
	modelTranslation[x] -= jetpackSpeed;
}

LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) PostQuitMessage(0);
		// Toggles
		if (wParam == 'P') toggleView();
		if (wParam == '0') toggleRotation();
		if (wParam == '8') toggleLights();
		if (wParam == '7') pauseLight();

		// Reset Position
		if (wParam == VK_SPACE) resetPos();

		// Object Rotation
		if (wParam == '1')
		{
			modelNo = 1;
			atOrigin = false;
		}
		if (wParam == '2')
		{
			modelNo = 2;
			atOrigin = false;
		}
		if (wParam == '3')
		{
			modelNo = 3;
			atOrigin = false;
		}
		if (wParam == '4')
		{
			modelNo = 4;
			atOrigin = false;
		}
		if (wParam == '5')
		{
			modelNo = 5;
			atOrigin = false;
		}
		if (wParam == '6')
		{
			modelNo = 6;
			atOrigin = false;
		}
		if (wParam == 'W')
		{
			if (rotationMode)
			{
				rotateClockwise(0);
			}
			else
			{
				lookY += cameraChange;
				if (!isOrtho)
				{
					enablePerspective();
				}
				else
				{
					enableOrthographic();
				}
			}
		}
		if (wParam == 'A')
		{
			if (rotationMode)
			{
				rotateClockwise(1);
			}
			else
			{
				lookX -= cameraChange;
				if (!isOrtho)
				{
					enablePerspective();
				}
				else
				{
					enableOrthographic();
				}
			}
		}
		if (wParam == 'S')
		{
			if (rotationMode)
			{
				rotateCClockwise(0);
			}
			else
			{
				lookY -= cameraChange;
				if (!isOrtho)
				{
					enablePerspective();
				}
				else
				{
					enableOrthographic();
				}
			}
		}
		if (wParam == 'D')
		{
			if (rotationMode)
			{
				rotateCClockwise(1);
			}
			else
			{
				lookX += cameraChange;
				if (!isOrtho)
				{
					enablePerspective();
				}
				else
				{
					enableOrthographic();
				}
			}
		}
		if (wParam == 'Q')
		{
			if (rotationMode)
			{
				rotateCClockwise(2);
			}
			else
			{
				lookZ -= cameraChange;
				if (!isOrtho)
				{
					enablePerspective();
				}
				else
				{
					enableOrthographic();
				}
			}
		}
		if (wParam == 'E')
		{
			if (rotationMode)
			{
				rotateClockwise(2);
			}
			else
			{
				lookZ += cameraChange;
				if (!isOrtho)
				{
					enablePerspective();
				}
				else
				{
					enableOrthographic();
				}
			}
		}
		if (wParam == 'I')
		{
			if (isOrtho)
			{
				cameraTop += cameraChange;
				cameraBottom += cameraChange;

				enableOrthographic();
			}
			if (!isOrtho)
			{
				cameraY += cameraChange;
				enablePerspective();
			}
		}
		if (wParam == 'J')
		{
			if (isOrtho)
			{
				cameraLeft -= cameraChange;
				cameraRight -= cameraChange;

				enableOrthographic();
			}
			if (!isOrtho)
			{
				cameraX -= cameraChange;
				enablePerspective();
			}
		}
		if (wParam == 'K')
		{
			if (isOrtho)
			{
				cameraTop -= cameraChange;
				cameraBottom -= cameraChange;

				enableOrthographic();
			}
			if (!isOrtho)
			{
				cameraY -= cameraChange;
				enablePerspective();
			}
		}
		if (wParam == 'L')
		{
			if (isOrtho)
			{
				cameraLeft += cameraChange;
				cameraRight += cameraChange;

				enableOrthographic();
			}
			if (!isOrtho)
			{
				cameraX += cameraChange;
				enablePerspective();
			}
		}
		if (wParam == 'U')
		{
			if (isOrtho)
			{
				cameraNear -= cameraChange;
				cameraFar -= cameraChange;

				enableOrthographic();
			}
			if (!isOrtho)
			{
				cameraZ += cameraChange;
				enablePerspective();
			}
		}
		if (wParam == 'O')
		{
			if (isOrtho)
			{
				cameraNear += cameraChange;
				cameraFar += cameraChange;

				enableOrthographic();
			}
			if (!isOrtho)
			{
				cameraZ -= cameraChange;
				enablePerspective();
			}
		}
		if (wParam == VK_NUMPAD1)
		{
			toggleRobotMovement(form, 0, -2);
		}
		if (wParam == VK_NUMPAD3)
		{
			toggleRobotMovement(form, 0, 2);
		}
		if (wParam == VK_NUMPAD5)
		{
			toggleRobotMovement(form, 1, 2);
		}
		if (wParam == VK_NUMPAD2)
		{
			toggleRobotMovement(form, 1, -2);
		}
		if (wParam == VK_NUMPAD4)
		{
			toggleRobotMovement(form, 2, 2);
		}
		if (wParam == VK_NUMPAD6)
		{
			toggleRobotMovement(form, 2, -2);
		}
		if (wParam == VK_UP)
		{
			if (jetPackSwitch)
			{
				flyTowards(1);
			}
		}
		if (wParam == VK_DOWN)
		{
			if (jetPackSwitch)
			{
				flyAway(1);
			}
		}
		if (wParam == VK_LEFT)
		{
			if (jetPackSwitch)
			{
				flyAway(0);
			}
		}
		if (wParam == VK_RIGHT)
		{
			if (jetPackSwitch)
			{
				flyTowards(0);
			}
		}
		if (wParam == VK_NUMPAD7)
		{
			if (jetPackSwitch)
			{
				flyAway(2);
			}
		}
		if (wParam == VK_NUMPAD9)
		{
			if (jetPackSwitch)
			{
				flyTowards(2);
			}
		}


		if (wParam == 'M') // lower left leg movement
		{
			form = 1;
		}
		if (wParam == 'N') // lower right leg movement
		{
			form = 2;
		}
		if (wParam == 'B') // left leg movement
		{
			form = 3;
		}
		if (wParam == 'V') // right leg movement
		{
			form = 4;
		}
		if (wParam == 'C') // head movement
		{
			form = 5;
		}
		if (wParam == 'X') // left arm
		{
			form = 6;
		}
		if (wParam == 'Z') // right arm
		{
			form = 7;
		}
		if (wParam == 'H') // lower left arm
		{
			form = 8;
		}
		if (wParam == 'G') // lower right arm
		{
			form = 9;
		}
		if (wParam == 'F') // gun shoot faster
		{
			gunSwitch = true;
			rateOfChange += 0.01f;
		}
		if (wParam == 'T') // jetpack launching
		{
			jetPackSwitch = true;
		}if (wParam == 'R') // jetpack stop
		{
			jetPackSwitch = false;
		}
		break;

	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
//--------------------------------------------------------------------

bool initPixelFormat(HDC hdc)
{
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.cAlphaBits = 8;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 0;

	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;

	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;

	// choose pixel format returns the number most similar pixel format available
	int n = ChoosePixelFormat(hdc, &pfd);

	// set pixel format returns whether it sucessfully set the pixel format
	if (SetPixelFormat(hdc, n, &pfd))
	{
		return true;
	}
	else
	{
		return false;
	}
}
//--------------------------------------------------------------------

void buildRobot() {
	glPushMatrix();
	{
		glTranslatef(modelTranslation[0], modelTranslation[1], modelTranslation[2]);
		glRotatef(rotationAngle[0], x, 0, 0);
		glRotatef(rotationAngle[1], 0, y, 0);
		glRotatef(rotationAngle[2], 0, 0, z);
		glRotatef(-180, 0, 1, 0);

		glPushMatrix();
		{
			// Jetpack Animation
			glPushMatrix();
			{
				if (jetPackSwitch == true) {
					if (jetPackExplosionSize < 0.07f) {
						jetPackExplosionSize += 0.00005f;
						jetpackVal += 0.0005f;

						for (int i = 0; i < 3; i++)
						{
							if (jetpackVal > 1) jetpackVal = 1;
							jetpackAmbient[i] = jetpackVal;
							jetpackDiffuse[i] = jetpackVal;
						}
					}
					else {
						jetPackExplosionROC += 10;
						if (jetPackAnimationForm == 0) {
							if (jetPackExplosionLength < 0.5f) {
								jetPackExplosionLength += 0.01f;
							}
							else {
								jetPackAnimationForm = 1;
							}
						}
						else if (jetPackAnimationForm == 1) {
							if (jetPackExplosionLength >= 0.35f) {
								jetPackExplosionLength -= 0.001f;
							}
							else {
								jetPackAnimationForm = 0;
							}
						}
					}
				}
				else {
					jetPackExplosionROC += 10;
					if (jetPackExplosionLength > 0.0f) {
						jetPackExplosionLength -= 0.0002f;
					}
					else {
						if (jetPackExplosionSize > 0.0f) {
							jetPackExplosionSize -= 0.0001f;
							jetpackVal -= 0.001f;

							for (int i = 0; i < 3; i++)
							{
								jetpackAmbient[i] = jetpackVal;
								jetpackDiffuse[i] = jetpackVal;
							}
						}
					}
				}
				if (jetPackExplosionSize > 0) {
					glEnable(GL_LIGHT2);
				}
				else {
					glDisable(GL_LIGHT2);
				}
				glDisable(GL_LIGHTING);
				glBindTexture(GL_TEXTURE_2D, explosionTexture);
				glTranslatef(0.005f, 0.222f, 0.364f);
				gluSphere(sphere, jetPackExplosionSize, 50, 50);
				glRotatef(15, 0, 1, 0);
				glRotatef(60, 1, 0, 0);
				glPushMatrix();
				{
					glRotatef(jetPackExplosionROC, 0, 0, 1);
					gluCylinder(cylinder, jetPackExplosionSize, 0, jetPackExplosionLength, 50, 50);
				}
				glPopMatrix();
				glTranslatef(0.235f, 0, -0.00f);
				gluSphere(sphere, jetPackExplosionSize, 50, 50);
				glPushMatrix();
				{
					glRotatef(jetPackExplosionROC, 0, 0, 1);
					gluCylinder(cylinder, jetPackExplosionSize, 0, jetPackExplosionLength, 50, 50);
				}
				glPopMatrix();
				glEnable(GL_LIGHTING);
			}
			glPopMatrix();

			// Head
			glPushMatrix();
			{
				glBindTexture(GL_TEXTURE_2D, robotInteriorTexture);
				drawNeck();
				glPushMatrix();
				{
					glRotatef(headRotationSpeed[0], 0, 1, 0);
					glTranslatef(0, 0.7f, 0);
					glRotatef(headRotationSpeed[1], 1, 0, 0);
					glTranslatef(0, -0.7f, 0);
					applyMaterial(shinyAmbient, shinyDiffuse, shinySpecular, shinyShininess);
					glBindTexture(GL_TEXTURE_2D, robotArmorTexture);
					drawHelmet();
					glBindTexture(GL_TEXTURE_2D, robotEyeTexture);
					drawEyes();
					applyMaterial(shinyAmbient, shinyDiffuse, shinySpecular, defaultShininess);
					glBindTexture(GL_TEXTURE_2D, robotInteriorTexture);
					drawHead();
				}
				glPopMatrix();
			}
			glPopMatrix();


			// Left Arm
			glPushMatrix();
			{
				glTranslatef(0.5f, 0.6f, -0.2f);
				glRotatef(leftArmRotationSpeed[2], 0, 1, 0);
				glRotatef(leftArmRotationSpeed[1], 1, 0, 0);
				glRotatef(leftArmRotationSpeed[0] + 5, 0, 0, 1);
				glRotatef(-6, 0, 0, 1);
				glTranslatef(-0.5f, -0.6f, 0.2f);
				glBindTexture(GL_TEXTURE_2D, robotInteriorTexture);
				drawLeftUpperArmExtension();
				applyMaterial(shinyAmbient, shinyDiffuse, shinySpecular, shinyShininess);
				glBindTexture(GL_TEXTURE_2D, robotArmorTexture);
				drawLeftUpperArm();

				glPushMatrix();
				{
					glTranslatef(0.56f, 0.2f, -0.3f);
					glRotatef(lowerLeftArmRotationSpeed[0], 0, 0, 1);
					glRotatef(lowerLeftArmRotationSpeed[2], 0, 1, 0);
					glRotatef(lowerLeftArmRotationSpeed[1], 1, 0, 0);
					glTranslatef(-0.56f, -0.2f, 0.3f);
					applyMaterial(shinyAmbient, shinyDiffuse, shinySpecular, shinyShininess);
					glBindTexture(GL_TEXTURE_2D, robotArmorTexture);
					drawLeftForearmArmor();
					applyMaterial(shinyAmbient, shinyDiffuse, shinySpecular, defaultShininess);
					glBindTexture(GL_TEXTURE_2D, robotInteriorTexture);
					drawLeftForearmExtension();

					glPushMatrix();
					{
						applyMaterial(shinyAmbient, shinyDiffuse, shinySpecular, shinyShininess);
						glBindTexture(GL_TEXTURE_2D, robotArmorTexture);
						drawLeftHand();

						// Shotgun
						glPushMatrix();
						{
							glTranslatef(0.40f, -0.35f, -0.35f);
							if (gunSwitch == true) {
								glPushMatrix(); // gun animation
								{
									glDisable(GL_LIGHTING);
									glTranslatef(0.04f, -0.65f, -0.08f);
									bulletSpeed += rateOfChange;
									if (bulletSpeed >= 3.0f) {
										bulletSpeed = 0.0f;
										rateOfChange = 0.0f;
										gunSwitch = false;
									}
									if (bulletSpeed <= 1.0f && bulletSpeed > 0.0f) {
										glEnable(GL_LIGHT1);
										explosionX = 3;
										explosionY = 3;
										explosionZ = 3;
										glPushMatrix();
										{
											glScalef(explosionX, explosionY, explosionZ);
											glBindTexture(GL_TEXTURE_2D, explosionTexture);
											gluSphere(sphere, 0.025f, 50, 50);
										}
										glPopMatrix();
										glEnable(GL_LIGHTING);
										applyMaterial(shinyAmbient, shinyDiffuse, shinySpecular, shinyShininess);
										glBindTexture(GL_TEXTURE_2D, grayTexture);
										glPushMatrix();
										{
											glTranslatef(0, -bulletSpeed, 0);
											gluSphere(sphere, 0.025f, 50, 50);
										}
										glPopMatrix();
										glPushMatrix();
										{
											glRotatef(15, 1, 1, 0);
											glTranslatef(0, -bulletSpeed, 0);
											gluSphere(sphere, 0.025f, 50, 50);
										}
										glPopMatrix();
										glPushMatrix();
										{
											glRotatef(-15, 1, 1, 0);
											glTranslatef(0, -bulletSpeed, 0);
											gluSphere(sphere, 0.025f, 50, 50);
										}
										glPopMatrix();
										glPushMatrix();
										{
											glRotatef(-15, 0, 1, 1);
											glTranslatef(0, -bulletSpeed, 0);
											gluSphere(sphere, 0.025f, 50, 50);
										}
										glPopMatrix();
										glPushMatrix();
										{
											glRotatef(15, 0, 1, 1);
											glTranslatef(0, -bulletSpeed, 0);
											gluSphere(sphere, 0.025f, 50, 50);
										}
										glPopMatrix();
										glPushMatrix();
										{
											glRotatef(-15, 1, 0, 1);
											glTranslatef(0, -bulletSpeed, 0);
											gluSphere(sphere, 0.025f, 50, 50);
										}
										glPopMatrix();
										glPushMatrix();
										{
											glRotatef(15, 1, 0, 1);
											glTranslatef(0, -bulletSpeed, 0);
											gluSphere(sphere, 0.025f, 50, 50);
										}
										glPopMatrix();
										glPushMatrix();
										{
											glRotatef(-15, 1, 1, 1);
											glTranslatef(0, -bulletSpeed, 0);
											gluSphere(sphere, 0.025f, 50, 50);
										}
										glPopMatrix();
										glPushMatrix();
										{
											glRotatef(15, 1, 1, 1);
											glTranslatef(0, -bulletSpeed, 0);
											gluSphere(sphere, 0.025f, 50, 50);
										}
										glPopMatrix();
									}
									else {
										glEnable(GL_LIGHTING);
										glDisable(GL_LIGHT1);
										explosionX = 1;
										explosionY = 1;
										explosionZ = 1;
										applyMaterial(shinyAmbient, shinyDiffuse, shinySpecular, shinyShininess);
										glBindTexture(GL_TEXTURE_2D, grayTexture);
										glPushMatrix();
										{
											glTranslatef(0, -bulletSpeed, 0);
											gluSphere(sphere, 0.025f, 50, 50);
										}
										glPopMatrix();
										glPushMatrix();
										{
											glRotatef(15, 1, 1, 0);
											glTranslatef(0, -bulletSpeed, 0);
											gluSphere(sphere, 0.025f, 50, 50);
										}
										glPopMatrix();
										glPushMatrix();
										{
											glRotatef(-15, 1, 1, 0);
											glTranslatef(0, -bulletSpeed, 0);
											gluSphere(sphere, 0.025f, 50, 50);
										}
										glPopMatrix();
										glPushMatrix();
										{
											glRotatef(-15, 0, 1, 1);
											glTranslatef(0, -bulletSpeed, 0);
											gluSphere(sphere, 0.025f, 50, 50);
										}
										glPopMatrix();
										glPushMatrix();
										{
											glRotatef(15, 0, 1, 1);
											glTranslatef(0, -bulletSpeed, 0);
											gluSphere(sphere, 0.025f, 50, 50);
										}
										glPopMatrix();
										glPushMatrix();
										{
											glRotatef(-15, 1, 0, 1);
											glTranslatef(0, -bulletSpeed, 0);
											gluSphere(sphere, 0.025f, 50, 50);
										}
										glPopMatrix();
										glPushMatrix();
										{
											glRotatef(15, 1, 0, 1);
											glTranslatef(0, -bulletSpeed, 0);
											gluSphere(sphere, 0.025f, 50, 50);
										}
										glPopMatrix();
										glPushMatrix();
										{
											glRotatef(-15, 1, 1, 1);
											glTranslatef(0, -bulletSpeed, 0);
											gluSphere(sphere, 0.025f, 50, 50);
										}
										glPopMatrix();
										glPushMatrix();
										{
											glRotatef(15, 1, 1, 1);
											glTranslatef(0, -bulletSpeed, 0);
											gluSphere(sphere, 0.025f, 50, 50);
										}
										glPopMatrix();
									}
									glEnable(GL_LIGHTING);

								}
								glPopMatrix();
							}
							glRotatef(90, 0, 1, 0);
							glRotatef(-90, 0, 0, 1);
							glBindTexture(GL_TEXTURE_2D, gunTexture);
							drawHandle();
							drawGunBarrel();
						}
						glPopMatrix();
					}
					glPopMatrix();
				}
				glPopMatrix();
			}
			glPopMatrix();


			// Right Arm
			glPushMatrix();
			{
				glTranslatef(-0.45f, 0.60f, 0.2f);
				glRotatef(rightArmRotationSpeed[2] + 30, 0, 1, 0);
				glRotatef(rightArmRotationSpeed[1], 1, 0, 0);
				glRotatef(rightArmRotationSpeed[0], 0, 0, 1);
				glTranslatef(0.45f, -0.60f, -0.2f);
				applyMaterial(shinyAmbient, shinyDiffuse, shinySpecular, shinyShininess);
				glBindTexture(GL_TEXTURE_2D, robotArmorTexture);
				drawRightUpperArmExtension();
				applyMaterial(shinyAmbient, shinyDiffuse, shinySpecular, defaultShininess);
				glBindTexture(GL_TEXTURE_2D, robotInteriorTexture);
				drawRightUpperArm();

				glPushMatrix();
				{
					glTranslatef(-0.60f, 0.2f, 0.2f);
					glRotatef(lowerRightArmRotationSpeed[0], 0, 0, 1);
					glRotatef(lowerRightArmRotationSpeed[2], 0, 1, 0);
					glRotatef(lowerRightArmRotationSpeed[1], 1, 0, 0);

					glTranslatef(0.60f, -0.2f, -0.2f);
					applyMaterial(shinyAmbient, shinyDiffuse, shinySpecular, shinyShininess);
					glBindTexture(GL_TEXTURE_2D, robotArmorTexture);
					drawRightForearmArmor();
					applyMaterial(shinyAmbient, shinyDiffuse, shinySpecular, defaultShininess);
					glBindTexture(GL_TEXTURE_2D, robotInteriorTexture);
					drawRightForearmExtension();

					glPushMatrix();
					{
						applyMaterial(shinyAmbient, shinyDiffuse, shinySpecular, shinyShininess);
						glBindTexture(GL_TEXTURE_2D, robotArmorTexture);
						drawRightHand();

						// Shield
						glPushMatrix();
						{
							glTranslatef(-0.75f, -0.4f, -0.1f);
							glRotatef(-30, 0, 0, 1);
							glRotatef(-90, 1, 0, 0);
							applyMaterial(shinyAmbient, shinyDiffuse, shinySpecular, shinyShininess);
							glBindTexture(GL_TEXTURE_2D, shieldSpikesTexture);
							drawShieldSpikes();
							glBindTexture(GL_TEXTURE_2D, silverTexture);
							drawShieldBoard();
						}
						glPopMatrix();
					}
					glPopMatrix();
				}
				glPopMatrix();
			}
			glPopMatrix();


			// Torso
			glPushMatrix();
			{
				applyMaterial(shinyAmbient, shinyDiffuse, shinySpecular, defaultShininess);
				glBindTexture(GL_TEXTURE_2D, robotInteriorTexture);
				drawTorsoWithJetpackExhaust();
				glBindTexture(GL_TEXTURE_2D, blackTexture);
				drawJetpackExhaustHole();
				applyMaterial(shinyAmbient, shinyDiffuse, shinySpecular, shinyShininess);
				glBindTexture(GL_TEXTURE_2D, robotAttireTexture);
				drawTorsoCover();
				glBindTexture(GL_TEXTURE_2D, robotArmorTexture);
				drawJetpackWithPipes();
				glBindTexture(GL_TEXTURE_2D, robotAttireTexture);
				drawLowerTorsoArmorFront();
				drawLowerTorsoArmorBack();
				applyMaterial(shinyAmbient, shinyDiffuse, shinySpecular, defaultShininess);
				glBindTexture(GL_TEXTURE_2D, robotInteriorTexture);
				drawLeftArmConnector();
				drawRightArmConnector();
			}
			glPopMatrix();


			// Left Leg
			glPushMatrix();
			{
				glRotatef(leftLegRotationSpeed[1], 1, 0, 0);
				glRotatef(leftLegRotationSpeed[2], 0, 0, 1);
				glRotatef(leftLegRotationSpeed[0], 0, 1, 0);
				applyMaterial(shinyAmbient, shinyDiffuse, shinySpecular, shinyShininess);
				glBindTexture(GL_TEXTURE_2D, robotArmorTexture);
				drawLeftThigh();
				glTranslatef(0, -0.50f, -0.1f);
				glPushMatrix();
				{
					glRotatef(6, 0, 0, 1);
					glRotatef(leftLowerLegRotationSpeed, 1, 0, 0);
					glTranslatef(0, 0.50f, 0.1f);
					applyMaterial(shinyAmbient, shinyDiffuse, shinySpecular, defaultShininess);
					glBindTexture(GL_TEXTURE_2D, robotInteriorTexture);
					drawLeftFeet();
					applyMaterial(shinyAmbient, shinyDiffuse, shinySpecular, shinyShininess);
					glBindTexture(GL_TEXTURE_2D, robotArmorTexture);
					drawLeftLeg();
					applyMaterial(shinyAmbient, shinyDiffuse, shinySpecular, defaultShininess);
					glBindTexture(GL_TEXTURE_2D, robotInteriorTexture);
					drawLeftKnee();
					applyMaterial(shinyAmbient, shinyDiffuse, shinySpecular, shinyShininess);
					glBindTexture(GL_TEXTURE_2D, robotArmorTexture);
					drawLeftKneeArmor();
					drawLeftLegPipe();
				}
				glPopMatrix();
			}
			glPopMatrix();


			// Right Leg
			glPushMatrix();
			{
				glRotatef(rightLegRotationSpeed[1], 1, 0, -0.5);
				glRotatef(rightLegRotationSpeed[2], 0, 0, 1);
				glRotatef(rightLegRotationSpeed[0], 0, 1, 0);
				applyMaterial(shinyAmbient, shinyDiffuse, shinySpecular, shinyShininess);
				glBindTexture(GL_TEXTURE_2D, robotArmorTexture);
				drawRightThigh();
				glTranslatef(0, -0.55f, -0.1f);
				glPushMatrix();
				{
					glRotatef(rightLowerLegRotationSpeed, 1, 0, -0.5);
					glTranslatef(0, 0.55f, 0.1f);
					applyMaterial(shinyAmbient, shinyDiffuse, shinySpecular, defaultShininess);
					glBindTexture(GL_TEXTURE_2D, robotInteriorTexture);
					drawRightFeet();
					applyMaterial(shinyAmbient, shinyDiffuse, shinySpecular, shinyShininess);
					glBindTexture(GL_TEXTURE_2D, robotArmorTexture);
					drawRightLeg();
					applyMaterial(shinyAmbient, shinyDiffuse, shinySpecular, defaultShininess);
					glBindTexture(GL_TEXTURE_2D, robotInteriorTexture);
					drawRightKnee();
					applyMaterial(shinyAmbient, shinyDiffuse, shinySpecular, shinyShininess);
					glBindTexture(GL_TEXTURE_2D, robotArmorTexture);
					drawRightKneeArmor();
					drawRightLegPipe();
				}
				glPopMatrix();
			}
			glPopMatrix();
		}
		glPopMatrix();
	}
	glPopMatrix();
}
void Display()
{
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT_AND_DIFFUSE, lightAmbient);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT1, GL_AMBIENT, gunAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, gunDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, gunLightPosition);
	glLightfv(GL_LIGHT2, GL_AMBIENT, jetpackAmbient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, jetpackDiffuse);
	glLightfv(GL_LIGHT2, GL_POSITION, jetpackLightPosition);


	gluQuadricTexture(sphere, TRUE);
	gluQuadricTexture(cylinder, TRUE);
	if (lightsOn)
	{
		glEnable(GL_LIGHTING);
	}
	else
	{
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	{
		glDisable(GL_LIGHTING);
		glTranslatef(lightPosition[0], lightPosition[1], lightPosition[2]);
		glRotatef(sunRotation += 0.1, 0, 1, 0);
		glBindTexture(GL_TEXTURE_2D, sunTexture);
		gluQuadricTexture(sun, sunTexture);
		gluSphere(sun, 0.2, 50, 50);
		glEnable(GL_LIGHTING);

		sunRevolution();
	}
	glPopMatrix();

	// Skybox
	glPushMatrix();
	{
		glDisable(GL_LIGHTING);
		if (isOrtho)
		{
			drawSkyBox(20, 20, 20);
		}
		else
		{
			drawSkyBox(100, 100, 100);
		}
		glEnable(GL_LIGHTING);
	}
	glPopMatrix();

	switch (modelNo) {
	case 1:
		returnToOGPos();
		buildRobot();
		break;
	case 2: // robot flying and blocking with shield
		if (atOrigin)
		{
			jetPackSwitch = true;
			shieldPoseAnimation();
			buildRobot();
		}
		else {
			returnToOGPos();
			buildRobot();
		}
		break;
	case 3: // shooting to side
		if (atOrigin)
		{
			gunPoseAnimation();
			buildRobot();
		}
		else {
			returnToOGPos();
			buildRobot();
		}
		break;
	case 4: // victory pose
		if (atOrigin)
		{
			victoryPoseAnimation();
			buildRobot();
		}
		else {
			returnToOGPos();
			buildRobot();
		}
		break;
	case 5: // Running Animation
		if (atOrigin)
		{
			runningAnimation();
			buildRobot();
		}
		else {
			returnToOGPos();
			buildRobot();
		}
		break;
	case 6:
		buildRobot();
		break;
	}
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow)
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpfnWndProc = WindowProcedure;
	wc.lpszClassName = WINDOW_TITLE;
	wc.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClassEx(&wc)) return false;

	HWND hWnd = CreateWindow(WINDOW_TITLE, "Robot Prototype", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 800,
		NULL, NULL, wc.hInstance, NULL);

	//--------------------------------
	//	Initialize window for OpenGL
	//--------------------------------

	HDC hdc = GetDC(hWnd);

	//	initialize pixel format for the window
	initPixelFormat(hdc);

	//	get an openGL context
	HGLRC hglrc = wglCreateContext(hdc);

	//	make context current
	if (!wglMakeCurrent(hdc, hglrc)) return false;

	glEnable(GL_DEPTH_TEST);
	//--------------------------------
	//	End initialization
	//--------------------------------
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	enableOrthographic();
	// enablePerspective();

	ShowWindow(hWnd, nCmdShow);

	// Initialize Texture
	hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), "assets/texture/blackTexture.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	GetObject(hBMP, sizeof(BMP), &BMP);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &blackTexture);
	glBindTexture(GL_TEXTURE_2D, blackTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);


	hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), "assets/texture/grayTexture.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	GetObject(hBMP, sizeof(BMP), &BMP);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &grayTexture);
	glBindTexture(GL_TEXTURE_2D, grayTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);

	hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), "assets/texture/silverTexture.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	GetObject(hBMP, sizeof(BMP), &BMP);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &silverTexture);
	glBindTexture(GL_TEXTURE_2D, silverTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);

	hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), "assets/texture/shieldSpikesTexture.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	GetObject(hBMP, sizeof(BMP), &BMP);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &shieldSpikesTexture);
	glBindTexture(GL_TEXTURE_2D, shieldSpikesTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);

	hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), "assets/texture/gunTexture.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	GetObject(hBMP, sizeof(BMP), &BMP);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &gunTexture);
	glBindTexture(GL_TEXTURE_2D, gunTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);

	hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), "assets/texture/explosionTexture.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	GetObject(hBMP, sizeof(BMP), &BMP);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &explosionTexture);
	glBindTexture(GL_TEXTURE_2D, explosionTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);

	hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), "assets/texture/sunTexture.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	GetObject(hBMP, sizeof(BMP), &BMP);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &sunTexture);
	glBindTexture(GL_TEXTURE_2D, sunTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);

	// Robot Textures
	hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), "assets/texture/robotEyeTexture.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	GetObject(hBMP, sizeof(BMP), &BMP);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &robotEyeTexture);
	glBindTexture(GL_TEXTURE_2D, robotEyeTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);

	hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), "assets/texture/robotArmorTexture.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	GetObject(hBMP, sizeof(BMP), &BMP);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &robotArmorTexture);
	glBindTexture(GL_TEXTURE_2D, robotArmorTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);

	hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), "assets/texture/robotInteriorTexture.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	GetObject(hBMP, sizeof(BMP), &BMP);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &robotInteriorTexture);
	glBindTexture(GL_TEXTURE_2D, robotInteriorTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);

	hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), "assets/texture/robotAttireTexture.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	GetObject(hBMP, sizeof(BMP), &BMP);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &robotAttireTexture);
	glBindTexture(GL_TEXTURE_2D, robotAttireTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);

	// Skybox
	hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), "assets/skybox/px.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	GetObject(hBMP, sizeof(BMP), &BMP);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &skyRight);
	glBindTexture(GL_TEXTURE_2D, skyRight);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);

	hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), "assets/skybox/nx.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	GetObject(hBMP, sizeof(BMP), &BMP);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &skyLeft);
	glBindTexture(GL_TEXTURE_2D, skyLeft);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);

	hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), "assets/skybox/py.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	GetObject(hBMP, sizeof(BMP), &BMP);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &skyTop);
	glBindTexture(GL_TEXTURE_2D, skyTop);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);

	hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), "assets/skybox/ny.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	GetObject(hBMP, sizeof(BMP), &BMP);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &skyBottom);
	glBindTexture(GL_TEXTURE_2D, skyBottom);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);

	hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), "assets/skybox/pz.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	GetObject(hBMP, sizeof(BMP), &BMP);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &skyFront);
	glBindTexture(GL_TEXTURE_2D, skyFront);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);

	hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), "assets/skybox/nz.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	GetObject(hBMP, sizeof(BMP), &BMP);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &skyBack);
	glBindTexture(GL_TEXTURE_2D, skyBack);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);


	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		Display();

		SwapBuffers(hdc);
	}

	// Cleanup 
	gluDeleteQuadric(sphere);
	gluDeleteQuadric(cylinder);
	gluDeleteQuadric(sun);

	// Texture Cleanup
	glDisable(GL_TEXTURE_2D);
	DeleteObject(hBMP);
	glDeleteTextures(1, &silverTexture);
	glDeleteTextures(1, &blackTexture);
	glDeleteTextures(1, &grayTexture);
	glDeleteTextures(1, &shieldSpikesTexture);
	glDeleteTextures(1, &gunTexture);
	glDeleteTextures(1, &explosionTexture);
	glDeleteTextures(1, &sunTexture);

	glDeleteTextures(1, &robotEyeTexture);
	glDeleteTextures(1, &robotArmorTexture);
	glDeleteTextures(1, &robotInteriorTexture);
	glDeleteTextures(1, &robotAttireTexture);

	glDeleteTextures(1, &skyBack);
	glDeleteTextures(1, &skyFront);
	glDeleteTextures(1, &skyLeft);
	glDeleteTextures(1, &skyRight);
	glDeleteTextures(1, &skyTop);
	glDeleteTextures(1, &skyBottom);
	UnregisterClass(WINDOW_TITLE, wc.hInstance);

	return true;
}
//--------------------------------------------------------------------

