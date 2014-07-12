#include <QApplication>
#include <QWidget>
#include <iostream>
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

	//startX();

	QApplication app(argc, argv);
	Window window;

	window.resize(250, 150);
	window.setWindowTitle("My Custom DM");
	//window.setWindowFlags(Qt::FramelessWindowHint);
	window.setStyleSheet("background-image: url(/home/marcus/Pictures/splendor_by_mio188-d415zcq.png)");
	//window.showFullScreen();
	window.show();

	return app.exec();
}


/*
** FIXME!
** Need to get the DM to start X before running the rest of the program.
**
*/
void startX()
{
	pid_t pID = vfork();

	switch(pID)
	{
		case 0:
		break;
	}
}