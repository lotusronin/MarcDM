#include "Auth.h"
#include <iostream>

static char* up_vals[2];

static int pam_conv_func(int num_msg, const struct pam_message **msg, struct pam_response **resp, void* data) {
	int retval = PAM_SUCCESS;
	int m;
	*resp = (struct pam_response*) calloc(num_msg, sizeof(struct pam_response));
	for(int i = 0; i<num_msg;i++) {
		m = msg[i]->msg_style;
		if(m == PAM_PROMPT_ECHO_ON) {
			if(up_vals[0] != NULL) {
				(*resp)[i].resp = strdup(up_vals[0]);
			} else {
				(*resp)[i].resp = strdup("");
			}
		} else if (m == PAM_PROMPT_ECHO_OFF) {
			if(up_vals[1] != NULL) {
				(*resp)[i].resp = strdup(up_vals[1]);
			} else {
				(*resp)[i].resp = NULL;
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
		std::cout << "Ending old pam session\n";
		pam_end(pamh,0);
	}
	retcode = pam_start("marcdm", (char*)user.toStdString().c_str(), &conv, &pamh);
	
	if(retcode != PAM_SUCCESS) {
        endPam(retcode);
		std::cout << "Error Starting PAM!!!!\n";
		return -1;
	}

	pam_misc_setenv(pamh,"XDG_SESSION_CLASS","user",0);
	pam_misc_setenv(pamh,"XDG_SEAT","seat0",0);
    
    up_vals[0] = strdup(user.toStdString().c_str());
	up_vals[1] = strdup(pass.toStdString().c_str());
	retcode = pam_authenticate(pamh, 0);
	up_vals[0] = 0;
	up_vals[1] = 0;
	if(retcode != PAM_SUCCESS) {
		std::cout << "Error, could not authenticate user.\n";
		fprintf(stderr,"\n%s\n",pam_strerror(pamh, retcode));
        endPam(retcode);
		retval = 0;
        return 0;
	}
	
    retcode = pam_acct_mgmt(pamh, retcode);
	if(retcode != PAM_SUCCESS) {
		std::cout << "Error, acct mgmt failed.\n";
		fprintf(stderr,"\n%s\n",pam_strerror(pamh, retcode));
        endPam(retcode);
	} else {
		retval = 2;
		std::cout << "User " << user.toStdString() << " <HACKING> is a real user\n";
	}

	return retval;
}

void Auth::startSession(QString dSession) {
	int ret;
	if(pamh) {
		std::cout << "Starting Session: " << dSession.toStdString() << "\n";
		ret = pam_open_session(pamh,0);
		if(ret != PAM_SUCCESS) {
            endPam(ret);
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

void Auth::endPam() {
	pam_end(pamh, 0);
}

void Auth::endPam(int laststatus) {
	pam_end(pamh, laststatus);
    pamh = NULL;
}

char** Auth::getEnv() {
    return pam_getenvlist(pamh);
}

