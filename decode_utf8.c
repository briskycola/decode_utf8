// bit masks and other constants
#define UTF8_DATA_BIT_MASK          0xC0
#define UTF8_FRAMING_BIT_MASK       0x3F
#define UTF8_CONTINUATION_BIT_MASK  0x80
#define UTF8_MAX_BYTE_SIZE          0xFF
#define UTF8_TOTAL_BYTES            0x04
#define UTF8_SHIFT_LENGTH           0x06

// min and max ranges for each UTF-8 byte
#define UTF8_BYTE_ONE_MIN           0x00
#define UTF8_BYTE_ONE_MAX           0x7F
#define UTF8_BYTE_TWO_MIN           0xC0
#define UTF8_BYTE_TWO_MAX           0xDF
#define UTF8_BYTE_THREE_MIN         0xE0
#define UTF8_BYTE_THREE_MAX         0xEF
#define UTF8_BYTE_FOUR_MIN          0xF0
#define UTF8_BYTE_FOUR_MAX          0xF7

// bit masks for the utf8 bytes
#define UTF8_BYTE_TWO_MASK          0x1F
#define UTF8_BYTE_THREE_MASK        0xF
#define UTF8_BYTE_FOUR_MASK         0x7

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint8_t bytesToRead(uint8_t value)
{
    /*
    *   verify the no. of bytes to read
    *   based on the framing bit pattern
    *   of the first byte
    *
    *   Ex: 110x xxxx requires 2 bytes
    *   to represent the UTF-8 char
    */

    uint8_t returnValue;
    if      (UTF8_BYTE_ONE_MIN <= value && value <= UTF8_BYTE_ONE_MAX)        returnValue = 1;
    else if (UTF8_BYTE_TWO_MIN <= value && value <= UTF8_BYTE_TWO_MAX)        returnValue = 2;
    else if (UTF8_BYTE_THREE_MIN <= value && value <= UTF8_BYTE_THREE_MAX)    returnValue = 3;
    else if (UTF8_BYTE_FOUR_MIN <= value && value <= UTF8_BYTE_FOUR_MAX)      returnValue = 4;
    else                                                                      returnValue = 0;
    return returnValue;
}

uint8_t isContinuation(uint8_t value)
{
    /*
    *   format of value: | ff dd dddd |
    *   where 'f' denotes a framing bit
    *   where 'd' denotes a data bit
    *
    *   To verify that the input is a continuation
    *   byte, we need to remove the data bits
    *   using bitwise AND.
    *
    *   If the byte is in the form 10xx xxxx,
    *   it is a continuation byte.
    */
    
    return ((value & UTF8_DATA_BIT_MASK) == UTF8_CONTINUATION_BIT_MASK);
}

uint8_t eliminateBits(uint8_t value)
{
    /*
    *   remove all framing bits and preserve
    *   data bits based on the bit pattern
    *
    *   Ex: 110x xxxx will now be
    *   000x xxxx
    */

    uint8_t returnValue;
    if      (UTF8_BYTE_ONE_MIN <= value && value <= UTF8_BYTE_ONE_MAX)        returnValue = value;
    else if (UTF8_BYTE_TWO_MIN <= value && value <= UTF8_BYTE_TWO_MAX)        returnValue = value & UTF8_BYTE_TWO_MASK;
    else if (UTF8_BYTE_THREE_MIN <= value && value <= UTF8_BYTE_THREE_MAX)    returnValue = value & UTF8_BYTE_THREE_MASK;
    else if (UTF8_BYTE_FOUR_MIN <= value && value <= UTF8_BYTE_FOUR_MAX)      returnValue = value & UTF8_BYTE_FOUR_MASK;
    else                                                                      returnValue = 0;
    return returnValue;
}

uint8_t decode(uint8_t *bytes)
{
    uint8_t  i, numOfBytes;
    uint32_t sum;

    i   = 0;
    sum = 0;

    // check if first byte is valid
    if (bytes[0] > UTF8_MAX_BYTE_SIZE) return sum;

    // check for # of continuation bytes to read
    numOfBytes = bytesToRead(bytes[0]);
    if (numOfBytes == 0) return sum;

    // elimiate framing bits from the first byte
    bytes[0] = eliminateBits(bytes[0]);

    // add first byte to sum
    sum += bytes[0];

    for (i = 1; i <= numOfBytes; i++)
    {
        // check if the ith bit is EOF
        if (bytes[i] == 0) break;

        // verify that it is a continuation bit
        if (isContinuation(bytes[i]) == 0) break;

        // removing framing bits
        bytes[i] = bytes[i] & UTF8_FRAMING_BIT_MASK;

        // add to sum
        sum = sum << UTF8_SHIFT_LENGTH;
        sum += bytes[i];
    }

    // print the decoded value
    printf("U+%04X\n", sum);
    return sum;
}

int main(void)
{
    uint8_t i;
    uint8_t *input;
    input = (uint8_t*) malloc(sizeof(uint8_t) * UTF8_TOTAL_BYTES);
    for (i = 0; i < UTF8_TOTAL_BYTES; i++)
    {
        printf("Enter byte #%d: ", i+1);
        scanf("%hhx", &input[i]);
        if (input[i] == 0) break;
    }
    decode(input);
    free(input);
    return 0;
}
