#include "server.h"
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <iostream>

Server::Server()
{
}

Server::~Server()
{}

void Server::setArgs()
{}

void Server::startX()
{
	pid_t pID = vfork();
	if(pID == 0)
	{
		char* xexec[] = {"Xorg",":0","-background","none","-noreset","-verbose","3","-logfile","/dev/null","-seat","seat0","-nolisten","tcp","vt1", "-novtswitch", NULL};
		execvp("Xorg", xexec);
		std::cout << "Error starting X!!!!\n";
		exit(0);
	}
	int res = 0;
	res = setenv("DISPLAY",":0",1);
}