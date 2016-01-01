#ifndef SESSIONAPP_H
#define SESSIONAPP_H

#include <QtCore/QCoreApplication>
#include <QtCore/QProcessEnvironment>
#include <pwd.h>
#include <string>

#include "Auth.h"



class SessionApp {
    public:
        SessionApp();
        ~SessionApp();
        int setup();
        int auth();
        void start();
        std::string getSessionCmnd();
    private:
        QString m_session;
        QString m_user;
        QString m_pass;
        Auth* m_auth;
        struct passwd* pwd;
};


#endif
