#include <cstring>

#include "string.h"

namespace genesis {

// Remove all leading whitespace
void TrimLeft( std::wstring& s ) {
	// find first non-space character
	int i = 0;
	int len = (int)s.length();
	while( i <  len ) {
#ifdef UNICODE
		wchar_t ch = s[i];
		int white = iswspace( ch );
#else
		char ch = s[i];
		int white = isspace( ch );
#endif
		if( !white )
			break;
		++i;
	}

	if( i < len )
		s = s.substr(i);
}//TrimLeft


// The following function was found on http://xoomer.virgilio.it/acantato/dev/wildcard/wildmatch.html, where it was attributed to
// the C/C++ Users Journal, written by Mike Cornelison. It is a little ugly, but it is FAST. Use this as an excercise in not reinventing the
// wheel, even if you see gotos.

bool WildcardMatch( const char *pat, const char *str ) {
	int i, star;

new_segment:

	star = 0;
	if( *pat == '*' ) {
		star = 1;
		do {
			pat++;
		} while( *pat == '*' );
	}

test_match:

	for( i = 0; pat[i] && (pat[i] != '*'); i++ ) {
		if( str[i] != pat[i] ) {
			if( !str[i] )
				return false;
			if( (pat[i] == '?') && (str[i] != '.') )
				continue;
			if( !star )
				return false;
			str++;
			goto test_match;
		}
	}
	if( pat[i] == '*' ) {
		str += i;
		pat += i;
		goto new_segment;
	}
	if( !str[i] )
		return true;
	if( i && pat[i - 1] == '*' )
		return true;
	if( !star )
		return false;
	str++;
	goto test_match;
}//WildcardMatch




/*std::string ws2s( const std::wstring& s ) {
	int slength = (int)s.length() + 1;
	int len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0)-1;
	std::string r(len, '\0');
	WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, &r[0], len, 0, 0);
	return r;
}//ws2s


std::wstring s2ws( const std::string& s ) {
	int slength = (int)s.length() + 1;
	int len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0)-1;
	std::wstring r(len, '\0');
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, &r[0], len);
	return r;
}//s2ws*/





/*std::string ToStr( int num, int base ) {
	char str[MAX_DIGITS_IN_INT];
	memset(str,0,MAX_DIGITS_IN_INT);
	_itoa_s(num,str,MAX_DIGITS_IN_INT,base);
	return (std::string(str));
}//ToStr(int,int)

std::string ToStr( unsigned int num, int base ) {
	char str[MAX_DIGITS_IN_INT];
	memset(str, 0, MAX_DIGITS_IN_INT);
	_ultoa_s((unsigned long)num, str, MAX_DIGITS_IN_INT, base);
	return (std::string(str));
}//ToStr(unsigned int,int)

std::string ToStr( unsigned long num, int base ) {
	char str[MAX_DIGITS_IN_INT];
	memset(str,0,MAX_DIGITS_IN_INT);
	_ultoa_s(num,str,MAX_DIGITS_IN_INT,base);
	return (std::string(str));
}//ToStr(unsigned long,int)

std::string ToStr( float num ) {
	char str[64];  // I'm sure this is overkill
	memset(str,0,64);
	_sprintf_p(str,64,"%f",num);
	return (std::string(str));
}//ToStr(float)

std::string ToStr( double num ) {
	char str[64];  // I'm sure this is overkill
	memset(str,0,64);
	_sprintf_p(str,64,"%fL",num);
	return (std::string(str));
}//ToStr(double)

std::string ToStr( bool val ) {
	return (std::string( (val == true ? "true" : "false") ));
}//ToStr(bool)*/

/*std::string ToStr( const Vec3& vec ) {
	return string("(" + ToStr(vec.x) + "," + ToStr(vec.y) + "," + ToStr(vec.z) + ")");
}////ToStr(Vec3)*/



//---------------------------------------------------------------------------------------------------------------------
// This is basically like the Perl split() function.  It splits str into substrings by cutting it at each delimiter.
// The result is stored in vec.
//---------------------------------------------------------------------------------------------------------------------
void Split( const std::string& str, StringVec& vec, char delimiter ) {
	vec.clear();
	size_t strLen = str.size();
	if( strLen == 0 )
		return;

		size_t startIndex = 0;
		size_t indexOfDel = str.find_first_of(delimiter, startIndex);
		while( indexOfDel != std::string::npos ) {
			vec.push_back(str.substr(startIndex, indexOfDel-startIndex));
			startIndex=indexOfDel + 1;
			if( startIndex >= strLen )
				break;
			indexOfDel = str.find_first_of(delimiter, startIndex);
		}
		if( startIndex < strLen )
			vec.push_back(str.substr(startIndex));
}//Split

}
