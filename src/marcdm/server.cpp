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
	setenv("DISPLAY",":0",1);
	setenv("XDG_SEAT","seat0",1);
	setenv("XDG_VTNR","1",1);
	setenv("XDG_SESSION_CLASS","greeter",1);
	setenv("XDG_SESSION_TYPE","x11",1);
}

bool Server::pollServer() {
	xcb_connection_t* connection = xcb_connect(NULL, NULL);
	int res = xcb_connection_has_error(connection);
	xcb_disconnect(connection);
	return (res == 0);
}
