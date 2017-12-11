#pragma once
#include "Model_3DS.h"
#include "Enums.h"

class Car
{
	private:
		float bodyScale;
		float wheelScale;
		Model_3DS carModel;
		Model_3DS wheelModel;
		CAR_DIRECTION direction;
		float wheelRotation;
		void scaleModel();
	public:
		Car();
		Car(Model_3DS,Model_3DS,float,float);
		void setCarDirection(CAR_DIRECTION);
		CAR_DIRECTION getCarDirection();
		float getWheelRotation();
		void setWheelRotation(float);
		void drawCar();
		~Car();
};

