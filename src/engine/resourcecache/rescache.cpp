//#include <list>
//#include <vector>
//#include <cassert>
#include <algorithm>
#include <cstring>

#include "rescache.h"
#include "utilities/string.h"
#include "utilities/logger.h"

namespace genesis {

Resource::Resource( const std::string& name ) {
	m_name = name;
	std::transform(m_name.begin(), m_name.end(), m_name.begin(), (int(*)(int)) std::tolower);
}//Resource::Resource

ResourceZipFile::~ResourceZipFile() {
	delete m_pZipFile;
}//ResourceZipFile::~ResourceZipFile

bool ResourceZipFile::VOpen() {
	m_pZipFile = new ZipFile;
	if( m_pZipFile ) {
		return m_pZipFile->init(m_resFileName.c_str());
	}

	return false;
}//ResourceZipFile::VOpen

int ResourceZipFile::VGetRawResourceSize( const Resource& r ) {
	int resourceNum = m_pZipFile->find(r.m_name.c_str());
	if( resourceNum == -1 )
		return -1;

	return m_pZipFile->getFileLength(resourceNum);
}//ResourceZipFile::VGetRawResourceSize

int ResourceZipFile::VGetRawResource( const Resource& r, char* buffer ) {
	int size = 0;
	int resourceNum = m_pZipFile->find(r.m_name.c_str());
	if( resourceNum != -1 ) {
		size = m_pZipFile->getFileLength(resourceNum);
		m_pZipFile->readFile(resourceNum, buffer);
	}

	return size;
}//ResourceZipFile::VGetRawResource

int ResourceZipFile::VGetNumResources() const {
	return (m_pZipFile == NULL) ? 0 : m_pZipFile->getNumFiles();
}//ResourceZipFile::VGetNumResources

std::string ResourceZipFile::VGetResourceFileName() const {
	return m_resFileName;
}//ResourceZipFile::VGetResourceFileName

std::string ResourceZipFile::VGetResourceName( int num ) const {
	std::string resName = "";
	if( m_pZipFile != NULL && num >= 0 && num < m_pZipFile->getNumFiles() ) {
		resName = m_pZipFile->getFilename(num);
	}

	return resName;
}//ResourceZipFile::VGetResourceName

ResHandle::ResHandle( Resource& resource, char* buffer, unsigned int size, ResCache* pResCache ) : m_resource(resource) {
	m_buffer = buffer;
	m_size = size;
	m_extra = NULL;
	m_pResCache = pResCache;
}//ResHandle::ResHandle

ResHandle::~ResHandle() {
	delete[] m_buffer;
	m_pResCache->memoryHasBeenFreed(m_size);
}//ResHandle::~ResHandle

ResCache::ResCache(const unsigned int sizeInMb, ResourceFiles files ) {
	m_cacheSize = sizeInMb * 1024 * 1024;						// total memory size
	m_allocated = 0;											// total memory allocated
	m_files = files;
}//ResCache::ResCache

ResCache::~ResCache() {
	while( !m_lru.empty() ) {
		freeOneResource();
	}
	while( !m_files.empty() ) {
		ResourceFiles::iterator file = m_files.begin();
		delete (*file);
	}
}//ResCache::~ResCache

bool ResCache::init() {
	for( ResourceFiles::iterator fileItr = m_files.begin(); fileItr != m_files.end(); ++fileItr ) {
		if( (*fileItr)->VOpen() ) {
			registerLoader(std::shared_ptr<IResourceLoader>(new DefaultResourceLoader()));
		}
		else {
			GEN_FATAL("unable to load filename: " + (*fileItr)->VGetResourceFileName());
			return false;
		}
	}

	return true;
}//ResCache::init

void ResCache::registerLoader( std::shared_ptr<IResourceLoader> loader ) {
	m_resourceLoaders.push_front(loader);
}//ResCache::registerLoader

std::shared_ptr<ResHandle> ResCache::getHandle( Resource* r ) {
	std::shared_ptr<ResHandle> handle(find(r));
	if( handle == NULL ) {
		handle = load(r);
		assert(handle);
	}
	else {
		update(handle);
	}

	return handle;
}//ResCache::getHandle

std::shared_ptr<ResHandle> ResCache::load( Resource* r ) {
	// Create a new resource and add it to the lru list and map
	std::shared_ptr<IResourceLoader> loader;
	std::shared_ptr<ResHandle> handle;

	for( ResourceLoaders::iterator it = m_resourceLoaders.begin(); it != m_resourceLoaders.end(); ++it ) {
		std::shared_ptr<IResourceLoader> testLoader = *it;

		if( wildcardMatch(testLoader->VGetPattern().c_str(), r->m_name.c_str()) ) {
			loader = testLoader;
			break;
		}
	}

	if( !loader ) {
		assert(loader && "Default resource loader not found!");
		return handle;          // Resource not loaded!
	}

	// determine which resource file it's located in
	ResourceFiles::iterator fileItr = m_files.begin();
	while( fileItr != m_files.end() ) {
		if( (*fileItr)->VGetRawResourceSize(*r) == -1 )
			++fileItr;
		else
			break;
	}

	int rawSize = (*fileItr)->VGetRawResourceSize(*r);
	if( rawSize < 0 ) {
		assert(rawSize > 0 && "Resource size returned -1 - Resource not found");
		return std::shared_ptr<ResHandle>();
	}

	int allocSize = rawSize + ((loader->VAddNullZero()) ? (1) : (0));
	char* rawBuffer = loader->VUseRawFile() ? allocate(allocSize) : new char[allocSize];
	memset(rawBuffer, 0, allocSize);

	if( rawBuffer == NULL || (*fileItr)->VGetRawResource(*r, rawBuffer) == 0 ) {
		// resource cache out of memory
		return std::shared_ptr<ResHandle>();
	}

	char* buffer = NULL;
	unsigned int size = 0;

	if( loader->VUseRawFile() ) {
		buffer = rawBuffer;
		handle = std::shared_ptr<ResHandle>(new ResHandle(*r, buffer, rawSize, this));
	}
	else {
		size = loader->VGetLoadedResourceSize(rawBuffer, rawSize);
		buffer = allocate(size);
		if( rawBuffer == NULL || buffer == NULL ) {
			// resource cache out of memory
			return std::shared_ptr<ResHandle>();
		}
		handle = std::shared_ptr<ResHandle>(new ResHandle(*r, buffer, size, this));
		bool success = loader->VLoadResource(rawBuffer, rawSize, handle);

		if( loader->VDiscardRawBufferAfterLoad() ) {
			delete[] rawBuffer;
		}

		if( !success ) {
				// resource cache out of memory
				return std::shared_ptr<ResHandle>();
		}
	}

	if( handle ) {
		m_lru.push_front(handle);
		m_resources[r->m_name] = handle;
	}

	assert(loader && "Default resource loader not found!");

	return handle;			// ResCache is out of memory!
}//ResCache::load

std::shared_ptr<ResHandle> ResCache::find( Resource* r ) {
	ResHandleMap::iterator i = m_resources.find(r->m_name);
	if( i == m_resources.end() )
		return std::shared_ptr<ResHandle>();

	return i->second;
}//ResCache::find

void ResCache::update( std::shared_ptr<ResHandle> handle ) {
	m_lru.remove(handle);
	m_lru.push_front(handle);
}//ResCache::update

char* ResCache::allocate( unsigned int size ) {
	if( !makeRoom(size) )
		return NULL;

	char* mem = new char[size];
	if( mem ) {
		m_allocated += size;
	}

	return mem;
}//ResCache::allocate

void ResCache::freeOneResource() {
	ResHandleList::iterator gonner = m_lru.end();
	gonner--;

	std::shared_ptr<ResHandle> handle = *gonner;

	m_lru.pop_back();
	m_resources.erase(handle->m_resource.m_name);
}//ResCache::freeOneResource

void ResCache::flush() {
	while( !m_lru.empty() ) {
		std::shared_ptr<ResHandle> handle = *(m_lru.begin());
		free(handle);
		m_lru.pop_front();
	}
}//ResCache::flush

bool ResCache::makeRoom( unsigned int size ) {
	if( size > m_cacheSize )
		return false;

	// return null if there's no possible way to allocate the memory
	while( size > (m_cacheSize - m_allocated) ) {
		// The cache is empty, and there's still not enough room.
		if( m_lru.empty() )
			return false;

		freeOneResource();
	}

	return true;
}//ResCache::makeRoom

void ResCache::free( std::shared_ptr<ResHandle> gonner ) {
	m_lru.remove(gonner);
	m_resources.erase(gonner->m_resource.m_name);
}//ResCache::free

void ResCache::memoryHasBeenFreed( unsigned int size ) {
	m_allocated -= size;
}//ResCache::memoryHasBeenFreed

std::vector<std::string> ResCache::match( const std::string pattern ) {
	std::vector<std::string> matchingNames;
	if( m_files.empty() )
		return matchingNames;

	for( ResourceFiles::iterator fileItr = m_files.begin(); fileItr != m_files.end(); ++fileItr ) {
		int numFiles = (*fileItr)->VGetNumResources();
		for( int i = 0; i < numFiles; ++i ) {
			std::string name = (*fileItr)->VGetResourceName(i);
			std::transform(name.begin(), name.end(), name.begin(), (int(*)(int)) std::tolower);
			if( wildcardMatch(pattern.c_str(), name.c_str()) ) {
				matchingNames.push_back(name);
			}
		}
	}

	return matchingNames;
}//ResCache::match

int ResCache::preload( const std::string pattern, void (*progressCallback)(int, bool &) ) {
	if( m_files.empty() )
		return 0;

	int loaded = 0;
	for( ResourceFiles::iterator fileItr = m_files.begin(); fileItr != m_files.end(); ++fileItr ) {
		int numFiles = (*fileItr)->VGetNumResources();
		bool cancel = false;
		for( int i = 0; i < numFiles; ++i ) {
			Resource resource((*fileItr)->VGetResourceName(i));

			if( wildcardMatch(pattern.c_str(), resource.m_name.c_str()) ) {
				std::shared_ptr<ResHandle> handle = getHandle(&resource);
				++loaded;
			}

			if( progressCallback != NULL ) {
				progressCallback(i * 100/numFiles, cancel);
			}
		}
	}

	return loaded;
}//ResCache::preload

}
