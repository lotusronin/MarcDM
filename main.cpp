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
#include <QFont>

Server server;

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
	double ppi = app.primaryScreen()->physicalDotsPerInch();
	std::cout << "Your ppi is " << ppi << "\n";
	Window window;
	if(ppi >= 200.00) {
		/* HiDPI
		** some hidpi font setting hack
		*/
		window.isHiDPI(true);
		QFont font = app.font();
		font.setPointSize(font.pointSize()*2);
		app.setFont(font);
	}
	window.resize(app.primaryScreen()->size());
	window.setWindowFlags(Qt::FramelessWindowHint);
	window.update();
	window.show();

	return app.exec();
}
