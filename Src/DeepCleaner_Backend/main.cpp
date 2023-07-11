/*-----------------------------------------------------------------------------
File name : main.cpp
Author(s) : G. Courbat, J. Streckeisen, T. Van Hove
Creation date : 29.08.2022
Description :  Program that handles the timeline generation based on a base state
 and constraints

Command line arguments: DeepCleaner_Backend [-help] [-b <Base state path>]
 [-c <Constraints path>] [-o <Output path>]
Compiler : Mingw-w64 g++ 11.2.0
-----------------------------------------------------------------------------*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include "utils.h"
#include "timeline.h"
#include "position.h"
#include "state.h"
#include "robot.h"
#include "particle.h"
#include "trajectory.h"
#include <exception>
#include <set>

using namespace std;
const double SQRT2 = sqrt(2.0);
const string CONSTRAINT_EXT = ".constraints", STATE_EXT = ".stat",
        TIMELINE_EXT = ".tlin";
const char BASE_STATE_ARG = 'b', CONSTRAINTS_ARG = 'c', OUTPUT_PATH_ARG = 'o';
const string HELP1 = "-help", HELP2 = "-?", HELP3 = "-h";
const double EPSILON = 2.0;
const string DEFAULT_PATH = R"(..\..\JSON\)";
const int FRAME_PER_SEC = 24;
const double TIME_PER_FRAME = 1.0 / FRAME_PER_SEC;

typedef enum {
    IDLE, ROTATION, LINE
} MovementType;

struct Movement {
    Robot *robot;
    MovementType movementType;
    double lSpeed;
    double rSpeed;
};

map<int, Movement> initRobots(vector<Robot> &robots, MovementType status);

void assignAllNearestParticle(vector<Robot> &robots, vector<Particle> &particles);

void assignNearestParticle(Robot &robot, vector<Particle> &particles);

Movement initRotation(Robot &r, Position pPos, Constraints con);

Movement initLineMovement(Robot &r, Position pPos, Constraints con, double pRadius);

Movement initIdleMovement(Robot &r);

bool hasExploded(Particle &p, double currentTime);

void showMenuHelp();

void showArgHelp();

string argumentList();

bool manageArguments(int argc, char *argv[], string &bst, string &cst, string &tln);

void menuSelection(string &bst, string &cst, string &out);

void manageParticleExplosion(double timer, bool &explosionHappened,
                             vector<Particle> &particles);

Particle *particleCollision(Robot r1, vector<Particle> &particles);

bool robotCollision(Robot r1, vector<Robot> &robs);

Particle *findTargetParticle(Robot r, vector<Particle> &particles);


int main(int argc, char *argv[]) {
    string outputFname = DEFAULT_PATH + "generatedTimeline" + TIMELINE_EXT,
            baseStateFname = DEFAULT_PATH + "stateOriginExemple" + STATE_EXT,
            constraintsFname = DEFAULT_PATH + "constraints" + CONSTRAINT_EXT;
    if (!manageArguments(argc, argv, baseStateFname, constraintsFname,
                         outputFname)) {
        menuSelection(baseStateFname, constraintsFname, outputFname);
    }
    try {
        //Create a timeline with base state
        Timeline newTimeline;
        newTimeline.deserializeConstraints(constraintsFname);
        Constraints constraints = newTimeline.getConstraints();
        newTimeline.addAndSetState(State(baseStateFname));

        State *initialState = newTimeline.getCurrentState();
        vector<Robot> robots = initialState->getRobots();
        vector<Particle> particles = initialState->getParticles();

        Position wo(initialState->getWorldOrigin());
        Position we(initialState->getWorldEnd());

        map<int, Movement> movementMap = initRobots(robots, IDLE);

        assignAllNearestParticle(robots, particles);

        // robot id->movement
        double timer = 0;
        double targetAngle;

        bool particleExploded;
        bool commandSent;
        bool particleEaten;
        bool hasCollided;

        bool hasCollidedWithParticle;
        bool hasCollidedWithRobot;

        while (!newTimeline.getCurrentState()->getParticles().empty()) {
            // Flags for state creation
            particleExploded = false;
            commandSent = false;
            particleEaten = false;
            hasCollided = false;
            //For each particle Manage check if it will explode
            manageParticleExplosion(timer, particleExploded, particles);
            if (particleExploded)
                assignAllNearestParticle(robots, particles);

            for (Robot &r: robots) {
                hasCollidedWithParticle = false;
                hasCollidedWithRobot = false;
                //Find the particle linked to the current robot
                Particle *p = findTargetParticle(r, particles);

                //Get the actual movement status of the robot
                Movement movement = {movementMap[r.getId()]};


                //Special condition for collision management.
                if (movement.movementType != IDLE) {
                    //Store object that collides with our robot
                    bool robotCollider = robotCollision(r, robots);
                    Particle *particleCollider = particleCollision(r, particles);

                    if (robotCollider || particleCollider != nullptr) {
                        movementMap[r.getId()] = initIdleMovement(r);
                        r.setSpeed(movement.lSpeed, movement.rSpeed);
                        hasCollided = true;
                    }
                    if (robotCollider) {
							  //se stop
                        hasCollidedWithRobot = true;
								movementMap[r.getId()] = initIdleMovement(r);
								r.setSpeed(movement.lSpeed,movement.rSpeed);
                        
                    }
                    if (particleCollider != nullptr) {
                        particles.erase(remove_if(particles.begin(), particles.end(),
                                                  [=](const Particle &p) {
                                                      return p.getId() ==
                                                             particleCollider->getId();
                                                  }));
                        particleEaten = true;
                        hasCollidedWithParticle = true;
                        r.setScore(r.getScore() + getArea(p->getRadius()));
                    }
                }
                movement = {movementMap[r.getId()]};
                if (movement.movementType == IDLE) {

                    //If the robot collided with particle or if it hasn't collided at all
                    // starting point of our moving algorithm
                    if (hasCollidedWithParticle || (!hasCollidedWithParticle &&
                                                    !hasCollidedWithRobot)) {
                        assignAllNearestParticle(robots, particles);
                        p = findTargetParticle(r, particles);


                        targetAngle = getAngle(r.getPosition(), p->getPosition());
                        if (!equal(targetAngle, r.getAngle(), 0.005)) {

                            //Set the new movement the robot is doing
                            movement = initRotation(r, p->getPosition(),
                                                    constraints);
                            movementMap[r.getId()] = {movement};
                            r.setSpeed(movement.lSpeed, movement.rSpeed);
                        } else {
                            // Rotation is finished, now go in straight line to the particle
                            movement = initLineMovement(r, p->getPosition(),
                                                        constraints, p->getRadius());
                            r.setSpeed(movement.lSpeed, movement.rSpeed);
                            movementMap[r.getId()] = {movement};
                        }
                        commandSent = true;
                    }

                } else if (movement.movementType == ROTATION) {
                    if (particleEaten || particleExploded) {
                        assignAllNearestParticle(robots, particles);
                        p = findTargetParticle(r, particles);
                    }
                    targetAngle = getAngle(r.getPosition(), p->getPosition());
                    if (!equal(targetAngle, r.getAngle(), 0.005)) {
                        double newAngle = updateAngle(r.getAngle(), r.getRadius(), r
                                .getLeftSpeed(), r.getRightSpeed(), TIME_PER_FRAME);
                        r.setAngle(toDeg(newAngle));
                    } else {
                        movement = initLineMovement(r, p->getPosition(),
                                                    constraints, p->getRadius());
                        r.setSpeed(movement.lSpeed, movement.rSpeed);
                        movementMap[r.getId()] = {movement};
                        commandSent = true;
                    }
                } else if (movement.movementType == LINE) {
                    if (particleEaten || particleExploded) {
                        assignAllNearestParticle(robots, particles);
                        p = findTargetParticle(r, particles);
                    }
                    Position newPos = updateCoordinate(r.getPosition(),
                                                       r.getRightSpeed(),
                                                       r.getAngle(RAD),
                                                       TIME_PER_FRAME);
                    r.setPosition(newPos);
                }

            }
            if (particleExploded || commandSent || particleEaten || hasCollided) {
                newTimeline.addAndSetState(State(timer, wo, we, robots, particles));
            }
            timer += TIME_PER_FRAME;
        }
        newTimeline.serialize(outputFname);
        cout << "Timeline successfully generated at : " << outputFname << '\n';
        //pause("Press enter to quit...");
    }
    catch (exception &e) {
        cerr << "Exception occurred : " << e.what() << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

Movement initRotation(Robot &r, Position pPos, Constraints con) {
    // Calculate the angle that we need to rotate with deltaX and deltaY
    double targetAngle = getAngle(r.getPosition(), pPos);

    // Calculate the difference between the 2 angles
    double effectiveAngle = deltaAngle(r.getAngle(RAD), targetAngle);

    //Manage the direction of rotation to rotate the minimum distance
    AngularDirection dir = rotateShortestPath(r.getAngle(RAD), targetAngle);

    //Calculate the rotation time with maximum speed (left & right must be equal)
    double maxRotationSpeed = min(con.maxBackwardSpeed, con.maxForwardSpeed);

    //Inverse rotationSpeed if CCW
    if (dir == CCW)
        maxRotationSpeed = -maxRotationSpeed;

    // Get the angular speed in rad/s
    double omega = angularSpeed(r.getRadius(), maxRotationSpeed, -maxRotationSpeed);

    // Now get the time that the rotation takes
    double time = rotationTime(effectiveAngle, omega);

    //Avoid precision problems
    double syncedTime = getSyncTime(time, con.commandTimeInterval);

    // Calculate the new speed synced to the time contraints
    if (dir == CW)
        omega = effectiveAngle / syncedTime;
    else
        omega = -effectiveAngle / syncedTime;

    double syncedSpeed = (omega * 2 * r.getRadius()) / 2;

    Movement rotMovement = {&r, ROTATION, syncedSpeed,
                            -syncedSpeed};
    return rotMovement;
}

Movement initLineMovement(Robot &r, Position pPos, Constraints con, double pRadius) {
    //TODO ALIGNEMENT AVEC TIMER
    double distance =
            linearDistance(r.getPosition(), pPos) - r.getRadius() - pRadius;
    double time = distance / con.maxForwardSpeed;
    double syncedTime = getSyncTime(time, con.commandTimeInterval);
    double syncedSpeed = distance / syncedTime;
    Movement linearMovement = {&r, LINE, syncedSpeed, syncedSpeed};
    return linearMovement;
}



Movement initIdleMovement(Robot &r) {
    return Movement{&r, IDLE, 0, 0};
}

bool allRobotsStopped(map<int, Movement> &movementMap) {
    for (auto &[id, movement]: movementMap) {
        if (movement.movementType != IDLE) {
            return false;
        }
    }
    return true;
}

bool hasExploded(Particle &p, double currentTime) {
    ExplosionTimes times = *p.getExplosionTime();
    if (times.at(0).at(0) < currentTime || equal(times.at(0).at(0), currentTime,
                                                 0.0001)) {
        return true;
    }
    return false;
}

bool robotCollision(Robot r1, vector<Robot> &robs) {
    for (Robot &robot: robs) {
        if (r1.getId() != robot.getId() &&
            detectCollision(r1.getPosition(), r1.getRadius(), robot.getPosition(),
                            robot.getRadius(), EPSILON)) {
            return true;
        }
    }
    return false;
}

Particle *particleCollision(Robot r1, vector<Particle> &particles) {
    for (Particle &p: particles) {
        if (detectCollision(r1.getPosition(), r1.getRadius(), p.getPosition(),
                            p.getRadius(), EPSILON)) {
            return &p;
        }
    }
    return nullptr;
}

map<int, Movement> initRobots(vector<Robot> &robots, MovementType status) {
    map<int, Movement> movementMap;
    for (Robot &rob: robots) {
        // Set the robots to idle
        movementMap[rob.getId()] = {&rob, status, 0, 0};
    }
    return movementMap;
}

void assignAllNearestParticle(vector<Robot> &robots, vector<Particle> &particles) {
    //The robots select the nearest particle automatically
    if(particles.size() < 1)
        return;
    for (Robot &rob: robots) {
        assignNearestParticle(rob, particles);
        //if assigned particle == other robots assigned particle, then change it
        Robot samePartRob =  *(find_if(robots.begin(), robots.end(),[=](Robot r)
        {
            if(r.getTargetParticleId()!= -1 && r.getId() != rob.getId()) {
                return r.getTargetParticleId() == rob.getTargetParticleId();
            }
            return false;
        }));

        if(rob.getTargetParticleId() != -1 &&
        rob.getTargetParticleId() == samePartRob.getTargetParticleId()){

            vector<Particle> copyPart(particles);
            copyPart.erase(remove_if(copyPart.begin(), copyPart.end(),
                                      [=](const Particle &p) {
                                          return rob.getTargetParticleId() == p.getId();
                                      }));

            assignNearestParticle(rob, copyPart);
        }
    }
}

void assignNearestParticle(Robot &robot, vector<Particle> &particles) {
    int nearestParticleId = -1;
    double dist = numeric_limits<double>::max();
    for (Particle &p: particles) {
        double newDist = linearDistance(robot.getPosition(), p.getPosition());
        if (newDist <= dist) {
            nearestParticleId = p.getId();
            dist = newDist;
        }
    }
    robot.setTargetParticleId(nearestParticleId);
}

void manageParticleExplosion(double timer, bool &explosionHappened,
                             vector<Particle> &particles) {
    vector<Particle> copyParticles(particles);
    int maxId = std::max_element(copyParticles.begin(), copyParticles.end(),
                                 [](Particle &p1, Particle &p2) {
                                     return p1.getId()
                                            < p2.getId();
                                 })->getId();
    for (Particle &particle: copyParticles) {
        try {
            if (hasExploded(particle, timer)) {
                int id = particle.getId();
                ExplosionTimes currentExplosionTime = *particle.getExplosionTime();
                if (currentExplosionTime.size() > 1) {
                    double radius = ((particle.getRadius() * 2) / (1 + SQRT2)) / 2;
                    Position newPos[] = {
                            Position(particle.getPosition().getX() - radius,
                                     particle.getPosition().getY() - radius),
                            Position(particle.getPosition().getX() - radius,
                                     particle.getPosition().getY() + radius),
                            Position(particle.getPosition().getX() + radius,
                                     particle.getPosition().getY() - radius),
                            Position(particle.getPosition().getX() + radius,
                                     particle.getPosition().getY() + radius)};
                    unsigned index = 0;
                    ExplosionTimes newExplosionTimes;
                    for (double &childTime: currentExplosionTime.at(1)) {
                        maxId++;
                        newExplosionTimes.push_back({childTime});
                        if (currentExplosionTime.size() > 2) {

                                //Push back each child vector
                       newExplosionTimes.push_back(
                               {currentExplosionTime.at(2).at(index * 4u),
                                currentExplosionTime.at(2).at(index * 4 + 1u),
                                currentExplosionTime.at(2).at(index * 4 + 2u),
                                currentExplosionTime.at(2).at(index * 4 + 3u)});

                        }
                        //Create 4 new particles
                        particles.emplace_back(maxId, newPos[index], radius,
                                               newExplosionTimes);
                        newExplosionTimes.clear();
                        index++;
                    }
                }
                explosionHappened = true;
                //Erase the exploded particle
                particles.erase(remove_if(particles.begin(), particles.end(),
                                          [=](const Particle &p) {
                                              return p.getId() == id;
                                          }));
            }
        }
        catch (exception &e) {
            cerr << "Id : " << particle.getId()
            << " Explosion time : " << (*particle.getExplosionTime())[0][0]
            << " Timer : " << timer << endl
            << "What " << e.what() << endl;
            throw(e);
        }
    }
}


Particle *findTargetParticle(Robot r, vector<Particle> &particles) {
    Particle *p = &(*find_if(particles.begin(),
                             particles.end(), [=](const Particle &p) {
                return p.getId() == r.getTargetParticleId();
            }));
    return p;
}

void menuSelection(string &bst, string &cst, string &out) {
    int menuSelection = 0;
    do {
        string currentBst = bst.substr(bst.find_last_of('\\') + 1);
        string currentCst = cst.substr(bst.find_last_of('\\') + 1);
        string currentOut = out.substr(bst.find_last_of('\\') + 1);
        menuSelection = 0;
        std::cout << "Please enter the desired operation : \n\n"
                  << "1) Load the base state & generate the Timeline\n"
                  << "2) Set the output timeline name (current = " << currentOut
                  << ")\n"
                  << "3) set the base state file name (current = " << currentBst
                  << ")\n"
                  << "4) set the constraints file name (current = " << currentCst
                  << ")\n"
                  << "5) help\n"
                  << "6) Exit \n";

        while (!(cin >> menuSelection)) {
            cout << "Invalid input, please retry\n";
            cin.clear();
            emptyBuffer();
        }

        switch (menuSelection) {
            case 1:
                break;
            case 2:
                cout << "\nEnter the timeline file name : ";
                cin >> out;
                emptyBuffer();
                out = setRelativePath(DEFAULT_PATH, out,
                                      TIMELINE_EXT);
                break;
            case 3:
                cout << "\nEnter the state file name : ";
                cin >> bst;
                emptyBuffer();
                bst = setRelativePath(DEFAULT_PATH, bst,
                                      STATE_EXT);
                break;
            case 4:
                cout << "\nEnter the constraints file name : ";
                cin >> cst;
                emptyBuffer();
                cst = setRelativePath(DEFAULT_PATH, cst,
                                      CONSTRAINT_EXT);
                break;
            case 5:
                showMenuHelp();
                break;
            case 6:
                exit(EXIT_SUCCESS);
            default :
                cout << "Invalid menu choice\n";
        }

    } while (menuSelection != 1);
}

string argumentList() {
    stringstream ss;
    ss << "[-" << BASE_STATE_ARG << " <Base state file name>]\n"
       << "[-" << CONSTRAINTS_ARG << " <Constraints file name>]\n"
       << "[-" << OUTPUT_PATH_ARG << " <Output file name>]\n";
    return ss.str();
}

void showMenuHelp() {
    cout << "-------------------------Help---------------------------\n";
    cout << "Command line arguments : \n";
    cout << argumentList();
    cout << "\nThe default path to open files is " << DEFAULT_PATH << "\n";
    cout << "Please put the origin state file & constraint file there\n";
    pause("Press enter to continue...\n");
}

void showArgHelp() {
    cout << "Command line arguments : \n";
    cout << argumentList();
    cout << "Continue/quit? [y/n]";
    char key;
    string error = "Invalid input [y/n], please retry\n";
    do {
        while (!(cin >> key)) {
            cout << error;
            cin.clear();
            emptyBuffer();
        }
        key = char(tolower(key));
    } while (key != 'y' && key != 'n');

    if (key == 'n')
        exit(EXIT_SUCCESS);
}

bool manageArguments(int argc, char *argv[], string &bst, string &cst, string &tln) {
    printTitle();
    // No arguments given
    if (argc <= 1)
        return false;
    // Help detection
    string a = argv[1];
    if (argc == 2 && (a == HELP1 || a == HELP2 || a == HELP3)) {
        showArgHelp();
        return false;
    }
    if (argc == 7) {
        for (int i = 0; i < argc; ++i) {
            string arg = argv[i];
            //Letter detection
            if (arg.size() == 2) {
                string path = argv[i + 1];
                switch (arg.at(1)) {
                    case BASE_STATE_ARG :
                        bst = setRelativePath(DEFAULT_PATH, path, STATE_EXT);
                        break;
                    case CONSTRAINTS_ARG :
                        cst = setRelativePath(DEFAULT_PATH, path, CONSTRAINT_EXT);
                        break;
                    case OUTPUT_PATH_ARG :
                        tln = setRelativePath(DEFAULT_PATH, path, TIMELINE_EXT);
                        break;
                    default :
                        break;
                }
            }
        }
        return true;
    }
    cout << "Please give the following arguments : \n" << argumentList()
         << "The program will close.\n";
    exit(EXIT_FAILURE);
}
