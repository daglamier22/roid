#include "string.h"

// The following function was found on http://xoomer.virgilio.it/acantato/dev/wildcard/wildmatch.html, where it was attributed to
// the C/C++ Users Journal, written by Mike Cornelison. It is a little ugly, but it is FAST. Use this as an excercise in not reinventing the
// wheel, even if you see gotos.

bool wildcardMatch( const char *pat, const char *str ) {
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
}//wildcardMatch
