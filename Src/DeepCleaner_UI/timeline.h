/*-----------------------------------------------------------------------------
File name : timeline.h
Author(s) : G. Courbat, J. Streckeisen, T. Van Hove
Creation date : 22.08.2022
Description :  Header of the class handling the Timeline objects
Compiler : Mingw-w64 g++ 11.2.0
-----------------------------------------------------------------------------*/

#ifndef TIMELINE_H
#define TIMELINE_H

#include <vector>
#include <string>
#include "state.h"
#include "json.hpp"

using json = nlohmann::json;
using StateIterator = std::vector<State>::iterator;

class Timeline {

public:
	Timeline() : states(std::vector<State>()) {}

	//The full path file (with extension) must be given
	Timeline(const std::string &path);

	void setCurrentState(double time);

	void setNextState();

	void setPreviousState();

	State *getCurrentState();

	State *getNextState();

	State *getPreviousState();

	State *getLastState();

	friend std::ostream &operator<<(std::ostream &os, const Timeline &tl);

	bool isLastState(StateIterator state) { return state == (states.end() - 1); }

	bool isFirstState(StateIterator state) { return state == (states.begin()); }

	bool isEmpty() { return states.empty(); }

	void serialize(const std::string &outputPath, const std::string &fileName);

	void serialize(const std::string &outputPath);

	void deserialize(const std::string &inputPath);

private:
	std::vector<State> states;
	StateIterator currentState;
	std::string fileExtension = ".tlin";

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Timeline, states)
};

#endif // TIMELINE_H
