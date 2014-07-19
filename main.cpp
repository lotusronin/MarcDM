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

//QProcess xproc;
Server server;
void startX();
int Xisready();
//Display* dpy xconn_open(const char *display);

int main(int argc, char** argv)
{
	
	uid_t uID = getuid();

	if(uID != 0)
	{
		std::cout << "You are not running as root, this will cause problems.\n";
	}

	server.startX();
	usleep(5*1000);
	//startX();

	QApplication app(argc, argv);
	Window window;

	//window.resize(250, 150);
	window.resize(app.primaryScreen()->size());
	//window.resize(dwidth, dheight);
	//window.setWindowTitle("My Custom DM");
	window.setWindowFlags(Qt::FramelessWindowHint);
	//window.setStyleSheet("background-image: url(/home/marcus/Pictures/splendor_by_mio188-d415zcq.png)");
	//window.showFullScreen();
	window.show();

	return app.exec();
}


/*
** FIXME!
** Need to get the DM to start X before running the rest of the program.
** This needs to be cleaned up and more flexible for other possible configurations.
*/
/*void startX()
{
	/*pid_t pID = vfork();
	if(pID == 0)
	{
		char* xexec = "Xorg :0 -background none -noreset -verbose 3 -logfile /dev/null -seat seat0 -nolisten tcp vt1";
		execvp("Xorg", xexec);
		std::cout << "Error starting X!!!!\n";
		exit(0);
	}

	Display* dpy;
	for(int i = 0; i < 100; i++)
	{
		dpy = XOpenDisplay(":0.0"); 
		if(dpy != NULL)
			break;
		usleep(5*1000);
	}
	if(dpy == NULL)
	{
		std::cout << "Error Starting X!!!\n";
	}*/
	//xproc.start("Xorg :0 -background none -noreset -verbose 3 -logfile /dev/null -seat seat0 -nolisten tcp vt1");
	//Display* dpy;
	/*for(int i = 0; i < 100; i++)
	{
		/*dpy = xconn_open(":0.0"); 
		if(dpy != NULL)
			break;
		usleep(5*1000);
	}
	if(dpy == NULL)
	{
		std::cout << "Error Starting X!!!\n";
	}
	std::cout << "X has been started!\n";*/
	/*usleep(5*1000);
}*/

/*
Display* dpy xconn_open(const char *display)
{
	return XOpenDisplay(display);
}*/