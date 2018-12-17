
#include <iostream>
#include <fstream>

#include "CyphosReader.h"

void printUsage() {
	std::cout << "Usage ./reader sqlite_db_name.db experiment_name [logfile]" << std::endl;
}

int main(int argc, char **argv) {
	std::cout << "CyPhOS SQlite log importer" << std::endl;

	if (argc < 3) {
		printUsage();
		exit(EXIT_FAILURE);
	}

	CyphosReader *reader;
	if (argc == 3) {
		// No logfile provided, use standard input
		reader = new CyphosReader(nullptr, argv[1], argv[2]);
	} else if (argc == 4) {
		reader = new CyphosReader(new std::ifstream(argv[3], std::ios::in), argv[1], argv[2]);
	}

	reader->startReading();

	return 0;
}
