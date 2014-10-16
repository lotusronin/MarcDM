#include "window.h"
#include <iostream>
#include <fstream>

Window::Window(QWidget *parent) : QWidget(parent)
{
	frame = new QFrame(this);
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

	grid = new QGridLayout();
	grid->addWidget(ufield, 0, 0);
	grid->addWidget(pfield, 1, 0);
	grid->addWidget(sessions, 0, 1);
	grid->addWidget(button, 1, 1);
	grid->addWidget(powerops, 2, 1);
	getSessions();
	frame->setLayout(grid);
}

Window::~Window()
{}

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
}

void Window::getSessions()
{
	if((dir = opendir(session_path)) != NULL)
		{
			std::string s;
			while((ent = readdir(dir)) != NULL)
			{
				/* FIXME!
				** Add the Sessions in alphabetical order. Also make sure the executables exist.
				*/
				s = ent->d_name;
				std::size_t found = s.find(".desktop");
				if(found != std::string::npos)
					sessions->addItem(QString(s.substr(0,found).c_str()));
			}
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
	//std::cout << "Login Button pressed.\n";
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

	//void login(std::string name, std::string shell, std::string home)
	if(auth == 2)
	{

		std::string name = username.toStdString();
		pwd = getpwnam(name.c_str());

		std::string shell = pwd->pw_shell;
		std::string home = pwd->pw_dir;

		pid_t pID = vfork();
		//int pID = 0;
		if(pID == 0)
		{
			/* Have child set environment vars, change g/uid and run session.
			*/
			int res = 0;
			res = setenv("USER",name.c_str(),1);
			//std::cout << res << "\n";
			res = setenv("LOGNAME",name.c_str(),1);
			//std::cout << res << "\n";
			res = setenv("SHELL",shell.c_str(),1);
			//std::cout << res << "\n";
			res = setenv("HOME",home.c_str(),1);
			/*std::cout << res << "\n";
			std::cout << getenv("HOME") << "\n";*/
			res = setenv("XAUTHORITY",(home+"/.Xauthoirty").c_str(),1);
			std::cout << "Logged in!\n";
			std::cout << "User: " << name << "\nHome: " << home << "\nShell: " << shell << "\n";
			std::cout << "Setting guid and uid!\n";
			std::string cmnd = readySession();
			cleanup();
			if(setgid(pwd->pw_gid) || setuid(pwd->pw_uid))
			{	
				std::cout << "Error switching g/uids\n";
				exit(1);
			}
			chdir(home.c_str());
			startSession(cmnd);
			//exit(0);
		}
		this->window()->hide();
		
		// This code should be in an onLogout() method
		int status;
		waitpid(pID, &status, 0);
		this->window()->show();
	}
}

std::string Window::readySession()
{
	std::string fpath = session_path + sessions->currentText().toStdString() + ".desktop";
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
	/*std::string session = sessions->currentText().toStdString();
	//std::cout << "Session is: " << session << "\n";

	std::string fpath = session_path + session + ".desktop";
	std::string line;
	//std::cout << f << "\n";
	std::ifstream file;
	file.open(fpath);
	while(line.compare(0,5,"Exec=") != 0)
	{
		getline(file,line);
	}
	std::string cmnd = line.substr(5,line.length());*/

	/*
	**	Start X session here!
	*/
	std::cout << "Starting xsession: " << cmnd << "\n";
	char s[cmnd.length()+1]; 
	std::cout << "Made a char* s\n";
	cmnd.copy(s, cmnd.length(), 0);
	s[cmnd.length()] = NULL; 
	std::cout << "Copied " << cmnd << " to s\n";
	char* args[] = {s, (char*)0};
	std::cout << "made a list of arguements\n";
	//char* args[] = {"startlxqt", NULL};
	std::cout << "Beginning process now...\n";
	execvp(s, args);
	std::cerr << "There Was a BIG error...\n";
}

void Window::suspend()
{
	/*char *s[] = {"systemctl", "suspend", (char *) 0};
	execvp("systemctl", s);*/
	QProcess* p = new QProcess(this);
	p->start("systemctl suspend");
}

void Window::restart()
{
	/*char* null = NULL;
	char *s[] = {"systemctl", "reboot", null};
	execvp("systemctl", s);*/
	QProcess* p = new QProcess(this);
	p->start("systemctl reboot");
	exit(0);
}

void Window::shutdown()
{
	/*char* null = NULL;
	char *s[] = {"systemctl", "poweroff", null};
	execvp("systemctl", s);*/
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