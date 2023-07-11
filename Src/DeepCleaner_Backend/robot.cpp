/*-----------------------------------------------------------------------------
File name : robot.cpp
Author(s) : G. Courbat, J. Streckeisen, T. Van Hove
Creation date : 22.08.2022
Description :  Implementation of the robot class
Compiler : Mingw-w64 g++ 11.2.0
-----------------------------------------------------------------------------*/

#include "robot.h"
#include "trajectory.h"
#include <fstream>

Robot::Robot(int id, Position position, double radius, double angle,
				 double captureAngle, double leftSpeed, double rightSpeed) {
	this->id = id;
	this->position = position;
	this->radius = radius;
	this->angle = angle;
	this->captureAngle = captureAngle;
	this->leftSpeed = leftSpeed;
	this->rightSpeed = rightSpeed;
	this->score = 0;
}

double Robot::getAngle(AngleUnit unit) const {
	if (unit == RAD)
		return toRad(this->angle);
	return this->angle;
}