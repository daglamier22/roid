#ifndef STRING_H
#define STRING_H

#include <vector>
#include <string>

namespace genesis {

#define MAX_DIGITS_IN_INT 12  // max number of digits in an int (-2147483647 = 11 digits, +1 for the '\0')
typedef std::vector<std::string> StringVec;

// Removes leading white space
extern void TrimLeft( std::wstring &s );

// Does a classic * & ? pattern match on a file name - this is case sensitive!
extern bool WildcardMatch( const char *pat, const char *str );
/*
extern std::string ws2s( const std::wstring& s );
extern std::wstring s2ws( const std::string &s );
*/
extern std::string ToStr( int num, int base = 10 );
extern std::string ToStr( unsigned int num, int base = 10 );
extern std::string ToStr( unsigned long num, int base = 10 );
extern std::string ToStr( float num );
extern std::string ToStr( double num );
extern std::string ToStr( bool val );
//extern std::string ToStr( const Vec3& vec );

// Splits a string by the delimeter into a vector of strings.  For example, say you have the following string:
// std::string test("one,two,three");
// You could call Split() like this:
// Split(test, outVec, ',');
// outVec will have the following values:
// "one", "two", "three"
void Split( const std::string& str, StringVec& vec, char delimiter );

}

#endif // STRING_H
