#include <string.h>
#include <cctype>
#include <zlib.h>
#include <limits>
#include <algorithm>

#include "zipfile.h"

namespace genesis {

typedef unsigned int dword;
typedef unsigned short word;
typedef unsigned char byte;

#pragma pack(1)
struct ZipFile::TZipLocalHeader {
	enum
	{
		SIGNATURE = 0x04034b50,
	};
	dword   sig;
	word    version;
	word    flag;
	word    compression;      // Z_NO_COMPRESSION or Z_DEFLATED
	word    modTime;
	word    modDate;
	dword   crc32;
	dword   cSize;
	dword   ucSize;
	word    fnameLen;         // Filename string follows header.
	word    xtraLen;          // Extra field follows filename.
};

struct ZipFile::TZipDirHeader {
	enum
	{
		SIGNATURE = 0x06054b50
	};
	dword   sig;
	word    nDisk;
	word    nStartDisk;
	word    nDirEntries;
	word    totalDirEntries;
	dword   dirSize;
	dword   dirOffset;
	word    cmntLen;
};

struct ZipFile::TZipDirFileHeader {
	enum
	{
		SIGNATURE   = 0x02014b50
	};
	dword   sig;
	word    verMade;
	word    verNeeded;
	word    flag;
	word    compression;      // COMP_xxxx
	word    modTime;
	word    modDate;
	dword   crc32;
	dword   cSize;            // Compressed size
	dword   ucSize;           // Uncompressed size
	word    fnameLen;         // Filename string follows header.
	word    xtraLen;          // Extra field follows filename.
	word    cmntLen;          // Comment field follows extra field.
	word    diskStart;
	word    intAttr;
	dword   extAttr;
	dword   hdrOffset;

	char* GetName() const { return (char *)(this + 1); }
	char* GetExtra() const { return GetName() + fnameLen; }
	char* GetComment() const { return GetExtra() + xtraLen; }
};

#pragma pack()

ZipFile::ZipFile() {
	m_numEntries = 0;
	m_pFile = NULL;
	m_pDirData = NULL;
}//ZipFile::ZipFile

ZipFile::~ZipFile() {
	end();
	fclose(m_pFile);
}//ZipFile::~ZipFile

bool ZipFile::init( const std::string& resFileName ) {
	end();

	m_pFile = fopen(resFileName.c_str(), "rb");
	if( !m_pFile )
		return false;

	// Assuming no extra comment at the end, read the whole end record.
	TZipDirHeader dh;
	int sized = sizeof(dword);
	int sizew = sizeof(word);
	int sizeb = sizeof(byte);
	int sizedh = sizeof(dh);
	fseek(m_pFile, -(int)sizeof(dh), SEEK_END);
	long dhOffset = ftell(m_pFile);
	memset(&dh, 0, sizeof(dh));
	fread(&dh, sizeof(dh), 1, m_pFile);

	// Check
	if( dh.sig != TZipDirHeader::SIGNATURE )
		return false;

	// Go to the beginning of the directory.
	fseek(m_pFile, dhOffset - dh.dirSize, SEEK_SET);

	// Allocate the data buffer, and read the whole thing.
	m_pDirData = new char[dh.dirSize + dh.nDirEntries*sizeof(*m_papDir)];
	if( !m_pDirData )
		return false;
	memset(m_pDirData, 0, dh.dirSize + dh.nDirEntries*sizeof(*m_papDir));
	fread(m_pDirData, dh.dirSize, 1, m_pFile);

	// Now process each entry.
	char* pfh = m_pDirData;
	m_papDir = (const TZipDirFileHeader **)(m_pDirData + dh.dirSize);

	bool success = true;

	for( int i = 0; i < dh.nDirEntries && success; i++ ) {
		TZipDirFileHeader& fh = *(TZipDirFileHeader*)pfh;

		// Store the address of nth file for quicker access.
		m_papDir[i] = &fh;

		// Check the directory entry integrity.
		if( fh.sig != TZipDirFileHeader::SIGNATURE )
			success = false;
		else {
			pfh += sizeof(fh);

			// Convert DOS backlashes to UNIX slashes.
			for( int j = 0; j < fh.fnameLen; j++ )
				if( pfh[j] == '\\'	)
					pfh[j] = '/';

			char fileName[PATH_MAX];
			memcpy(fileName, pfh, fh.fnameLen);
			fileName[fh.fnameLen]=0;
			//_strlwr_s(fileName, PATH_MAX);
			std::string lowerCase = fileName;
			std::transform(lowerCase.begin(), lowerCase.end(), lowerCase.begin(), (int(*)(int)) std::tolower);
			std::string spath = lowerCase;
			m_ZipContentsMap[spath] = i;

			// Skip name, extra and comment fields.
			pfh += fh.fnameLen + fh.xtraLen + fh.cmntLen;
		}
	}

	if( !success ) {
		delete[] m_pDirData;
	}
	else {
		m_numEntries = dh.nDirEntries;
	}

	return success;
}//ZipFile::init

int ZipFile::find( const std::string& path ) const {
	std::string lowerCase = path;
	std::transform(lowerCase.begin(), lowerCase.end(), lowerCase.begin(), (int(*)(int)) std::tolower);
	ZipContentsMap::const_iterator i = m_ZipContentsMap.find(lowerCase);
	if( i == m_ZipContentsMap.end() )
		return -1;

	return i->second;
}//ZipFile::find

void ZipFile::end() {
	m_ZipContentsMap.clear();
	delete[] m_pDirData;
	m_numEntries = 0;
}//ZipFile::end

int ZipFile::getNumFiles() const {
	return m_numEntries;
}//ZipFile::getNumFiles

std::string ZipFile::getFilename( int i ) const {
	std::string fileName = "";
	if( i >= 0 && i < m_numEntries ) {
		char pszDest[PATH_MAX];
		memcpy(pszDest, m_papDir[i]->GetName(), m_papDir[i]->fnameLen);
		pszDest[m_papDir[i]->fnameLen] = '\0';
		fileName = pszDest;
	}

	return fileName;
}//ZipFile::getFilename


int ZipFile::getFileLength( int i ) const {
	if( i < 0 || i >= m_numEntries )
		return -1;
	else
		return m_papDir[i]->ucSize;
}//ZipFile::getFileLen

bool ZipFile::readFile( int i, void* pBuf ) {
	if( pBuf == NULL || i < 0 || i >= m_numEntries )
		return false;

	// Quick'n dirty read, the whole file at once.
	// Ungood if the ZIP has huge files inside

	// Go to the actual file and read the local header.
	fseek(m_pFile, m_papDir[i]->hdrOffset, SEEK_SET);
	TZipLocalHeader h;

	memset(&h, 0, sizeof(h));
	fread(&h, sizeof(h), 1, m_pFile);
	if( h.sig != TZipLocalHeader::SIGNATURE )
		return false;

	// Skip extra fields
	fseek(m_pFile, h.fnameLen + h.xtraLen, SEEK_CUR);

	if( h.compression == Z_NO_COMPRESSION ) {
		// Simply read in raw stored data.
		fread(pBuf, h.cSize, 1, m_pFile);
		return true;
	}
	else if( h.compression != Z_DEFLATED )
		return false;

	// Alloc compressed data buffer and read the whole stream
	char* pcData = new char[h.cSize];
	if( !pcData )
		return false;

	memset(pcData, 0, h.cSize);
	fread(pcData, h.cSize, 1, m_pFile);

	bool ret = true;

	// Setup the inflate stream.
	z_stream stream;
	int err;

	stream.next_in = (Bytef*)pcData;
	stream.avail_in = (uInt)h.cSize;
	stream.next_out = (Bytef*)pBuf;
	stream.avail_out = h.ucSize;
	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;

	// Perform inflation. wbits < 0 indicates no zlib header inside the data.
	err = inflateInit2(&stream, -MAX_WBITS);
	if( err == Z_OK ) {
		err = inflate(&stream, Z_FINISH);
		inflateEnd(&stream);
		if( err == Z_STREAM_END )
			err = Z_OK;
		inflateEnd(&stream);
	}
	if( err != Z_OK )
		ret = false;

	delete[] pcData;

	return ret;
}//ZipFile::readFile

}
