#include "window.h"

Window::Window(QWidget *parent) : QWidget(parent)
{
	ufield = new QLineEdit(this);
	pfield = new QLineEdit(this);
	button = new QPushButton(this);
	sessions = new QComboBox(this);
	powerops = new QToolButton(this);
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
	setLayout(grid);
}

Window::~Window()
{}

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
	std::cout << "Login Button pressed.\n";
	QString username = ufield->text();
	if(!username.isEmpty())
	{
		n = getspnam(username.toStdString().c_str());

		if(n != NULL)
		{
			std::cout << n->sp_namp << "\n";
		}
		else
		{
			std::cout << "Error, no user: " << username.toStdString() << "\n";
		}
	}
	else
	{
		std::cout << "Error, empty username.\n";
	}
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
	char* null = NULL;
	char *s[] = {"systemctl", "reboot", null};
	execvp("systemctl", s);
	exit(0);
}

void Window::shutdown()
{
	char* null = NULL;
	char *s[] = {"systemctl", "poweroff", null};
	execvp("systemctl", s);
	exit(0);
}