#include "session.h"
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <iostream>
#include <string>

Session::Session() : QProcess() {
	
}

Session::~Session() {

}

void Session::setID(uid_t u, gid_t g) {
	uid = u;
	gid = g;
}

void Session::setupChildProcess() {
	if(setgid(gid) || setuid(uid))
	{	
		std::cout << "Error switching g/uids\n";
		exit(1);
	}
	chdir(this->processEnvironment().value("HOME").toStdString().c_str());
}