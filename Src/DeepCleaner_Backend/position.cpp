/*-----------------------------------------------------------------------------
File name : position.cpp
Author(s) : G. Courbat, J. Streckeisen, T. Van Hove
Creation date : 22.08.2022
Description :  Implementation of the position class
Compiler : Mingw-w64 g++ 11.2.0
-----------------------------------------------------------------------------*/

#include "position.h"

Position::Position(double x, double y) {
	this->x = x;
	this->y = y;
}

double Position::getX() {
	return this->x;
}

double Position::getY() {
	return this->y;
}
