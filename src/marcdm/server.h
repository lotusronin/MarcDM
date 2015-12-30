#ifndef SERVER_H
#define SERVER_H
#include <QProcess>
#include <QStringList>

class Server
{
	private:
	QProcess* xproc;
	public:
	Server();
	~Server();
	void setArgs();
	void startX();
	bool pollServer();
};



#endif //SERVER_H