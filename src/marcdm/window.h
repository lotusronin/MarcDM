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

//pam
#include <security/pam_appl.h>
#include <security/pam_misc.h>

//Forking requirements
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <crypt.h>
#include <sys/wait.h>
#include "session.h"
#include "settings.h"
#include "auth.h"


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
    QString defUser;
	Auth* authenticator;
    pam_handle_t* pam_handle;
	struct passwd* marcdm_pwd;

	//struct spwd* n;
	struct passwd* pwd;
	const char * session_path = "/usr/share/xsessions/";
	DIR *dir;
	struct dirent *ent;
	bool hdpi;

	void getSessions();
	QString getUserSession();
	void suspend();
	void restart();
	void shutdown();
	void cleanup();
protected:
	void onLogin();

public:
	Window(QWidget *parent = 0);
	void isHiDPI(bool hidpi);
	~Window();
	void update();
};

bool sessionExists(std::string file);

#endif //WINDOW_H
