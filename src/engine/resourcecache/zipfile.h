#ifndef ZIPFILE_H
#define ZIPFILE_H

#include <stdio.h>
#include <string>
#include <map>

namespace genesis {

typedef std::map<std::string, int> ZipContentsMap;

class ZipFile {
private:
	struct TZipDirHeader;
	struct TZipDirFileHeader;
	struct TZipLocalHeader;

	FILE*	m_pFile;
	char*	m_pDirData;
	int		m_numEntries;

	const TZipDirFileHeader**	m_papDir;

public:
	ZipFile();
	virtual ~ZipFile();

	bool init( const std::string &resFileName);
	void end();

	int getNumFiles() const;
	std::string getFilename( int i ) const;
	int getFileLength( int i ) const;
	bool readFile( int i, void* pBuf );

	int find( const std::string& path ) const;

	ZipContentsMap	m_ZipContentsMap;
};

}

#endif // ZIPFILE_H
