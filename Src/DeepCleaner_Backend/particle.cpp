/*-----------------------------------------------------------------------------
File name : particle.cpp
Author(s) : G. Courbat, J. Streckeisen, T. Van Hove
Creation date : 22.08.2022
Description :  Implementation of the particle class
Compiler : Mingw-w64 g++ 11.2.0
-----------------------------------------------------------------------------*/

#include "particle.h"

Particle::Particle(int id, Position position, double radius,
						 std::vector<std::vector<double>> explosionTimes) {
	this->id = id;
	this->position = position;
	this->radius = radius;
	this->explosionTimes = explosionTimes;
}

bool Particle::hasChild() {
	return !this->explosionTimes[1].empty();
}
