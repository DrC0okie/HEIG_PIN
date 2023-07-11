#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include "position.h"
#include "robot.h"

enum AngularDirection {
	CW, CCW
};

double getAngle(Position p1, Position p2);

Position updateLinCoordinate(Position pos, double speed, double angle, double
deltaTime);

void updateCircCoordinate(Robot &robot, double time);

double toRad(double deg);

double toDeg(double rad);

double angularSpeed(double radius, double leftSpeed, double rightSpeed);

double updateAngle(double angle, double radius, double leftSpeed, double rightSpeed,
						 double deltaTime);

#endif
