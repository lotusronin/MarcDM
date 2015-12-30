#ifndef SETTINGS_H
#define SETTINGS_H
#include <string>
#include <fstream>
#include <QString>

class Settings {
	public:
	Settings();
	~Settings();
	void load();
	void close();
	QString getValue(std::string key);
private:
	std::fstream f;
};


#endif //SETTINGS_H