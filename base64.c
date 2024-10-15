#include "base64.h"

const unsigned char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const unsigned char B64index[] =
{
    /* ASCII table */
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,     //16 by 16
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
    64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
    64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

// returns the size of the encoded buffer
size_t Base64encode_len(size_t _NunOfBytesToEncode)
{
    return ((_NunOfBytesToEncode + 2) / 3 * 4); /* 3-byte blocks to 4-byte */
}

// returns the number of valid encoded bytes to be processed, without padding charactes ('=')
size_t Base64Decode_nprbytes(const char* bufcoded, size_t len)
{
    size_t nprbytes;
    register const unsigned char* bufin;
    register const unsigned char* endbufin;

    endbufin = (const unsigned char*)(bufcoded + len);
    bufin = (const unsigned char*)bufcoded;
    while (bufin < endbufin && B64index[*bufin] <= 63) { bufin++; }
    nprbytes = bufin - (const unsigned char*)bufcoded;
    return nprbytes;
}

// Take as argument the return of Base64Decode_nprbytes(const char* bufcoded, size_t len) and returns the decoded size
size_t Base64decode_len(size_t nprbytes)
{
    size_t nbytesdecoded;
    nbytesdecoded = ((nprbytes * 3) / 4);
    return nbytesdecoded;
}

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
size_t Base64encode(char* _Encoded, const void* _ToEncode, size_t _NunOfBytesToEncode)
{
    register unsigned char* pos;
    register const unsigned char* end, * in;

    end = (unsigned char*)_ToEncode + _NunOfBytesToEncode;
    in = (unsigned char*)_ToEncode;
    pos = (unsigned char*)_Encoded;

    while (end - in >= 3) {     // 76 asm instructions
        *pos++ = base64_table[in[0] >> 2];
        *pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
        *pos++ = base64_table[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
        *pos++ = base64_table[in[2] & 0x3f];
        in += 3;
    }

    if (end - in) {
        *pos++ = base64_table[in[0] >> 2];
        if (end - in == 1) {
            *pos++ = base64_table[(in[0] & 0x03) << 4];
            *pos++ = '=';
        }
        else {
            *pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
            *pos++ = base64_table[(in[1] & 0x0f) << 2];
        }
        *pos++ = '=';
    }
    return (size_t)pos - (size_t)_Encoded;
}

/*
* Arguments:
*   _Decoded : buffer where to store the decoded data, minimum storage space = Base64decode_len(Base64Decode_nprbytes(_ToDecode, (len of _ToDecode))
*   _ToDecode : data to be decoded, it must contain only Base64 characters
*   _NunOfBytesToDecode : number of bytes to be decoded, _NunOfBytesToDecode = Base64Decode_nprbytes(_ToDecode, (len of _ToDecode))
* Return value:
*   Returns the size of _Decoded
*/
size_t Base64decode(char* _Decoded, const char* _ToDecode, size_t _NunOfBytesToDecode)
{
    if (_NunOfBytesToDecode <= 1) return 0;
    size_t nbytesdecoded;
    register const unsigned char* bufin;
    register unsigned char* bufout;
    register size_t nprbytes;

    nprbytes = _NunOfBytesToDecode;
    bufin = (const unsigned char*)_ToDecode;
    // the nest 3 lines will trim the padding characters ('=')
    nprbytes--;
    while (B64index[bufin[nprbytes]] > 63 && nprbytes >= 0) { nprbytes--; }
    nprbytes++;
    if (nprbytes == 0) return 0;    // only padding characters ('=') found

    nbytesdecoded = ((nprbytes * 3) / 4);
    bufout = (unsigned char*)_Decoded;
    bufin = (const unsigned char*)_ToDecode;

    while (nprbytes > 4) {
        *(bufout++) = (unsigned char)(B64index[*bufin] << 2 | B64index[bufin[1]] >> 4);
        *(bufout++) = (unsigned char)(B64index[bufin[1]] << 4 | B64index[bufin[2]] >> 2);
        *(bufout++) = (unsigned char)(B64index[bufin[2]] << 6 | B64index[bufin[3]]);
        bufin += 4;
        nprbytes -= 4;
    }

    /* Note: (nprbytes == 1) would be an error, so just ingore that case */
    if (nprbytes > 1) {
        *(bufout++) = (unsigned char)(B64index[*bufin] << 2 | B64index[bufin[1]] >> 4);
    }
    if (nprbytes > 2) {
        *(bufout++) = (unsigned char)(B64index[bufin[1]] << 4 | B64index[bufin[2]] >> 2);
    }
    if (nprbytes > 3) {
        *(bufout++) = (unsigned char)(B64index[bufin[2]] << 6 | B64index[bufin[3]]);
    }

    return nbytesdecoded;
}

/*
* Arguments:
*   _Encoded : a buffer where the encoded data will be stored, it must have a storage of minimum "Base64encode_len(_NunOfBytesToEncode)"
*   _ToEncode : data to be encoded
*   _NunOfBytesToEncode : size in bytes of the data to encode
* Return value:
*   Returns the size of _Encoded
*/
size_t Base64encode_BigEndian(char* _Encoded, const void* _ToEncode, size_t _NunOfBytesToEncode) {
    typedef struct b64_3bytes {
        unsigned char B1 : 2;
        unsigned char A : 6;
        unsigned char C1 : 4;
        unsigned char B2 : 4;
        unsigned char D : 6;
        unsigned char C2 : 2;
    }b64_3bytes;

    register b64_3bytes* buffer_iter;
    register unsigned char* end, * pos;

    end = (unsigned char*)_ToEncode + _NunOfBytesToEncode;
    //in = (unsigned char*)_ToEncode;
    buffer_iter = (b64_3bytes*)_ToEncode;
    pos = (unsigned char*)_Encoded;

    while (end - (unsigned char*)buffer_iter >= 3)  // 28 asm instructions
    {
        //buffer_iter = ((b64_3bytes*)in);
        *pos++ = base64_table[buffer_iter->A];
        *pos++ = base64_table[buffer_iter->B2 + ((buffer_iter->B1) << 4)];
        *pos++ = base64_table[buffer_iter->C2 + ((buffer_iter->C1) << 2)];
        *pos++ = base64_table[buffer_iter->D];
        //in += 3;
        buffer_iter += 1;
    }

    if ((_NunOfBytesToEncode % 3) == 1)
    {
        //buffer_iter = ((b64_3bytes*)in);
        *pos++ = base64_table[buffer_iter->A];
        *pos++ = base64_table[buffer_iter->B2 + ((buffer_iter->B1) << 4)];
        *pos++ = '=';
        *pos++ = '=';
    }
    else
    {
        //buffer_iter = ((b64_3bytes*)in);
        *pos++ = base64_table[buffer_iter->A];
        *pos++ = base64_table[buffer_iter->B2 + ((buffer_iter->B1) << 4)];
        *pos++ = base64_table[buffer_iter->C2 + ((buffer_iter->C1) << 2)];
        *pos++ = '=';
    }
    return (size_t)pos - (size_t)_Encoded;
}