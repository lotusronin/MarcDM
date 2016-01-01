#include <QString>
#include <QStringList>
#include <QProcess>
#include <iostream>
#include <fstream>
#include "SessionApp.h"
#include "session.h"

SessionApp::SessionApp() 
{
    m_auth = new Auth();
}

SessionApp::~SessionApp() 
{
    delete m_auth;
}

int SessionApp::setup()
{
    QStringList args = QCoreApplication::arguments();
    int pos;
    int len = args.length()-1;

    pos = args.indexOf(QStringLiteral("--user"));
    if(pos >= 0 && pos < len) {
        m_user = args.at(pos+1);
    } else {
        std::cerr << "Error, no user specified\n";
        return -1;
    }

    pos = args.indexOf(QStringLiteral("--session"));
    if(pos >= 0 && pos < len) {
        m_session = args.at(pos+1);
    } else {
        std::cerr << "Error, no session given\n";
        return -1;
    }

    return 0;
}

int SessionApp::auth() {
    //TODO Get password from original app
    std::string s;
    std::cin >> s;
    //std::cout << s << "\n";
    //return 0;

    return m_auth->verifyUser(m_user,m_pass);
}

void SessionApp::start() {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    Session usersession;
    
    m_auth->startSession(m_session);
    std::string name = m_user.toStdString();
	pwd = getpwnam(name.c_str());
    std::string shell = pwd->pw_shell; 
    std::string home = pwd->pw_dir; 
	env.insert("USER",name.c_str());
	env.insert("LOGNAME",name.c_str());
	env.insert("SHELL",shell.c_str());
	env.insert("HOME",home.c_str());
	//env.insert("XAUTHORITY",(home+"/.Xauthoirty").c_str());
	env.insert("DESKTOP_SESSION",m_session.toStdString().c_str());
	env.insert("XDG_SESSION_CLASS","user");
    
    usersession.setID(pwd->pw_uid, pwd->pw_gid);
	usersession.setName(pwd->pw_name);
	usersession.setProcessEnvironment(env);

    QString program = QString::fromStdString("/etc/marcdm/Xsession "+getSessionCmnd());
    usersession.start(program);
    usersession.waitForFinished();

}

std::string SessionApp::getSessionCmnd() {
    std::string fpath;
	std::string line;
	std::ifstream file;
	
	fpath = "/usr/share/xsessions/" + m_session.toStdString() + ".desktop";
	file.open(fpath);
	while(line.compare(0,5,"Exec=") != 0)
	{
		getline(file,line);
	}
    file.close();
	std::string cmnd = line.substr(5,line.length());
	return cmnd;
}
