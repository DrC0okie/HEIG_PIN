/*-----------------------------------------------------------------------------
File name : state.h
Author(s) : G. Courbat, J. Streckeisen, T. Van Hove
Creation date : 22.08.2022
Description :  Header of the class handling the State objects
Compiler : Mingw-w64 g++ 11.2.0
-----------------------------------------------------------------------------*/

#ifndef STATE_H
#define STATE_H

#include <utility>
#include <vector>
#include "robot.h"
#include "particle.h"
#include "position.h"
#include "json.hpp"

using json = nlohmann::json;

class State {

public:
	State() : time(0.0), worldOrigin(Position()), worldEnd(Position()), robots
		(std::vector<Robot>()), particles(std::vector<Particle>()) {}

	State(const std::string &path);

	State(double time, Position worldOrigin, Position worldEnd, std::vector<Robot>
	robots, std::vector<Particle>);

	std::vector<Robot> getRobots() { return robots; }

	std::vector<Particle> &getParticles() { return particles; }

	double getTime() { return time; }

	Position getWorldOrigin();

	Position getWorldEnd();

	void serialize(const std::string &outputPath, const std::string &fileName);

	void serialize(const std::string &outputPath);

	void deserialize(const std::string &fileName);

	friend std::ostream &operator<<(std::ostream &os, const State &s);

	bool isEmpty() { return robots.empty() && particles.empty(); }

private:
	double time;
	Position worldOrigin;
	Position worldEnd;
	std::vector<Robot> robots;
	std::vector<Particle> particles;
	std::string fileExtension = ".stat";

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(State, time, worldOrigin, worldEnd, robots,
											 particles)
};

#endif // STATE_H
