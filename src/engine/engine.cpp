#include <iostream>

#include "engine.h"
#include "utilities/logger.h"
#include "utilities/rng.h"
#include "resourcecache/rescache.h"

namespace genesis {

Engine::Engine()
{
}

void run() {
	ResourceFiles files;
	std::string resourcefilenames[3] = {"system.zip", "levels.zip", "objects.zip"};
	for( int i = 0; i < 3; i++ ) {
		IResourceFile* zip = new ResourceZipFile(resourcefilenames[i]);
		files.push_back(zip);
	}

	ResCache* resCache = new ResCache(50, files);
	if( resCache->init() ) {
		std::cout << "successfully loaded" << std::endl;
	}
	else {
		std::cout << "error loading" << std::endl;
	}
	GEN_LOG("test","testing a new tag");
}

}
