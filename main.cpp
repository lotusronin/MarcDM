#include <QApplication>
#include <QWidget>
#include <QLineEdit>
#include <QGridLayout>
#include <QPushButton>
#include <QComboBox>
#include <iostream>
#include <shadow.h>
#include <dirent.h>
#include <string>
//Forking requirements
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

#include "window.h"


void startX();
int Xisready();

int main(int argc, char** argv)
{
	
	uid_t uID = getuid();

	if(uID != 0)
	{
		std::cout << "You are not running as root, this will cause problems.\n";
	}

	QApplication app(argc, argv);
	Window window;
	//QWidget window;
	//QLineEdit field = QLineEdit(&window);

	//field.setEchoMode(QLineEdit::Password);

	window.resize(250, 150);
	window.setWindowTitle("My Custom DM");
	window.setStyleSheet("background-image: url(/home/marcus/Pictures/splendor_by_mio188-d415zcq.png)");
	//window.showFullScreen();
	window.show();
	//field.show();

	return app.exec();
}

void startX()
{
	pid_t pID = vfork();

	switch(pID)
	{
		case 0:
		break;
	}
}