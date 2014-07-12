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
#include <iostream>
#include <shadow.h>
#include <dirent.h>
#include <string>
//Forking requirements
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

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

	struct spwd* n;
	const char * session_path = "/usr/share/xsessions/";
	DIR *dir;
	struct dirent *ent;

	void getSessions();
	QString getUserSession();
	void suspend();
	void restart();
	void shutdown();

protected:
	void onLogin();

public:
	Window(QWidget *parent = 0);
	~Window();
};

#endif //WINDOW_H