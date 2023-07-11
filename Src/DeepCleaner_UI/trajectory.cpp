#include "utils.h"
#include "trajectory.h"

double getAngle(Position p1, Position p2) {
	double deltaY = abs(p1.getY() - p2.getY());
	double deltaX = abs(p1.getX() - p2.getX());
	double targetAngle = atan(deltaY / deltaX);

	if (p2.getX() >= p1.getX() && p2.getY() >= p1.getY()) {
		return targetAngle;
	} else if (p2.getX() < p1.getX() && p2.getY() > p1.getY()) {
		return toRad(180) - targetAngle;
	} else if (p2.getX() <= p1.getX() && p2.getY() <= p1.getY()) {
		return toRad(180) + targetAngle;
	}
	return toRad(360) - targetAngle;
}


Position updateLinCoordinate(Position pos, double speed, double angle, double
deltaTime) {
	Position targetPos(0, 0);
	//Calculate the target position
	double distance = speed * deltaTime;
	targetPos.setX(pos.getX() + distance * cos(angle));
	//We had to inverse Y because in QT the plan is inverted for Y axis
	targetPos.setY(pos.getY() + distance * sin(angle));
	return targetPos;
}

void updateCircCoordinate(Robot &robot, double time) {

	//calcule  R
	double bigR = robot.getRadius() *
					  ((robot.getLeftSpeed() + robot.getRightSpeed()) /
						(robot.getLeftSpeed() - robot.getRightSpeed()));
	//calcule rotation omega
	double omega = angularSpeed(robot.getRadius(), robot.getLeftSpeed(),
										 robot.getRightSpeed());
	//calcule nouvel angle robot
	double angle = updateAngle(robot.getAngle(), robot.getRadius(),
										robot.getLeftSpeed(), robot.getRightSpeed(), time);

	Position vtVec = {bigR * cos(robot.getAngle()), bigR * sin(robot.getAngle())};

	Position vrVec = {bigR * (-sin(robot.getAngle())), bigR * cos(robot.getAngle())};

	Position teleport = {
		(vtVec.getX() * sin(omega * time)) + (vrVec.getX() * (1 - cos(omega * time))) +
		robot.getPosition().getX(),
		(vtVec.getY() * sin(omega * time)) + (vrVec.getY() * (1 - cos(omega * time))) +
		robot.getPosition().getY()};
	robot.setPosition(teleport);
	robot.setAngle(toDeg(angle));
}

double updateAngle(double angle, double radius, double leftSpeed, double rightSpeed,
						 double deltaTime) {
	double omega = angularSpeed(radius, leftSpeed, rightSpeed);
	double temp = angle + omega * deltaTime;
	if (toDeg(temp) < 0) {
		return toRad(360) + temp;
	} else if (toDeg(temp) > 360) {
		return temp - toRad(360);
	}
	return temp;
}


double toRad(double deg) {
	return deg * M_PI / 180;
}

double toDeg(double rad) {
	return rad * 180 / M_PI;
}

double angularSpeed(double radius, double leftSpeed, double rightSpeed) {
	return (leftSpeed - rightSpeed) / (2 * radius);
}
