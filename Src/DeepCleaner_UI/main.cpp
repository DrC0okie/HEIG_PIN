/*-----------------------------------------------------------------------------
File name : Main.cpp
Author(s) : G. Courbat, J. Streckeisen, T. Van Hove
Creation date : 22.08.2022
Description :  Main entry point
Compiler : Mingw-w64 g++ 11.2.0
-----------------------------------------------------------------------------*/

#include "mainwindow.h"
#include <QApplication>
#include <string>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
