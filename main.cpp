#include <QApplication>
#include <QWidget>
#include <QProcess>
#include <QScreen>
#include <iostream>
//Forking requirements
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

#include "window.h"
#include "server.h"

Server server;
void startX();
int Xisready();

int main(int argc, char** argv)
{
	
	uid_t uID = getuid();

	if(uID != 0)
	{
		std::cout << "You are not running as root, this will cause problems.\n";
	}

	server.startX();
	while(!server.pollServer()) {
		usleep(5*1000);
		//sleep(1);
	}

	QApplication app(argc, argv);
	Window window;

	window.resize(app.primaryScreen()->size());
	//window.resize(dwidth, dheight);
	//window.setWindowTitle("My Custom DM");
	window.setWindowFlags(Qt::FramelessWindowHint);
	window.update();
	//window.setStyleSheet("background-image: url(/home/marcus/Pictures/splendor_by_mio188-d415zcq.png)");
	//window.showFullScreen();
	window.show();

	return app.exec();
}