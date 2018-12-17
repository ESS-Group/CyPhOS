/*
 * CyphosReader.cc
 *
 *  Created on: 26.03.2018
 *      Author: hendrik
 */

#include "CyphosReader.h"

#include <sstream>
#include <vector>

CyphosReader::CyphosReader(std::istream *inputStream, const char *dbName, const char *experimentName) : m_DatabaseInterface(dbName) {
	if (nullptr == inputStream) {
		// No input stream provided, use standard input
		m_InputStream = &std::cin;
	} else {
		m_InputStream = inputStream;
	}

	m_ExperimentName = std::string(experimentName);
}

CyphosReader::~CyphosReader() {
	// TODO Auto-generated destructor stub
}

void CyphosReader::startReading() {

	/**
	 * Counter of read lines
	 */
	int lineCount = 0;

	/**
	 * Counter of correctly parsed lines
	 */
	int validCount = 0;

	/**
	 * Vector of column names
	 */
	std::vector<std::string> columns;

	/**
	 * Vector of column types
	 */
	std::vector<std::string> types;

	/**
	 * Counter of transactions, every c_TransactionSize it will be reset
	 */
	int transactionCount = 0;

	while (!m_InputStream->eof()) {
		std::string line;
		line.replace(line.begin(), line.end(), '\n',' ');
		line.replace(line.begin(), line.end(), '\r',' ');
		std::getline(*m_InputStream, line,'\n');

		int resultBegin_POS;
		//std::cout << "Line: " << line << std::endl;
		// Search for "RESULT_BEGIN" in stream to define table columns
		if((resultBegin_POS = line.find("RESULT BEGIN")) != std::string::npos) {
			std::cout << "Found RESULT BEGIN at: " << resultBegin_POS << std::endl;

			// Check if columns already parsed (only necessary once. MUST NOT CHANGE DURING EXPERIMENT)
			if (columns.size() == 0) {
				// Extract following substring
				std::string subString = line.substr(line.find_last_of(":") + 1);
				//std::cout << "Substring: " << subString << std::endl;

				// Parse columns if not done before
				std::istringstream tmpStream(subString);
				std::string column;

				// Split column string and push into vector
				while (std::getline(tmpStream, column,';')) {
					columns.push_back(column);
				}

				// Create sqlite table with corresponding columns and experiment name
				std::string sqlCreateTableStatement = "CREATE TABLE IF NOT EXISTS \"" + m_ExperimentName + "\" (";
				bool first = true;
				for (auto &column: columns) {
					if (first) {
						sqlCreateTableStatement += column;
						first = false;
					} else {
						sqlCreateTableStatement += "," + column;
					}
				}
				sqlCreateTableStatement += ");";
				std::cout << "Sql create statement:" << sqlCreateTableStatement << std::endl;

				if (m_DatabaseInterface.executeStatement(sqlCreateTableStatement) != SQLITE_OK) {
					exit(EXIT_FAILURE);
				}

				// Remove types from columns
				for (auto &column: columns) {
					types.push_back(column.substr(column.find_first_of(' ') + 1, column.length()));
					column = column.substr(0, column.find_first_of(' '));
				}

			}
			continue;
		} else if (columns.size() > 0 && line.find(containerTAG) != std::string::npos) {
			if(transactionCount == 0) {
				// Begin new transaction
				m_DatabaseInterface.executeStatement("BEGIN TRANSACTION;");
				transactionCount++;
			}
			// Try to parse normal lines
			// Get substring minus tag and current cpu
			std::string subString = line.substr(line.find_last_of(":") + 1);
			//std::cout << "Found substring: " << subString << std::endl;

			// Split string
			std::istringstream tmpStream(subString);
			std::string columnValue;
			std::vector<std::string> values;
			// Split column string and push into vector
			while (std::getline(tmpStream, columnValue,';')) {
				values.push_back(columnValue);
			}
			if(values.size() != columns.size()) {
				std::cout << "Invalid line, size mismatch: " << values.size() << " vs. " << columns.size() << std::endl;
				// Invalid line, skip
				continue;
			}

			std::string sqlInsert = "INSERT INTO \"" + m_ExperimentName + "\" (";
			bool first = true;
			for (auto &column: columns) {
				if (first) {
					sqlInsert += column;
					first = false;
				} else {
					sqlInsert += "," + column;
				}
			}
			sqlInsert += ") VALUES (";

			first = true;
			int size = values.size();
			for(uint32_t i = 0; i < size; i++)
			{
				if (!first) {
					sqlInsert +=  ", ";
				} else {
					first = false;
				}
				if(types[i] == "integer") {
					sqlInsert += values[i];
				} else if (types[i] == "text") {
					sqlInsert += "\"" +  values[i] + "\"";
				}
			}
			sqlInsert += ");";
			//std::cout << "Insert statement: " << sqlInsert << std::endl;

			m_DatabaseInterface.executeStatement(sqlInsert);
			validCount++;
			transactionCount++;
			if (transactionCount == c_TransactionSize) {
				m_DatabaseInterface.executeStatement("COMMIT TRANSACTION;");
				transactionCount = 0;
			}
		}
		lineCount++;
	}

	std::cout << "Read " << lineCount << " lines, valid results: " << validCount << std::endl;
}
