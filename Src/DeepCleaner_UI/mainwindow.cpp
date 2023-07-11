/*-----------------------------------------------------------------------------
File name : mainwindow.cpp
Author(s) : G. Courbat, J. Streckeisen, T. Van Hove
Creation date : 22.08.2022
Description :  Implementation of the mainwindow functions and events
Compiler : Mingw-w64 g++ 11.2.0
-----------------------------------------------------------------------------*/

#include <iostream>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include "mainwindow.h"
#include "canvas.h"
#include "ui_mainwindow.h"
#include "math.h"

#ifdef _WIN32
#include <windows.h>
#endif

using Qmb = QMessageBox;

QString calculateScore(std::vector<Robot> robots, std::vector<Particle> particles);


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow) {
	currentlyLoaded = NONE;
	ui->setupUi(this);
	elapsedTime = 0.0;
	timer = new QTimer(this);

	// Events connection
	connect(this, &MainWindow::sendToCanvas, ui->frame, &Canvas::updateState);
	connect(this, &MainWindow::resizeCanvas, ui->frame, &Canvas::updateSize);
	connect(this, &MainWindow::sendRefreshTime, ui->frame, &Canvas::DrawOnRefresh);
	connect(timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));

	enableButtons(false);

	//Check & restore the JSON directory
	checkFileSystem("JSON");
}

MainWindow::~MainWindow() {
	delete ui;
	delete timer;
}


void MainWindow::checkFileSystem(QString jsonDirName) {
	// Go to the parent directory
	jsonDir.cdUp();

	// Make the JSON directory if it doesn't exist
	if (!jsonDir.cd(jsonDirName)) {
		jsonDir.mkdir(jsonDirName);
	}

	// Go to the JSON directory
	jsonDir.cd(jsonDirName);
}

void MainWindow::beep() {
	// Portable beep management
#ifdef _WIN32
	MessageBeep(MB_ICONERROR);
#else
	// Do not beep if the motherboard is not equipped with a mic
	// or if the std::cout is redirected
	std::cout << '\a';
#endif
}

void MainWindow::enableButtons(bool start, bool reset, bool timer, bool save) {
	ui->StartButton->setEnabled(start);
	ui->ResetButton->setEnabled(reset);
	ui->doubleSpinBox->setEnabled(timer);
	ui->actionSave_state->setVisible(save);
}

void MainWindow::enableButtons(bool state) {
	enableButtons(state, state, state, state);
}

QString MainWindow::getFileNameFromPath(QString path) {
	QFileInfo fi(path);
	return fi.fileName();
}

void MainWindow::displaySerializeError(std::exception &error) {
	beep();
	// Display the exception error
	Qmb msgBox;
	msgBox.setIcon(QMessageBox::Critical);
	msgBox.setWindowTitle("Error");
	msgBox.setText(tr(error.what()));
	msgBox.exec();
}

void MainWindow::on_actionOpen_state_triggered() {

	QString filePath = QFileDialog::getOpenFileName(this, "open a state file",
																	jsonDir.path(),
																	tr("State file (*.stat)"));
	// The window has been closed -> do nothing
	if (filePath.isEmpty()) {
		return;
	}
	try {
		this->state = State(filePath.toStdString());
	}
	catch (std::runtime_error e) {
		displaySerializeError(e);
		return;
	}
	//Retrieve fileName & update the window name
	QString fileName = getFileNameFromPath(filePath);
	setWindowTitle(fileName);

	//Reset the timer and display the state
	this->elapsedTime = 0.0;
	displayState(&this->state);

	currentlyLoaded = STATE;

	//Only enable the "save state" button
	enableButtons(false, false, false, true);
}

void MainWindow::on_actionOpen_timeline_triggered() {
	QString filePath = QFileDialog::getOpenFileName(this,
																	"open a timeline file",
																	jsonDir.path(),
																	tr("Timeline file (*.tlin)"));
	// The window has been closed -> do nothing
	if (filePath.isEmpty())
		return;

	try {
		//Creation of the timeline object
		this->timeline = Timeline(filePath.toStdString());
	}
	catch (std::runtime_error e) {
		displaySerializeError(e);
		return;
	}

	//Retrieve fileName & update the window name
	QString fileName = getFileNameFromPath(filePath);
	setWindowTitle(fileName);

	//Reset the timer
	this->elapsedTime = 0.0;

	//Initialize the timeline
	this->timeline.setCurrentState(0);
	this->ui->Score_Display->setText(
		calculateScore(this->timeline.getCurrentState()->getRobots(),
							this->timeline.getCurrentState()->getParticles()));
	displayState(this->timeline.getCurrentState());


	//Enable all the buttons only if the file has been loaded
	enableButtons(true);

	//Set the maximum time of the counter
	ui->doubleSpinBox->setMaximum(timeline.getLastState()->getTime());
	currentlyLoaded = TIMELINE;
}

void MainWindow::saveCurrentState() {
	QString fileName = QFileDialog::getSaveFileName(this, "Save the state",
																	jsonDir.path(),
																	tr("State file (*.stat)"));
	try {
		this->state.serialize(fileName.toStdString());
	}
	catch (std::exception &e) {
		displaySerializeError(e);
	}
}

void MainWindow::saveCurrentStateFromTimeline() {
	QString fileName = QFileDialog::getSaveFileName(this, "Save the state",
																	jsonDir.path(),
																	tr("State file (*.stat)"));
	this->state = *this->timeline.getCurrentState();
	this->state.serialize(fileName.toStdString());
}

void MainWindow::on_actionSave_state_triggered() {
	switch (this->currentlyLoaded) {
		case STATE:
			saveCurrentState();
			break;
		case TIMELINE:
			saveCurrentStateFromTimeline();
			break;
		case NONE:
			break;
		default:
			return;
	}
}

void MainWindow::on_actionQuit_triggered() {
	QApplication::quit();
}

void MainWindow::on_doubleSpinBox_editingFinished() {
	// Get the entered value
	this->elapsedTime = ui->doubleSpinBox->value();

	// Set the state corresponding to the value & display
	if (!this->timeline.isEmpty()) {
		this->timeline.setCurrentState(this->elapsedTime);
		displayState(this->timeline.getCurrentState());
	}
}

void MainWindow::displayState(State *state) {
	this->ui->Score_Display->setText(
		calculateScore(this->timeline.getCurrentState()->getRobots(),
							this->timeline.getCurrentState()->getParticles()));
	emit sendToCanvas(state);
	emit
	this->resizeCanvas(QSize(this->width(), this->height()));
}

void MainWindow::timerUpdate() {
	// Check that the timer is stopped after displaying the last state
	if (this->elapsedTime < this->timeline.getLastState()->getTime()) {

		//Increment & display the time
        if (int((elapsedTime / ui->spinBox->value()) * 1000) % 42 == 0) {
			emit sendRefreshTime(ui->spinBox->value(),
										this->timeline.getNextState()->getTime());
		}
		this->elapsedTime += 0.001 * ui->spinBox->value();
		ui->doubleSpinBox->setValue(this->elapsedTime);

		//TODO : Update the score here
	} else {
		//By unchecking the button on_StartButton_toggled is called
		ui->StartButton->setChecked(false);
	}

	//Update the states in the timeline & display it
	if (this->elapsedTime >= this->timeline.getNextState()->getTime()) {
		this->timeline.setNextState();
		displayState(this->timeline.getCurrentState());

	}
}

void MainWindow::on_ResetButton_released() {
	//Reset the time
	this->elapsedTime = 0.0;
	ui->doubleSpinBox->setValue(this->elapsedTime);

	//Uncheck the start button and call on_StartButton_toggled
	ui->StartButton->setChecked(false);

	//Reset the timeline & display the first state
	if (!this->timeline.isEmpty()) {
		this->timeline.setCurrentState(0);
		displayState(this->timeline.getCurrentState());
	}
}


void MainWindow::on_StartButton_toggled(bool checked) {
	// Start timer, load Timeline with entered time & display state
	if (!this->timeline.isEmpty()) {
		if (ui->StartButton->isChecked()) {
			timer->start(1);
			ui->doubleSpinBox->setValue(this->elapsedTime);
			this->timeline.setCurrentState(this->elapsedTime);
			displayState(this->timeline.getCurrentState());

			// Change the button text
			ui->StartButton->setText("| |");
		} else {
			timer->stop();

			//Change the button text
			ui->StartButton->setText("|>");
		}
	}
}

void MainWindow::resizeEvent(QResizeEvent *event) {
	// Window size management
	this->setMinimumHeight(ui->frame->y() + 250);
	this->setMinimumWidth(ui->frame->x() + 250);
	emit
	this->resizeCanvas(QSize(this->width(), this->height()));
}

QString calculateScore(std::vector<Robot> robots, std::vector<Particle> particles) {
	int totalScore = 0;
	int scoreLeft = 0;

	for (Robot r: robots) {
		totalScore += r.getScore();
	}
	for (Particle p: particles) {
		scoreLeft += p.getRadius() * p.getRadius() * M_PI;
	}
	QString ssl = QString::number(scoreLeft);
	QString sts = QString::number(totalScore);
	QString text = QString("%1 / %2").arg(sts, ssl);
	return text;
}
