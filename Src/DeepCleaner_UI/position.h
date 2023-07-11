/*-----------------------------------------------------------------------------
File name : position.h
Author(s) : G. Courbat, J. Streckeisen, T. Van Hove
Creation date : 22.08.2022
Description :  Header of the class handling the Position objects
Compiler : Mingw-w64 g++ 11.2.0
-----------------------------------------------------------------------------*/

#ifndef POSITION_H
#define POSITION_H

#include "json.hpp"

class Position {

public:
	Position() : x(0.0), y(0.0) {}

	Position(double x, double y);

	double getX();

	double getY();

	void setX(double _x) { this->x = _x; }


	void setY(double _y) { this->y = _y; }

private:
	double x; // horizontal coordinate
	double y; // vertical coordinate
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Position, x, y)
};

#endif // POSITION_H
