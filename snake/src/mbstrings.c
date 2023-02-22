#include "mbstrings.h"
#define MASKONE 0b10000000  
#define MASKTWO 0b11100000
#define TWO 0b11000000       
#define THREE 0b11100000        
#define FOUR 0b11110000 
#define MASK 0b11110000  

/* mbslen - multi-byte string length
 * - Description: returns the number of UTF-8 code points ("characters")
 * in a multibyte string. If the argument is NULL or an invalid UTF-8
 * string is passed, returns -1.
 *
 * - Arguments: A pointer to a character array (`bytes`), consisting of UTF-8
 * variable-length encoded multibyte code points.
 *
 * - Return: returns the actual number of UTF-8 code points in `src`. If an
 * invalid sequence of bytes is encountered, return -1.
 *
 * - Hints:
 * UTF-8 characters are encoded in 1 to 4 bytes. The number of leading 1s in the
 * highest order byte indicates the length (in bytes) of the character. For
 * example, a character with the encoding 1111.... is 4 bytes long, a character
 * with the encoding 1110.... is 3 bytes long, and a character with the encoding
 * 1100.... is 2 bytes long. Single-byte UTF-8 characters were designed to be
 * compatible with ASCII. As such, the first bit of a 1-byte UTF-8 character is
 * 0.......
 *
 * You will need bitwise operations for this part of the assignment!
 */
size_t mbslen(const char* bytes) {
    // TODO: implement!
    int code_points = 0; 
    while(*bytes != '\0'){
        if((*bytes & MASKONE) == 0b00000000){ //one byte
            code_points++;
            bytes++;
        }
        else if((*bytes & MASKTWO) == TWO){
            code_points++;
            bytes += 2;
        }
        else if((*bytes & MASK) == THREE){
            code_points++;
            bytes += 3;
        }
        else if((*bytes & MASK) == FOUR){
            code_points++;
            bytes += 4;
        }
        else{
            return -1;
        }
    }
    return code_points;
}
