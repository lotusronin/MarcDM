#include "session.h"
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <iostream>
#include <string>
#include <grp.h>

Session::Session() : QProcess() {
	
}

Session::~Session() {

}

void Session::setID(uid_t u, gid_t g) {
	uid = u;
	gid = g;
}

void Session::setName(char* n) {
	name = n;
}

void Session::setupChildProcess() {
	if(setgid(gid) || initgroups(name,gid) || setuid(uid))
	{	
		std::cout << "Error switching g/uids\n";
		exit(1);
	}
	chdir(this->processEnvironment().value("HOME").toStdString().c_str());
}