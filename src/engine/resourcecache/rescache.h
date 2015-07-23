#ifndef RESCACHE_H
#define RESCACHE_H

#include <list>
#include <vector>
#include <memory>

#include "rescache_interfaces.h"
#include "zipfile.h"

namespace genesis {

class ResHandle;
class ResCache;

class Resource
{
public:
	std::string		m_name;

	Resource( const std::string& name );
};

class ResourceZipFile : public IResourceFile
{
	ZipFile*		m_pZipFile;
	std::string		m_resFileName;

public:
	ResourceZipFile( const std::string resFileName ) { m_pZipFile = NULL; m_resFileName = resFileName; }
	virtual ~ResourceZipFile();

	virtual bool VOpen();
	virtual int VGetRawResourceSize( const Resource& r );
	virtual int VGetRawResource( const Resource& r, char* buffer );
	virtual int VGetNumResources() const;
	virtual std::string VGetResourceFileName() const;
	virtual std::string VGetResourceName( int num ) const;
	virtual bool VIsUsingDevelopmentDirectories() const { return false; }
};

class ResHandle
{
	friend class ResCache;

protected:
	Resource								m_resource;
	char*									m_buffer;
	unsigned int							m_size;
	std::shared_ptr<IResourceExtraData>		m_extra;
	ResCache*								m_pResCache;

public:
	ResHandle( Resource& resource, char* buffer, unsigned int size, ResCache* pResCache );
	virtual ~ResHandle();

	const std::string getName() { return m_resource.m_name; }
	unsigned int size() const { return m_size; }
	char* buffer() const { return m_buffer; }
	char* writableBuffer() { return m_buffer; }

	std::shared_ptr<IResourceExtraData> getExtra() { return m_extra; }
	void setExtra( std::shared_ptr<IResourceExtraData> extra ) { m_extra = extra; }
};

class DefaultResourceLoader : public IResourceLoader
{
public:
	virtual bool VUseRawFile() { return true; }
	virtual bool VDiscardRawBufferAfterLoad() { return true; }
	virtual unsigned int VGetLoadedResourceSize( char* rawBuffer, unsigned int rawSize ) { (void)rawBuffer; return rawSize; }
	virtual bool VLoadResource( char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> handle ) { (void)rawBuffer; (void)rawSize; (void)handle; return true; }
	virtual std::string VGetPattern() { return "*"; }
};

typedef std::list< std::shared_ptr<ResHandle> > ResHandleList;					// lru list
typedef std::map<std::string, std::shared_ptr<ResHandle> > ResHandleMap;		// maps indentifiers to resource data
typedef std::list<std::shared_ptr<IResourceLoader> > ResourceLoaders;
typedef std::vector<IResourceFile*> ResourceFiles;

class ResCache
{
	friend class ResHandle;

	ResHandleList		m_lru;							// lru list
	ResHandleMap		m_resources;
	ResourceLoaders		m_resourceLoaders;

	ResourceFiles		m_files;

	unsigned int		m_cacheSize;					// total memory size
	unsigned int		m_allocated;					// total memory allocated

protected:
	bool makeRoom( unsigned int size );
	char* allocate( unsigned int size );
	void free( std::shared_ptr<ResHandle> gonner );

	std::shared_ptr<ResHandle> load( Resource* r );
	std::shared_ptr<ResHandle> find( Resource* r );
	void update( std::shared_ptr<ResHandle> handle );

	void freeOneResource();
	void memoryHasBeenFreed( unsigned int size );

public:
	ResCache( const unsigned int sizeInMb, ResourceFiles files );
	virtual ~ResCache();

	bool init();

	void registerLoader( std::shared_ptr<IResourceLoader> loader );

	std::shared_ptr<ResHandle> getHandle( Resource* r );

	int preload( const std::string pattern, void (*progressCallback)(int, bool &) );
	std::vector<std::string> match( const std::string pattern );

	void flush();
};

}

#endif // RESCACHE_H
