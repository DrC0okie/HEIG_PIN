/*-----------------------------------------------------------------------------
File name : mainwindow.h
Author(s) : G. Courbat, J. Streckeisen, T. Van Hove
Creation date : 22.08.2022
Description :  Main window class handling all the objects events and UI logic
Compiler : Mingw-w64 g++ 11.2.0
-----------------------------------------------------------------------------*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QTimer>
#include <QDir>
#include "timeline.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum CurrentlyLoaded {
	NONE, STATE, TIMELINE
};

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);

	~MainWindow();

	void displayState(State *state);

private
	slots:
		void on_actionQuit_triggered();

	void on_actionOpen_state_triggered();

	void on_actionOpen_timeline_triggered();

	void on_actionSave_state_triggered();

	void on_doubleSpinBox_editingFinished();

	void timerUpdate();

	void on_ResetButton_released();

	void on_StartButton_toggled(bool checked);

	signals:
		void sendToCanvas(State * state);

	void resizeCanvas(QSize size);

	void sendRefreshTime(unsigned mult, double nextStateTime);

private:
	Ui::MainWindow *ui;
	QTimer *timer;
	double elapsedTime;
	Timeline timeline;
	State state;
	QDir jsonDir = QDir::currentPath();

	bool isFileLoaded() {
		return !(this->timeline.isEmpty() && this->state.isEmpty());
	}

	void enableButtons(bool start, bool reset, bool timer, bool save);

	void enableButtons(bool commonState);

	QString getFileNameFromPath(QString path);

	void displaySerializeError(std::exception &error);

	void checkFileSystem(QString jsonDirName);

	void showMessageFileNotLoaded();

	void resizeEvent(QResizeEvent *event);

	void beep();

	void saveCurrentStateFromTimeline();

	void saveCurrentState();

	CurrentlyLoaded currentlyLoaded;
};


#endif // MAINWINDOW_H
