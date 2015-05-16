#include "window.h"
#include "session.h"
#include <iostream>
#include <fstream>
#include <QProcessEnvironment>
#include <QPixmap>
#include <QImage>
#include <unistd.h>

Window::Window(QWidget *parent) : QWidget(parent)
{
	hdpi = false;
	hasChild = false;
	settings = new Settings();
	settings->load();
	bkg = new QLabel(this);
	frame = new QFrame(this);
	frame->setFrameStyle(QFrame::StyledPanel);
	ufield = new QLineEdit(frame);
	pfield = new QLineEdit(frame);
	button = new QPushButton(frame);
	sessions = new QComboBox(frame);
	powerops = new QToolButton(frame);
	powericon = new QIcon("/usr/share/icons/gnome/32x32/actions/system-shutdown.png");
	powermenu = new QMenu();
	PowerMenuActions[0] = new QAction("Suspend", this);
	PowerMenuActions[1] = new QAction("Restart", this);
	PowerMenuActions[2] = new QAction("Shutdown", this);

	pfield->setEchoMode(QLineEdit::Password);
	button->setText("Login");
	powerops->setIcon((*powericon));
	powermenu->addAction(PowerMenuActions[0]);
	powermenu->addAction(PowerMenuActions[1]);
	powermenu->addAction(PowerMenuActions[2]);
	powerops->setMenu(powermenu);
	powerops->setPopupMode(QToolButton::InstantPopup);

	connect(button, &QPushButton::clicked, this, &Window::onLogin);
	connect(PowerMenuActions[0], &QAction::triggered, this, &Window::suspend);
	connect(PowerMenuActions[1], &QAction::triggered, this, &Window::restart);
	connect(PowerMenuActions[2], &QAction::triggered, this, &Window::shutdown);

	connect(pfield, &QLineEdit::returnPressed, this, &Window::onLogin);

	grid = new QGridLayout();
	grid->addWidget(ufield, 0, 0);
	grid->addWidget(pfield, 1, 0);
	grid->addWidget(sessions, 0, 1);
	grid->addWidget(button, 1, 1);
	grid->addWidget(powerops, 2, 1);
	getSessions();
    defUser = settings->getValue("default_user");
    if(!defUser.isNull()) {
        ufield->setText(defUser);
    }
	frame->setLayout(grid);

	de = new Session();
	settings->close();
	authenticator = new Auth();
}

Window::~Window()
{
	delete de;
}

void Window::update()
{
	/* HiDPI
	** This is a hack to make the widgets be better
	** visibile on hidpi screens. When/if Qt5 gets
	** native support for hidpi, we should swap to
	** that.
	*/
	double mult = 1;
	if(hdpi) {
		mult = 1.5;
	}
	frame->setMinimumSize(frame->size()*mult);
	ufield->setMinimumSize(ufield->size()*mult);
	pfield->setMinimumSize(pfield->size()*mult);
	button->setMinimumSize(button->size()*mult);
	sessions->setMinimumSize(sessions->size()*mult);
	powerops->setMinimumSize(powerops->size()*mult);
	/*
	** End hidpi hackery
	*/

	QPoint temp = this->geometry().center();
	QSize temp2 = frame->size();
	std::cout << temp2.height() << " " << temp2.width() << "\n";
	std::cout << temp.x() << " " << temp.y() << "\n";
	temp.setX(temp.rx()-temp2.width());
	temp.setY(temp.ry()-temp2.height());
	std::cout << temp.x() << " " << temp.y() << "\n";
	frame->move(temp);
	settings->load();
	QString bkgpath = settings->getValue("background");
	//grid->addWidget(new QLabel(bkgpath),2,0);
	if(!bkgpath.isNull() && !bkgpath.isEmpty())	{
		bkg->setScaledContents(true);
		bkg->setPixmap(QPixmap::fromImage(QImage(bkgpath)).scaled(this->width(),this->height()));
		frame->setAutoFillBackground(true);
	}
    if(!defUser.isNull()){
        pfield->setFocus();
    } else {
	    ufield->setFocus();
    }
	settings->close();
}

void Window::getSessions()
{
	QString def = settings->getValue("default_session");
	if((dir = opendir(session_path)) != NULL)
		{
			std::string s;
			QStringList list;
			while((ent = readdir(dir)) != NULL)
			{
				/* FIXME!
				** Make sure the executables exist.
				*/
				s = ent->d_name;
				std::size_t found = s.find(".desktop");
				if(found != std::string::npos)
					list << s.substr(0,found).c_str();
			}
			list.sort();
			if(!def.isNull()) {
				defSession = def;
				list.prepend(QString("default"));
			}
			sessions->addItems(list);
			closedir(dir);
		}
}

QString Window::getUserSession()
{
	return sessions->currentText();
}

void Window::onLogin()
{
	pid_t pid = fork();
	if(!pid) {
		return;
	}
	/* FIXME!
	** Get Better User Authentication.
	*/
	int auth = 0;
	QString username = ufield->text();
	QString pass = pfield->text();

	/*
	 * PAM authentication in progress!
	 * Correctly identifies the user!!
	 */

	auth = authenticator->verifyUser(username,pass);

	
	/*
	** FIXME!
	** Make sure all of this is correct.
	**
	*/

	if(auth == 2)
	{
		std::cout << "USER LOGGED IN!!!\n";
		
		std::string name = username.toStdString();
		pwd = getpwnam(name.c_str());

		std::string shell = pwd->pw_shell;
		std::string home = pwd->pw_dir;
		
		QString sessionVal = sessions->currentText();
		std::string desktop_session = sessionVal.toStdString();
		if(desktop_session.compare("default") == 0) {
			desktop_session = defSession.toStdString();
			authenticator->startSession(defSession);
		}
		else {
			authenticator->startSession(sessionVal);
		}

		QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
		env.insert("USER",name.c_str());
		env.insert("LOGNAME",name.c_str());
		env.insert("SHELL",shell.c_str());
		env.insert("HOME",home.c_str());
		env.insert("XAUTHORITY",(home+"/.Xauthoirty").c_str());
		env.insert("DESKTOP_SESSION",desktop_session.c_str());
		de->setID(pwd->pw_uid, pwd->pw_gid);
		de->setName(pwd->pw_name);
		de->setProcessEnvironment(env);
		//setsid();
		startSession(readySession());
		pid_t pID = de->processId();
		cleanup();
		this->hide();
		// This code should be in an onLogout() method
		int status;
		waitpid(pID, &status, 0);
		this->show();
		authenticator->closeSession();
	}
	authenticator->endPam();
	exit(0);

}

std::string Window::readySession()
{
	std::string fpath;
	if(sessions->currentText().toStdString().compare("default") == 0)
	{
		fpath = session_path + defSession.toStdString()+".desktop";
	}
	else {
		fpath = session_path + sessions->currentText().toStdString() + ".desktop";
	}
	std::string line;
	std::ifstream file;
	
	file.open(fpath);
	while(line.compare(0,5,"Exec=") != 0)
	{
		getline(file,line);
	}
	std::string cmnd = line.substr(5,line.length());
	return cmnd;
}

void Window::startSession(std::string cmnd)
{
	/*
	**	Start X session here!
	*/
	QString program = QString::fromStdString("/etc/marcdm/Xsession "+cmnd);
	de->start(program);
}

void Window::suspend()
{
	QProcess* p = new QProcess(this);
	p->start("systemctl suspend");
}

void Window::restart()
{
	QProcess* p = new QProcess(this);
	p->start("systemctl reboot");
	exit(0);
}

void Window::shutdown()
{
	QProcess* p = new QProcess(this);
	p->start("systemctl poweroff");
	exit(0);
}

void Window::cleanup()
{
	endpwent ();		/* stop access to password file */
	endspent ();		/* stop access to shadow passwd file */
}

void Window::isHiDPI(bool hidpi) {
	hdpi = hidpi;
}
