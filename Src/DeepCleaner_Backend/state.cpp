/*-----------------------------------------------------------------------------
File name : state.cpp
Author(s) : G. Courbat, J. Streckeisen, T. Van Hove
Creation date : 22.08.2022
Description :  Implementation of the state class
Compiler : Mingw-w64 g++ 11.2.0
-----------------------------------------------------------------------------*/

#include <iostream>
#include <fstream>
#include <utility>
#include "state.h"
#include "utils.h"

State::State(const std::string &path) {
	try {
		deserialize(path);
	}
	catch (std::exception &e) {
		std::cerr << e.what() << '\n';
		throw std::runtime_error("Could not deserialize the given file");
	}
}

State::State(double time, Position worldOrigin, Position worldEnd, std::vector<Robot>
robots, std::vector<Particle> particles) {
	this->time = time;
	this->worldOrigin = worldOrigin;
	this->worldEnd = worldEnd;
	this->robots = std::move(robots);
	this->particles = std::move(particles);
}

void State::serialize(const std::string &outputPath, const std::string &fileName) {
	serialize(outputPath + addExtension(fileName, this->fileExtension));
}

void State::serialize(const std::string &outputPath) {
	std::ofstream ofs;
	json st_j = *this;
	try {
		ofs.open(outputPath);
	}
	catch (std::ios_base::failure &e) {
		throw "Error creating the file \'" + outputPath + "\' : " + e.what();
	}
	ofs << std::setw(4) << st_j;
	ofs.close();
}

void State::deserialize(const std::string &fileName) {
	std::ifstream f(fileName);
	json data = json::parse(f);
	this->time = data.get<State>().time;
	this->worldOrigin = data.get<State>().worldOrigin;
	this->worldEnd = data.get<State>().worldEnd;
	this->robots = data.get<State>().robots;
	this->particles = data.get<State>().particles;
}

std::ostream &operator<<(std::ostream &os, const State &s) {
	json s_j = s;
	std::cout << std::setw(4) << s_j << '\n';
	return os;
}

Position State::getWorldEnd() {
	return this->worldEnd;
}

Position State::getWorldOrigin() {
	return this->worldOrigin;
}
