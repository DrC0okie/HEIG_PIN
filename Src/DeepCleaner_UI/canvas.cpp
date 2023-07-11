/*-----------------------------------------------------------------------------
File name : canvas.cpp
Author(s) : G. Courbat, J. Streckeisen, T. Van Hove
Creation date : 22.08.2022
Description :  Implementation of the canvas class
Compiler : Mingw-w64 g++ 11.2.0
-----------------------------------------------------------------------------*/

#include <iostream>
#include <QPainter>
#include <QPaintEvent>
#include <QStyleOption>
#include <QTimer>
#include "particle.h"
#include "robot.h"
#include "canvas.h"
#include "trajectory.h"
#include "utils.h"

const int FPS = 24;
const double DELTA_T = 1.0 / FPS;

Canvas::Canvas(QWidget *parent) : QFrame{parent} {
}

void Canvas::paintEvent(QPaintEvent *event) {
	QStyleOption optBackground;
	optBackground.initFrom(this);
	QPainter painter(this);
	style()->drawPrimitive(QStyle::PE_Widget, &optBackground, &painter, this);
	QWidget::paintEvent(event);
	this->resize(newSize, newSize);
	drawEvent();

}

void Canvas::DrawOnRefresh(unsigned mult, double nextStateTime) {


	if (elapsedTime < nextStateTime) {
		double shift = DELTA_T * mult;
		for (Robot &robot: loadedState.getRobots()) {

			if (equal(robot.getLeftSpeed(), robot.getRightSpeed())) {

				if (!equal(robot.getLeftSpeed(), 0.0)) {
					robot.setPosition(
						updateLinCoordinate(robot.getPosition(), robot.getLeftSpeed(),
												  robot.getAngle(), shift));
				}
			} else if (robot.getLeftSpeed() == -robot.getRightSpeed()) {

				robot.setAngle(toDeg(
					updateAngle(robot.getAngle(), robot.getRadius(), robot.getLeftSpeed(),
									robot.getRightSpeed(), shift)));
			} else {
				updateCircCoordinate(robot, shift);
			}
		}


		//Update the frame
		this->update();
	}

}


void Canvas::drawEvent() {

	for (Robot &robot: loadedState.getRobots()) {
		robot.draw(this, loadedState.getWorldOrigin(), this->ratio);
	}
	for (Particle &particle: loadedState.getParticles()) {
		particle.draw(this, loadedState.getWorldOrigin(), this->ratio);
	}
}


void Canvas::updateState(State *state) {
	State copyState = *state;
	if (copyState.getTime() < this->loadedState.getTime()) {
		this->elapsedTime = this->loadedState.getTime();
	} else {
		this->elapsedTime = copyState.getTime();
	}
	this->loadedState = copyState;
	double finalSize =
		loadedState.getWorldEnd().getX() - loadedState.getWorldOrigin().getX();
	setBaseSize(finalSize);
	this->update();
}

void Canvas::updateSize(QSize size) {
	this->newSize =
		(size.width() < size.height() ? size.width() : size.height()) - 50;
	this->ratio = (double) this->newSize / this->baseSize;
	this->update();
}

void Canvas::setBaseSize(int size) {
	this->baseSize = size;
}
