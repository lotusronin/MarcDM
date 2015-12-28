#ifndef AUTH_H
#define AUTH_H

#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <QString>
#include <shadow.h>

class Auth {
	public:
	Auth();
	~Auth();
	int verifyUser(QString user, QString pass);
	void closeSession();
	void startSession(QString dSession);
	void endPam();
	void endPam(int laststatus);
    char** getEnv();
private:
	pam_handle_t *pamh;
	struct spwd* n;
};


#endif //AUTH_H
