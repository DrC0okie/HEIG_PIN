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
#include "position.h"
#include "json.hpp"

using ExplosionTimes = std::vector<std::vector<double>>;

class Particle {
public:
	Particle(int id, Position position, double radius,
				std::vector<std::vector<double>> explosionTimes);

	Particle() : id(0), position(Position()), radius(0), explosionTimes
		(std::vector<std::vector<double>>()) {}

	bool hasChild();

	std::vector<Particle> explode();

	Position getPosition() { return position; }

	double getRadius() const { return radius; }

	int getId() const { return id; }

    ExplosionTimes* getExplosionTime(){return &explosionTimes;}

private:

	int id;             // unique identifier
	Position position;  // (x,y) coordinates of the particle
	double radius;      // radius of the circular particle
	std::vector<std::vector<double>> explosionTimes;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Particle, id, position, radius, explosionTimes)
};

#endif // PARTICLE_H
