#ifndef __UTF8_H__
#define __UTF8_H__

/* utf8.h -- convert characters to/from UTF-8

  (c) 1998-2006 (W3C) MIT, ERCIM, Keio University
  See tidy.h for the copyright notice.

  CVS Info :

    $Author: arnaud02 $ 
    $Date: 2006/09/12 15:14:44 $ 
    $Revision: 1.5 $ 

*/

typedef unsigned int uint;
typedef unsigned char byte;
typedef uint tchar;         /* single, full character */
typedef char tmbchar;       /* single, possibly partial character */
typedef tmbchar* tmbstr;    /* pointer to buffer of possibly partial chars */
typedef const tmbchar* ctmbstr; /* Ditto, but const */
typedef enum
{
   no,
   yes
} Bool;

/* Internal symbols are prefixed to avoid clashes with other libraries */
#define TYDYAPPEND(str1,str2) str1##str2
#define TY_(str) TYDYAPPEND(prvTidy,str)

/** End of input "character" */
#define EndOfStream (~0u)

/* UTF-8 encoding/decoding support
** Does not convert character "codepoints", i.e. to/from 10646.
*/

int TY_(DecodeUTF8BytesToChar)( uint* c, uint firstByte, ctmbstr successorBytes,
                                int* count );

#endif /* __UTF8_H__ */
