#include "utils.h"
#include "trajectory.h"

const int FRAME_PER_SEC = 24;
const double TIME_PER_FRAME = 1.0 / FRAME_PER_SEC;

double getArea(double radius){
    return M_PI * radius * radius;
}

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

double deltaAngle(double originAngle, double targetAngle) {
	double result;
	if (targetAngle >= originAngle) {
		if (targetAngle - originAngle < toRad(180))
			result = targetAngle - originAngle;
		else result = toRad(360) - targetAngle + originAngle;
	} else {
		if (originAngle - targetAngle < toRad(180))
			result = originAngle - targetAngle;
		else result = toRad(360) - originAngle + targetAngle;
	}
	return result;
}

AngularDirection rotateShortestPath(double originAngle, double targetAngle) {
	if (targetAngle >= originAngle) {
		if (targetAngle - originAngle < toRad(180))
			return CW;
		else return CCW;
	} else {
		if (originAngle - targetAngle < toRad(180))
			return CCW;
		else return CW;
	}
}

Position updateCoordinate(Position pos, double speed, double angle, double
deltaTime) {
	Position targetPos(0, 0);
	//Calculate the target position
	double distance = speed * deltaTime;
	targetPos.setX(pos.getX() + distance * cos(angle));
	//We had to inverse Y because in QT the plan is inverted for Y axis
	targetPos.setY(pos.getY() + distance * sin(angle));
	return targetPos;
}
Position front2xRobot(double r, double angle){
	double x,y;
	x = 3. * r * cos(angle) ;
	y = 3. * r * sin(angle) ;
	return {x,y};
}

double linearDistance(Position p1, Position p2){
    double deltaX = abs(p1.getX() - p2.getX());
    double deltaY = abs(p1.getY() - p2.getY());
    return abs(sqrt((deltaX * deltaX) + (deltaY * deltaY)));
}

double getSyncTime(double time, const double refreshRate) {
	double remainder = fmod(time, refreshRate);
	double syncTime = refreshRate - remainder;
   //One tick before command interval
	return time + syncTime - TIME_PER_FRAME;
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

double rotationTime(double angle, double angularSpeed) {
	return abs(angle / angularSpeed);
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

bool detectCollision(Position p1, double radius1, Position p2, double radius2,
							double epsilon) {
	double deltaX = abs(p1.getX() - p2.getX());
	double deltaY = abs(p1.getY() - p2.getY());
	double distance = sqrt(deltaX * deltaX + deltaY * deltaY);
	//TODO Need to add the epsilon error margin
	return distance <= radius1 + radius2 + epsilon;
}
