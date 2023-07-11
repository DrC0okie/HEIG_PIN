/*-----------------------------------------------------------------------------
File name : robot.h
Author(s) : G. Courbat, J. Streckeisen, T. Van Hove
Creation date : 22.08.2022
Description :  Header of the class handling the Robot objects
Compiler : Mingw-w64 g++ 11.2.0
-----------------------------------------------------------------------------*/

#ifndef ROBOT_H
#define ROBOT_H

#include "position.h"

using json = nlohmann::json;

enum AngleUnit {
	DEG, RAD
};

class Robot {

public:
	Robot() : id(0), position(Position()), radius(0), angle(0), captureAngle(0),
				 leftSpeed(0), rightSpeed(0), score(0) {}

	Robot(int id, Position position, double radius, double angle, double
	captureAngle, double leftSpeed, double rightSpeed);

	double getAngle(AngleUnit unit = RAD) const;

	Position getPosition() const { return position; }

	double getRadius() const { return radius; }

	void setSpeed(double _leftSpeed, double _rightSpeed) {
		rightSpeed = _rightSpeed;
		leftSpeed = _leftSpeed;
	}


	double getRightSpeed() const { return rightSpeed; }

	double getLeftSpeed() const { return leftSpeed; }

	int getId() const { return id; }

    int getTargetParticleId() const {return targetParticleId;}

    double getScore(){return score;}

    void setTargetParticleId(int _id){targetParticleId = _id;}

	void setPosition(Position pos) { position = pos; }

	void setAngle(double _angle) { angle = _angle; }

	void setBothSpeed(double speed) { setSpeed(speed, speed); }

    void setScore(double newScore){ score = newScore;}
private:
    int targetParticleId = -1;
	int id;              // unique identifier
	Position position;   // (x,y) coordinates of the robot in the world (in pixels)
	double radius;       // radius of the circular robot. (pixels)
	double angle;        // direction in which the robot moves. [0,360[ in degrees. 0 = 3 o'clock, 90 = noon
	double captureAngle; // c.f. décontamination
	double leftSpeed;    // linear speed of the leftmost part of the robot in pixels/sec
	double rightSpeed;   // linear speed of the rightmost part of the robot in pixels/sec
	double score;        // c.f. décontamination

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Robot, id, position, radius, angle,
											 captureAngle, leftSpeed, rightSpeed, score)

};

#endif // ROBOT_H
