/*
 * DatabaseInterface.cc
 *
 *  Created on: 26.03.2018
 *      Author: hendrik
 */

#include "DatabaseInterface.h"
#include <iostream>
#include <string>


DatabaseInterface::DatabaseInterface(const char *fileName) {
	// Open the database
	int result = sqlite3_open(fileName, &m_DBHandle);
	if (result != SQLITE_OK) {
		std::cerr << "Error opening sqlite database: " << sqlite3_errmsg(m_DBHandle) << std::endl;
		exit(EXIT_FAILURE);
	}

}

DatabaseInterface::~DatabaseInterface() {
	// TODO Auto-generated destructor stub
}

int DatabaseInterface::executeStatement(std::string statement) {
	// Execute statement
	sqlite3_stmt *sqlite_statement;
	int result = sqlite3_prepare_v3(m_DBHandle, statement.c_str(),-1, 0, &sqlite_statement, NULL);
	if (result != SQLITE_OK) {
		std::cerr << "Error result insert: " << sqlite3_errmsg(m_DBHandle) << std::endl;
		sqlite3_finalize(sqlite_statement);
		return result;
	}

	// Evaluate statement
	result = sqlite3_step(sqlite_statement);
	if (result != SQLITE_DONE) {
		std::cerr << "Error result insert: " << sqlite3_errmsg(m_DBHandle) << std::endl;
		sqlite3_finalize(sqlite_statement);
		return result;
	}


	sqlite3_finalize(sqlite_statement);
	return SQLITE_OK;
}
