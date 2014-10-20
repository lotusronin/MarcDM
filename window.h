#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QGridLayout>
#include <QPushButton>
#include <QComboBox>
#include <QToolButton>
#include <QIcon>
#include <QMenu>
#include <QProcess>
#include <QFrame>
#include <QLabel>
#include <iostream>
#include <shadow.h>
#include <dirent.h>
#include <string>
#include <pwd.h>
//Forking requirements
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <crypt.h>
#include <sys/wait.h>
#include "session.h"
#include "settings.h"

char *pw_encrypt (const char *clear, const char *salt);

class Window : public QWidget
{
private:
	QLineEdit* ufield;
	QLineEdit* pfield;
	QGridLayout* grid;
	QPushButton* button;
	QComboBox* sessions;
	QToolButton* powerops;
	QIcon* powericon;
	QMenu* powermenu;
	QAction* PowerMenuActions[3];
	QFrame* frame;
	QLabel* bkg;
	Session* de;
	Settings* settings;
	QString defSession;

	struct spwd* n;
	struct passwd* pwd;
	const char * session_path = "/usr/share/xsessions/";
	DIR *dir;
	struct dirent *ent;

	void getSessions();
	std::string readySession();
	QString getUserSession();
	void startSession(std::string session);
	void suspend();
	void restart();
	void shutdown();
	void cleanup();
protected:
	void onLogin();

public:
	Window(QWidget *parent = 0);
	~Window();
	void update();
};

#endif //WINDOW_H