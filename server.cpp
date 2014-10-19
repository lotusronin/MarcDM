#include "server.h"
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <iostream>
#include <xcb/xcb.h>

Server::Server()
{
	xproc = new QProcess();
}

Server::~Server()
{
	delete xproc;
}

void Server::setArgs()
{}

/*
** FIXME!
** This needs to be more flexible for other possible configurations.
*/
void Server::startX()
{
	QStringList args;
	args << ":0" << "-background" << "none" << "-noreset" << "-verbose" << "3" << "-logfile" << "/dev/null" << "-seat" << "seat0" << "-nolisten" << "tcp" << "vt1" << "-novtswitch";
	xproc->start("Xorg",args);
	
	/*pid_t pID = vfork();
	if(pID == 0)
	{
		char* xexec[] = {"Xorg",":0","-background","none","-noreset","-verbose","3","-logfile","/dev/null","-seat","seat0","-nolisten","tcp","vt1", "-novtswitch", NULL};
		execvp("Xorg", xexec);
		std::cout << "Error starting X!!!!\n";
		exit(0);
	}*/
	setenv("DISPLAY",":0",1);
}

bool Server::pollServer() {
	xcb_connection_t* connection = xcb_connect(NULL, NULL);
	int res = xcb_connection_has_error(connection);
	xcb_disconnect(connection);
	return (res == 0);
}