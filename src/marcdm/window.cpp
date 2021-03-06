#include "window.h"
#include "session.h"
#include <iostream>
#include <fstream>
#include <QProcessEnvironment>
#include <QPixmap>
#include <QImage>
#include <unistd.h>
#include <grp.h>

static char* userpass[] = {"marcdm",NULL};

static int conv_func(int num_msg, const struct pam_message **msg, struct pam_response **resp, void* data) {
	int retval = PAM_SUCCESS;
	int m;
	*resp = (struct pam_response*) calloc(num_msg, sizeof(struct pam_response));
	for(int i = 0; i<num_msg;i++) {
		m = msg[i]->msg_style;
		if(m == PAM_PROMPT_ECHO_ON) {
			if(userpass[0] != NULL) {
				(*resp)[i].resp = strdup(userpass[0]);
			} else {
				(*resp)[i].resp = strdup("");
			}
		} else if (m == PAM_PROMPT_ECHO_OFF) {
			if(userpass[1] != NULL) {
				(*resp)[i].resp = strdup(userpass[1]);
			} else {
				(*resp)[i].resp = NULL;
			}
		}
	}
	return retval;
}

static struct pam_conv conv = {
	.conv=conv_func,
	.appdata_ptr = userpass
};

Window::Window(QWidget *parent) : QWidget(parent)
{
	hdpi = false;
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

	//de = new Session();
	settings->close();
	authenticator = new Auth();
    
    struct passwd *marcdm_pwd = getpwnam("marcdm");
    if(setgid(marcdm_pwd->pw_gid) || initgroups(marcdm_pwd->pw_name,marcdm_pwd->pw_gid) || setuid(marcdm_pwd->pw_uid)) {
        std::cerr << "Error setting group and user id for marcdm\n";
        std::cerr << "UHOH!!!\n";
        std::cerr << "gid: "<< marcdm_pwd->pw_gid << " uid: " << marcdm_pwd->pw_uid << "\n";
    }
    int retcode = pam_start("marcdm", "marcdm", &conv, &pam_handle);
    //pam_misc_setenv(pamh,"XDG_SESSION_CLASS","greeter",0);
}

Window::~Window()
{
    pam_end(pam_handle, 0);
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
				s = ent->d_name;
				std::size_t found = s.find(".desktop");
                std::string path(session_path);
				if(found != std::string::npos && sessionExists(path+s))
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
	QString username = ufield->text();
	QString pass = pfield->text();

	
    /*
     * Start helper app
     */
    QString sessionVal = sessions->currentText();
    std::string desktop_session = sessionVal.toStdString();
    if(desktop_session.compare("default") == 0) {
		desktop_session = defSession.toStdString();
	}

	//pwd = getpwnam(username.toStdString().c_str());
    QProcess* proc = new QProcess();
    proc->setProcessChannelMode(QProcess::ForwardedErrorChannel);
	//de->setID(pwd->pw_uid, pwd->pw_gid);
	QString program = QString::fromStdString("/usr/bin/marcdm-helper --user "+username.toStdString()+" --session "+desktop_session);
	proc->start(program);
    if(!proc->waitForStarted()) {
        std::cerr << "process hasn't started!\n";
    }
    std::cerr << "Command being run is: " << program.toStdString() << "\n";
    proc->write(pass.toUtf8());
    proc->closeWriteChannel();
    proc->waitForBytesWritten();
	/*pid_t pID = proc->processId();*/
	cleanup();
	this->hide();
    /*int status;
    waitpid(pID, &status, 0);*/
    std::cerr << "Finished: " << proc->waitForFinished(-1) << "\n";
    this->show();

#if 0
	/*
	 * PAM authentication in progress!
	 * Correctly identifies the user!!
	 */

	int auth = authenticator->verifyUser(username,pass);

	
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
		env.insert("XDG_SESSION_CLASS","user");
        /*char** penv = authenticator->getEnv();
        if(penv) {
            std::cout << "Printing out environment vars from pam\n";
            for(int i = 0; penv[i] != nullptr; i++) {
                QString temp(penv[i]);
                std::cout << temp.toStdString() << "\n";
                int idx = temp.indexOf('=');
                if(idx != -1) {
                    env.insert(temp.left(idx), temp.mid(idx+1));
                }
                free(penv[i]);
            }
            free(penv);
        }*/
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
	    authenticator->endPam(0);
	}
#endif
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
}

void Window::isHiDPI(bool hidpi) {
	hdpi = hidpi;
}

bool sessionExists(std::string file) {
	std::string line;
	std::string cmnd;
	std::ifstream f;
	
	f.open(file);
	while(getline(f,line))
	{
	    if(line.compare(0,8,"TryExec=") == 0) {
            cmnd = line.substr(8,line.length());
            std::ifstream file_exists(cmnd.c_str());
            std::string shell_cmd = "which "+cmnd;
            return file_exists.is_open() || system(shell_cmd.c_str()) == 0;
        }
	}

    //Assume true if we don't find TryExec field in the desktop file
	return true;
}
