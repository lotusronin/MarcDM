#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QGridLayout>
#include <QPushButton>
#include <QComboBox>
#include <iostream>
#include <shadow.h>
#include <dirent.h>
#include <string>
//Forking requirements
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

char * session_path = "/usr/share/xsessions/";
DIR *dir;
struct dirent *ent;

class Window : public QWidget
{
private:
	QLineEdit* ufield;
	QLineEdit* pfield;
	QGridLayout * grid;
	QPushButton * button;
	QComboBox * sessions;
	struct spwd* n;
	void getSessions()
	{
		if((dir = opendir(session_path)) != NULL)
		{
			std::string s;
			while((ent = readdir(dir)) != NULL)
			{
				s = ent->d_name;
				std::size_t found = s.find(".desktop");
				if(found != std::string::npos)
					sessions->addItem(QString(s.substr(0,found).c_str()));
			}
			closedir(dir);
		}
	}

	QString getUserSession()
	{
		return sessions->currentText();
	}

protected:
	void onLogin()
	{
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

public:
	Window(QWidget *parent = 0) : QWidget(parent)
	{
		ufield = new QLineEdit(this);
		pfield = new QLineEdit(this);
		button = new QPushButton(this);
		sessions = new QComboBox(this);

		pfield->setEchoMode(QLineEdit::Password);
		button->setText("Login");

		connect(button, &QPushButton::clicked, this, &Window::onLogin);

		grid = new QGridLayout();
		grid->addWidget(ufield, 0, 0);
		grid->addWidget(pfield, 1, 0);
		grid->addWidget(sessions, 0, 1);
		grid->addWidget(button, 1, 1);
		getSessions();
		setLayout(grid);

	}

	~Window();
};

#endif //WINDOW_H