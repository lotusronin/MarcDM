#ifndef SESSION_H
#define SESSION_H

#include <QProcess>
#include <QString>
#include <QStringList>
#include <QObject>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>

class Session : public QProcess {
//Q_OBJECT
public:
	Session();
	~Session();
	void setID(uid_t u, gid_t g);

protected:
	void setupChildProcess();

private:
	gid_t gid;
	uid_t uid;
};

#endif //SESSION_H