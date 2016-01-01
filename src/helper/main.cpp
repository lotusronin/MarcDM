#include <QCoreApplication>
#include <QProcess>
#include <iostream>
//Forking requirements
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

#include "SessionApp.h"


int main(int argc, char** argv)
{
	QCoreApplication app(argc, argv);
    SessionApp session;
    
    if(session.setup()) {
        return -1;
    }

    if(session.auth() != 2) {
        return -1;
    }

    session.start();
	return 0;
}
