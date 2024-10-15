#ifndef __BASE64_H__
#define __BASE64_H__

#include <stdint.h>

// returns the size of the encoded buffer
size_t Base64encode_len(size_t _NunOfBytesToEncode);

// returns the number of valid encoded bytes to be processed, without padding charactes ('=')
size_t Base64Decode_nprbytes(const char* bufcoded, size_t len);

// Take as argument the return of Base64Decode_nprbytes(const char* bufcoded, size_t len) and returns the decoded size
size_t Base64decode_len(size_t nprbytes);

/*
* Used to encode bynary data to text and sent over text channels like HTTP.
* It encodes 3 input bytes to 4 output bytest (3 in, 4 out)
* Arguments:
*   _Encoded : a buffer where the encoded data will be stored, it must have a storage of minimum "Base64encode_len(_NunOfBytesToEncode)"
*   _ToEncode : data to be encoded
*   _NunOfBytesToEncode : size in bytes of the data to encode
* Return value:
*   Returns the size of _Encoded
*/
size_t Base64encode(char* _Encoded, const void* _ToEncode, size_t _NunOfBytesToEncode);

/*
* Arguments:
*   _Decoded : buffer where to store the decoded data, minimum storage space = Base64decode_len(Base64Decode_nprbytes(_ToDecode, (len of _ToDecode))
*   _ToDecode : data to be decoded, it must contain only Base64 characters
*   _NunOfBytesToDecode : number of bytes to be decoded, _NunOfBytesToDecode = Base64Decode_nprbytes(_ToDecode, (len of _ToDecode))
* Return value:
*   Returns the size of _Decoded
*/
size_t Base64decode(char* _Decoded, const char* _ToDecode, size_t _NunOfBytesToDecode);

/*
* Arguments:
*   _Encoded : a buffer where the encoded data will be stored, it must have a storage of minimum "Base64encode_len(_NunOfBytesToEncode)"
*   _ToEncode : data to be encoded
*   _NunOfBytesToEncode : size in bytes of the data to encode
* Return value:
*   Returns the size of _Encoded
*/
size_t Base64encode_BigEndian(char* _Encoded, const void* _ToEncode, size_t _NunOfBytesToEncode);

#endif // __BASE64_H__