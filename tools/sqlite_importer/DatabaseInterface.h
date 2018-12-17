/*
 * DatabaseInterface.h
 *
 *  Created on: 26.03.2018
 *      Author: hendrik
 */

#ifndef TOOLS_SQLITE_IMPORTER_DATABASEINTERFACE_H_
#define TOOLS_SQLITE_IMPORTER_DATABASEINTERFACE_H_

#include <sqlite3.h>

#include <string>

class DatabaseInterface {
public:
	DatabaseInterface(const char *fileName);
	virtual ~DatabaseInterface();

	int executeStatement(std::string statement);
private:
	sqlite3 *m_DBHandle;
};

#endif /* TOOLS_SQLITE_IMPORTER_DATABASEINTERFACE_H_ */
