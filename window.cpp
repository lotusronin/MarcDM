#include "window.h"
#include "session.h"
#include <iostream>
#include <fstream>
#include <QProcessEnvironment>
#include <QPixmap>
#include <QImage>

Window::Window(QWidget *parent) : QWidget(parent)
{
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
	frame->setLayout(grid);

	de = new Session();
	settings->close();
}

Window::~Window()
{
	delete de;
}

void Window::update()
{
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
	bkg->setScaledContents(true);
	bkg->setPixmap(QPixmap::fromImage(QImage(bkgpath)).scaled(this->width(),this->height()));
	frame->setAutoFillBackground(true);
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
	/* FIXME!
	** Get Better User Authentication and add Password Auth. too.
	*/
	int auth = 0;
	QString username = ufield->text();
	QString pass = pfield->text();
	if(!username.isEmpty())
	{
		n = getspnam(username.toStdString().c_str());

		/*
		**	User Authentication
		*/
		if(n != NULL)
		{
			//std::cout << n->sp_namp << "\n";
			auth = 1;
		}
		else
		{
			endspent ();		/* stop access to shadow passwd file */
			std::cout << "Error, no user: " << username.toStdString() << "\n";
		}

		/*
		**	Password Authentication
		*/
		if(!pass.isEmpty() && auth == 1 && n != NULL)
		{
			std::string spass = n->sp_pwdp;
			std::string encpass = pw_encrypt(pass.toStdString().c_str(), spass.c_str());
			if(encpass.compare(spass) == 0)
			{
				std::cout << "Username authenticated\n";
				std::cout << "Password authenticated\n";
				auth++;
			}
			else
			{
				std::cout << "Login failed!\n";
			}
		}
		else
		{
			endspent ();		/* stop access to shadow passwd file */
			if(pass.isEmpty())
				std::cout << "Error, empty password.\n";
		}

		
	}
	else
	{
		std::cout << "Error, empty username.\n";
	}
	/*
	** FIXME!
	** Make sure all of this is correct.
	**
	*/

	if(auth == 2)
	{

		std::string name = username.toStdString();
		pwd = getpwnam(name.c_str());

		std::string shell = pwd->pw_shell;
		std::string home = pwd->pw_dir;
		
		QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
		env.insert("USER",name.c_str());
		env.insert("LOGNAME",name.c_str());
		env.insert("SHELL",shell.c_str());
		env.insert("HOME",home.c_str());
		env.insert("XAUTHORITY",(home+"/.Xauthoirty").c_str());
		de->setID(pwd->pw_uid, pwd->pw_gid);
		de->setProcessEnvironment(env);
		cleanup();
		startSession(readySession());
		pid_t pID = de->processId();
		this->hide();
		// This code should be in an onLogout() method
		int status;
		waitpid(pID, &status, 0);
		this->show();
	}
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
	QString program = QString::fromStdString(cmnd);
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

char *pw_encrypt (const char *clear, const char *salt)
{
	static char cipher[128];
	char *cp;

	cp = crypt (clear, salt);
	if (!cp) {
		/*
		 * Single Unix Spec: crypt() may return a null pointer,
		 * and set errno to indicate an error.  The caller doesn't
		 * expect us to return NULL, so...
		 */
		perror ("crypt");
		exit (1);
	}
	if (strlen (cp) != 13)
		return cp;	/* nonstandard crypt() in libc, better bail out */
	strcpy (cipher, cp);

	return cipher;
}