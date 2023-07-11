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
using constStr = const std::string;

struct Constraints {
    double commandTimeInterval = 1.0;
    double maxBackwardSpeed = 5.0;
    double maxForwardSpeed = 20.0;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Constraints,
                                   commandTimeInterval,
                                   maxBackwardSpeed,
                                   maxForwardSpeed)
};

class Timeline {

public:
    Timeline() : states(std::vector<State>()) {}

    //The full path file (with extension) must be given
    explicit Timeline(constStr &path);

    void setCurrentState(double time);

    void setFirstState();

    void setLastState();

    void setNextState();

    void setPreviousState();

    void addState(const State &state);

    void addAndSetState(const State &state);

    State *getCurrentState() const;

    State *getNextState() const;

    State *getPreviousState() const;

    State *getLastState();

    State *getFirstState();

    friend std::ostream &operator<<(std::ostream &os, const Timeline &tl);

    bool isLastState(StateIterator state) const {
        return state == (states.end() - 1);
    }

    bool isFirstState(StateIterator state) const {
        return state == (states.begin());
    }

    bool isEmpty() { return states.empty(); }

    void serialize(constStr &outputPath, constStr &fileName);

    void serialize(constStr &outputPath);

    void deserialize(constStr &inputPath);

    Constraints deserializeConstraints(const std::string &fileName);

    Constraints getConstraints(){return this->constraints;}

private:
    Constraints constraints;
    std::vector<State> states;
    StateIterator currentState;
    std::string fileExtension = ".tlin";

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Timeline, states)
};

#endif // TIMELINE_H
