#include "auth.h"
#include <iostream>

static char* up_vals[2];

static int pam_conv_func(int num_msg, const struct pam_message **msg, struct pam_response **resp, void* data) {
	int retval = PAM_SUCCESS;
	int m;
	//std::cout << "Function is called!!!\n";
	/*if((*msg)!= NULL)
		std::cout << (*msg)->msg << "\nMessage done\n";*/
	*resp = (struct pam_response*) calloc(num_msg, sizeof(struct pam_response));
	for(int i = 0; i<num_msg;i++) {
		m = msg[i]->msg_style;
		if(m == PAM_PROMPT_ECHO_ON) {
			if(up_vals[0] != NULL) {
				(*resp)[i].resp = strdup(up_vals[0]);
				//std::cout << "PAM_PROMPT_ECHO_ON!!!!\n";
			} else {
				(*resp)[i].resp = strdup("");
				//std::cout << "PAM_PROMPT_ECHO_ON 2!!!!\n";
			}
		} else if (m == PAM_PROMPT_ECHO_OFF) {
			if(up_vals[1] != NULL) {
				(*resp)[i].resp = strdup(up_vals[1]);
				//std::cout << "PAM_PROMPT_ECHO_OFF!!!!\n";
				//std::cout << up_vals[1] << "\n";
				//std::cout << up_vals[0] << "\n";
			} else {
				(*resp)[i].resp = NULL;
				//std::cout << "PAM_PROMPT_ECHO_OFF 2!!!!\n";
			}
		}
	}
	return retval;
}

static struct pam_conv conv = {
	.conv=pam_conv_func,
	.appdata_ptr = up_vals
};


Auth::Auth() {
	pamh = NULL;
}

Auth::~Auth() {
}

int Auth::verifyUser(QString user, QString pass) {
	int retcode = 0;
	int retval = 0;
	if(pamh) {
		/* 
		 * Fix me!
		 * call to pam_end crashes progam if login is pressed twiced.
		 */
		std::cout << "Ending old pam session\n";
		pam_end(pamh,0);
	}
	retcode = pam_start("marcdm", (char*)user.toStdString().c_str(), &conv, &pamh);
	
	if(retcode != PAM_SUCCESS) {
		std::cout << "Error Starting PAM!!!!\n";
		pamh = NULL;
		return -1;
	}

	if(retcode == PAM_SUCCESS) {
		/* This really is a user, check password*/ 
		//std::cout << user.toStdString() << " Is Is Is " << pass.toStdString() << std::endl;
		
		/*
		 * There was a strange bug where up_vals contained a pointer to the same string in memory.
		 * I assume there is a bug with QString.toStdString(), perhaps All QStrings for a certain
		 * application have a single spot where a std::String equivalent is placed when this method
		 * is called. Fixed by using strdup().
		 */
		up_vals[0] = strdup(user.toStdString().c_str());
		up_vals[1] = strdup(pass.toStdString().c_str());
		/*std::cout << "up_vals[0] = " << up_vals[0] << "\n";
		std::cout << "up_vals[1] = " << up_vals[1] << "\n";*/
		retcode = pam_authenticate(pamh, 0);
		up_vals[0] = 0;
		up_vals[1] = 0;
		if(retcode != PAM_SUCCESS) {
			std::cout << "Error, could not authenticate user.\n";
			fprintf(stderr,"\n%s\n",pam_strerror(pamh, retcode));
			retval = 0;
		}
		else {
			retcode = pam_acct_mgmt(pamh, retcode);
			if(retcode != PAM_SUCCESS) {
				std::cout << "Error, acct mgmt failed.\n";
				fprintf(stderr,"\n%s\n",pam_strerror(pamh, retcode));
			}
			else {
				retval = 2;
				std::cout << "User " << user.toStdString() << " <HACKING> is a real user\n";
			}
		}
	}
	else {
		std::cout << "Error, not a user!!!!\n";
	}

	return retval;
}

void Auth::startSession(QString dSession) {
	int ret;
	if(pamh) {
		std::cout << "Starting Session: " << dSession.toStdString() << "\n";
		ret = pam_open_session(pamh,0);
		if(ret != PAM_SUCCESS) {
			std::cout << "There was an error opening pam session!!!\n";
		}
	} else {
		std::cout << "No valid pamh!!!\n";
	}
}

void Auth::closeSession() {
	int ret = 0;
	if(pamh) {
		ret = pam_close_session(pamh, 0);
	}
	if(ret != PAM_SUCCESS) {
		std::cout << "There was an error closing pam session!!!\n";
	}

	ret = pam_end(pamh, ret);
	if(ret != PAM_SUCCESS) {
		std::cout << "There was an error closing PAM!!!\n";
	}
	pamh = NULL;
}


int Auth::legacyAuth(QString user, QString pass) {
	int auth = 0;
	if(!user.isEmpty())
	{
		n = getspnam(user.toStdString().c_str());

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
			std::cout << "Error, no user: " << user.toStdString() << "\n";
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

	return auth;
}

char* Auth::pw_encrypt (const char *clear, const char *salt)
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