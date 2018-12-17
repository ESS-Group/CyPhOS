/*
 * CyphosReader.h
 *
 *  Created on: 26.03.2018
 *      Author: hendrik
 */

#ifndef TOOLS_SQLITE_IMPORTER_CYPHOSREADER_H_
#define TOOLS_SQLITE_IMPORTER_CYPHOSREADER_H_

#include <iostream>

#include "DatabaseInterface.h"

class CyphosReader {
public:
	CyphosReader(std::istream*, const char *dbName, const char *experimentName);
	virtual ~CyphosReader();

	void startReading();

private:
	std::istream *m_InputStream;

	/**
	 * Only lines with this tag are used for parsing. Other lines are discarded
	 */
	static constexpr char *containerTAG = "RESULT";

	static constexpr uint32_t c_TransactionSize = 500;

	std::string m_ExperimentName;

	DatabaseInterface m_DatabaseInterface;
};

#endif /* TOOLS_SQLITE_IMPORTER_CYPHOSREADER_H_ */
