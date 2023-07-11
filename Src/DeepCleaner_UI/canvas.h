/*-----------------------------------------------------------------------------
File name : canvas.h
Author(s) : G. Courbat, J. Streckeisen, T. Van Hove
Creation date : 22.08.2022
Description :  Header of the class handling the drawing on the central frame.
This object also handles the display of the states and the resizing of the window
Compiler : Mingw-w64 g++ 11.2.0
-----------------------------------------------------------------------------*/

#ifndef CANVAS_H
#define CANVAS_H

#include <QFrame>
#include "state.h"

class Canvas : public QFrame {
	Q_OBJECT

public:
	explicit Canvas(QWidget *parent = nullptr);

	virtual void paintEvent(QPaintEvent *event);

	void trajectoryR();

public
	slots:
		void updateState(State * state);

	void updateSize(QSize size);

	void DrawOnRefresh(unsigned mult, double nextStateTime);

	void drawEvent();


private:
	State loadedState;
	int baseSize = 500;
	int newSize;
	double elapsedTime = 0;

	void setBaseSize(int size);

	double ratio = 1.0;
};

#endif // CANVAS_H
