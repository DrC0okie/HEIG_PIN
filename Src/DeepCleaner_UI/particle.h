/*-----------------------------------------------------------------------------
File name : particle.h
Author(s) : G. Courbat, J. Streckeisen, T. Van Hove
Creation date : 22.08.2022
Description :  Header of the class handling the Particle objects
Compiler : Mingw-w64 g++ 11.2.0
-----------------------------------------------------------------------------*/

#ifndef PARTICLE_H
#define PARTICLE_H

#include <vector>
#include <string>
#include <QObject>
#include <QPainter>
#include "position.h"
#include "json.hpp"

class Particle {
public:
	Particle(int id, Position position, double radius,
				std::vector<std::vector<double>> explosionTimes);

	Particle() : id(0), position(Position()), radius(0), explosionTimes
		(std::vector<std::vector<double>>()) {}

	void draw(QPaintDevice *device, Position worldOrigin, double ratio);

	double getRadius() { return radius; };
	signals:

private:
	static const QBrush background;
	static const QPen border;

	int id;             // unique identifier
	Position position;  // (x,y) coordinates of the particle
	double radius;      // radius of the circular particle
	std::vector<std::vector<double>> explosionTimes;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Particle, id, position, radius, explosionTimes)
};

#endif // PARTICLE_H
