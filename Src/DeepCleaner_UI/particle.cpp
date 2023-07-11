/*-----------------------------------------------------------------------------
File name : particle.cpp
Author(s) : G. Courbat, J. Streckeisen, T. Van Hove
Creation date : 22.08.2022
Description :  Implementation of the particle class
Compiler : Mingw-w64 g++ 11.2.0
-----------------------------------------------------------------------------*/

#include "particle.h"
#include <iostream>

const QBrush Particle::background = QBrush(QColor(227, 68, 68));
const QPen Particle::border = QPen(Qt::black);

Particle::Particle(int id, Position position, double radius,
						 std::vector<std::vector<double>> explosionTimes) {
	this->id = id;
	this->position = position;
	this->radius = radius;
	this->explosionTimes = explosionTimes;
}

void Particle::draw(QPaintDevice *device, Position worldOrigin, double ratio) {
	QPainter p(device);
	p.setRenderHint(QPainter::Antialiasing, QPainter::SmoothPixmapTransform);
	double ratioedRadius = this->radius * ratio;
	double diameter = ratioedRadius * 2;
	double textSize = diameter / 2.5 * ratio;

	Position realPosition = Position(
		this->position.getX() - worldOrigin.getX() - this->radius,
		this->position.getY() - worldOrigin.getY() - this->radius);

	QRect boundingRect(realPosition.getX() * ratio, realPosition.getY() * ratio,
							 diameter, diameter);
	QFont textFont;
	textFont.setPixelSize(textSize);
	textFont.setBold(true);
	p.setFont(textFont);

	QRect fontSize = QFontMetrics(textFont).tightBoundingRect(
		QString::number(this->id));

	p.setBrush(this->background);
	p.setPen(this->border);
	p.drawEllipse(boundingRect);
	p.setBrush(QBrush(QColor(230, 130, 130)));

	p.drawText(QPoint(realPosition.getX() * ratio + ratioedRadius -
							fontSize.width() / 2,
							realPosition.getY() * ratio + ratioedRadius +
							fontSize.height() / 2), QString::number(this->id));
}
