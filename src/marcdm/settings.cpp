#include "settings.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <QString>

Settings::Settings() {
}

Settings::~Settings() {
}

void Settings::load() {
	f.open("/etc/marcdm/marcdm.conf");
}

void Settings::close() {
	f.close();
}

QString Settings::getValue(std::string key) {
	QString res;
	std::cout << "Getting value " << key << "\n";
	if(!f.is_open()) {
		std::cout << "File isn't open!\n";
		return res;
	}

	std::string line;
	while(!f.eof()) {
		getline(f,line);
		if(line.compare(0,key.size(),key) == 0) {
			res = QString::fromStdString(line.substr(key.size()+1));
			std::cout << res.toStdString() << "\n";
			return res;
		}
	}
	return res;
}