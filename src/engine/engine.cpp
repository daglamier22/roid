#include <iostream>

#include "engine.h"
#include "utilities/logger.h"
#include "utilities/rng.h"
#include "resourcecache/zipfile.h"

namespace genesis {

Engine::Engine()
{
}

void run() {
	ZipFile* zip;
	zip = new ZipFile();
	char* buf;
	if( !zip->init("/mnt/Storage/Projects/roid/game/data.zip") ) {
		std::cout << "error loading zip file" << std::endl;
	}
	else {
		std::cout << "zip loaded successfully" << std::endl;
		std::cout << zip->find("docs/logging.xml") << std::endl;
		std::cout << zip->getFilename(1) << " " << zip->getFileLength(1) << std::endl;
		buf = new char[zip->getFileLength(0)];
		zip->readFile(0,buf);
	}
}

}
