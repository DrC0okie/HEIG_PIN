/*-----------------------------------------------------------------------------
File name : robot.cpp
Author(s) : G. Courbat, J. Streckeisen, T. Van Hove
Creation date : 22.08.2022
Description :  Implementation of the robot class
Compiler : Mingw-w64 g++ 11.2.0
-----------------------------------------------------------------------------*/

#include "robot.h"
#include <QPainterPath>
#include "math.h"
#include "trajectory.h"
#include <iostream>

const QBrush
Robot::background = QBrush(QColor(255, 255, 255));
const QPen Robot::border = QPen(Qt::black);
const double Robot::mouthDiameter = 5;
const QBrush Robot::sectorBackground = QBrush(QColor(155, 155, 155));

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

void Robot::draw(QPaintDevice *device, Position worldOrigin, double ratio) {
	double ratioedRadius = this->radius * ratio;
	double diameter = ratioedRadius * 2;
	double textSize = diameter / 2 * ratio;
	double effectiveAngle = this->captureAngle * 2;
	Position realPosition = Position(
		this->position.getX() - worldOrigin.getX() - this->radius,
		this->position.getY() - worldOrigin.getY() - this->radius);

	QRect boundingRect(realPosition.getX() * ratio, realPosition.getY() * ratio,
							 diameter, diameter);
	QPainter p(device);
	p.setRenderHint(QPainter::Antialiasing, QPainter::SmoothPixmapTransform);
	QPainterPath path;
	QFont textFont;
	QPoint center = boundingRect.center();
	QTransform trans = QTransform().translate(center.x(), center.y()).rotate(
		this->angle).translate(-center.x(), -center.y());;
	textFont.setPixelSize(textSize);
	textFont.setBold(true);
	QRect fontSize = QFontMetrics(textFont).tightBoundingRect(
		QString::number(this->id));

	p.setBrush(this->background);
	p.setPen(this->border);
	p.setFont(textFont);
	p.save();
	p.setTransform(trans);

//Main circle
	p.drawEllipse(boundingRect);

//Circle sector
	path.moveTo(realPosition.getX() * ratio + ratioedRadius,
					realPosition.getY() * ratio + ratioedRadius);
	path.arcTo(boundingRect, -effectiveAngle / 2, effectiveAngle);
	path.closeSubpath();
	p.setBrush(this->sectorBackground);
	p.drawPath(path);

//Mouth
	p.setBrush(this->background);
	p.drawEllipse(realPosition.getX() * ratio + (diameter / 4 * 3),
					  realPosition.getY() * ratio + ratioedRadius - mouthDiameter / 2,
					  mouthDiameter, mouthDiameter);
	p.restore();

//Text
	p.drawText(QPoint(realPosition.getX() * ratio + ratioedRadius -
							fontSize.width() / 2,
							realPosition.getY() * ratio + ratioedRadius +
							fontSize.height() / 2), QString::number(this->id));
}
