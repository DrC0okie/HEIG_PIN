#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include "position.h"

enum AngularDirection {
	CW, CCW
};

double getAngle(Position p1, Position p2);

AngularDirection rotateShortestPath(double originAngle, double targetAngle);

Position updateCoordinate(Position pos, double speed, double angle, double
deltaTime);

double toRad(double deg);

double toDeg(double rad);

Position front2xRobot(double r, double angle);

double getArea(double radius);

double angularSpeed(double radius, double leftSpeed, double rightSpeed);

double deltaAngle(double originAngle, double targetAngle);

double linearDistance(Position p1, Position p2);

double getSyncTime(double time, const double refreshRate);

double rotationTime(double angle, double angularSpeed);

double updateAngle(double angle, double radius, double leftSpeed, double rightSpeed,
						 double deltaTime);

bool detectCollision(Position p1, double radius1, Position p2, double radius2,
							double epsilon);

#endif
