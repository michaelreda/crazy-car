#include "Car.h"
#include "Model_3DS.h"
#include "Enums.h"
#include <glut.h>

Car::Car() {};
Car::Car(Model_3DS carModel, Model_3DS wheelModel, float bodyScale, float wheelScale):carModel(carModel),wheelModel(wheelModel)
{
	this->bodyScale = bodyScale;
	this->wheelScale = wheelScale;
	this->wheelRotation = 0.0f;
	this->direction = CENTER;
	scaleModel();
}

void Car::scaleModel()
{
	this->carModel.scale = bodyScale;
	this->wheelModel.scale = wheelScale;
}
void Car::setCarDirection(CAR_DIRECTION direction)
{
	this->direction = direction;
}

CAR_DIRECTION Car::getCarDirection()
{
	return this->direction;
}
float Car::getWheelRotation()
{
	return this->wheelRotation;
}
void Car::setWheelRotation(float rotation)
{
	this->wheelRotation = rotation;
}
void Car::drawCar()
{
	glPushMatrix();
		glPushMatrix();
			this->carModel.Draw();
		glPopMatrix();
		glPushMatrix();//front left wheel
			glTranslatef(0.1f, 1.2f, 4.2f);
			if(this->direction == LEFT)
				glRotatef(30.0f, 0.0f, 1.0f, 0.0f);
			if (this->direction == RIGHT)
				glRotatef(-30.0f, 0.0f, 1.0f, 0.0f);
			glRotatef(this->wheelRotation, 1.0f, 0.0f, 0.0f);
			this->wheelModel.Draw();
		glPopMatrix();
		glPushMatrix();//back left wheel
			glTranslatef(0.1f, 1.2f, -3.8f);
			glRotatef(this->wheelRotation, 1.0f, 0.0f, 0.0f);
			this->wheelModel.Draw();
		glPopMatrix();
		glPushMatrix();//back right wheel
			glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
			glTranslatef(4.3f, 1.2f, 4.0f);
			glRotatef(this->wheelRotation, -1.0f, 0.0f, 0.0f);
			this->wheelModel.Draw();
		glPopMatrix();
		glPushMatrix();//front right wheel
			glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
			glTranslatef(4.2f, 1.2f, -4.0f);
			if (this->direction == LEFT)
				glRotatef(30.0f, 0.0f, 1.0f, 0.0f);
			if (this->direction == RIGHT)
				glRotatef(-30.0f, 0.0f, 1.0f, 0.0f);
			glRotatef(this->wheelRotation, -1.0f, 0.0f, 0.0f);
			this->wheelModel.Draw();
		glPopMatrix();
	glPopMatrix();
}
Car::~Car(){}
