/*-----------------------------------------------------------------------------
File name : timeline.cpp
Author(s) : G. Courbat, J. Streckeisen, T. Van Hove
Creation date : 22.08.2022
Description :  Implementation of the timeline class
Compiler : Mingw-w64 g++ 11.2.0
-----------------------------------------------------------------------------*/

#include <fstream>
#include <iostream>
#include "timeline.h"
#include "utils.h"

Timeline::Timeline(const std::string &path) {
	try {
		deserialize(path);
		currentState = states.begin();
	}
	catch (std::exception &e) {
		std::cerr << e.what() << '\n';
		throw std::runtime_error("Could not deserialize the given file");
	}
}

std::ostream &operator<<(std::ostream &os, const Timeline &tl) {
	json tl_j;
	if (tl.states.empty())
		tl_j = "{ \"states\": null}"_json;
	else
		tl_j = tl;
	std::cout << std::setw(4) << tl_j << '\n';
	return os;
}

void
Timeline::serialize(const std::string &outputPath, const std::string &fileName) {
	serialize(outputPath + addExtension(fileName, ".tlin"));
}

void Timeline::serialize(const std::string &outputPath) {
	std::ofstream ofs;
	json tl_j = *this;
	try {
		ofs.open(outputPath);
	}
	catch (std::ios_base::failure &e) {
		throw "Error creating the file \'" + outputPath + "\' : " + e.what();
	}
	ofs << std::setw(4) << tl_j;
	ofs.close();
}

void Timeline::deserialize(const std::string &inputPath) {
	std::ifstream f(inputPath);
	json data = json::parse(f);
	this->states = data.get<Timeline>().states;
}

void Timeline::setCurrentState(double time) {
	if (!this->isEmpty()) {
		// Search the state before the first one with time bigger than the actual time
		StateIterator newState = std::prev(std::find_if(states.begin(), states.end
			(), [=](State s) { return s.getTime() > time; }), 1);

		// Update the current state
		if (newState != currentState) {
			currentState = newState;
		}
	}
}

void Timeline::setNextState() {
	if (!isLastState(currentState))
		++currentState;
}

void Timeline::setPreviousState() {
	if (!isFirstState(currentState))
		--currentState;
}

State *Timeline::getCurrentState() {
	return &(*currentState);
}

State *Timeline::getNextState() {
	if (!isLastState(currentState))
		return &(*std::next(currentState, 1));
	return &(*currentState);
}

State *Timeline::getPreviousState() {
	if (!isFirstState(currentState))
		return &(*std::prev(currentState, 1));
	return &(*currentState);
}

State *Timeline::getLastState() {
	return &(*(states.end() - 1));
}
