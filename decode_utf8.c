#include <stdio.h>
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

    uint8_t retval;
    if      (0x00 <= value && value <= 0x7F)    retval = 1;
    else if (0xC0 <= value && value <= 0xDF)    retval = 2;
    else if (0xE0 <= value && value <= 0xEF)    retval = 3;
    else if (0xF0 <= value && value <= 0xF7)    retval = 4;
    else                                        retval = 0;

    return retval;
}

uint8_t isContinuation(uint8_t value)
{
    // format of value: | ff dd dddd |
    // where 'f' denotes a framing bit
    // where 'd' denotes a data bit
    uint8_t retval = 0;

    // eliminate the data bits from value
    value = value & 0xC0;

    // ensure the framing bits are "10"
    if (value == 0x80) retval = 1;
    return retval;
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

    uint8_t retval;

    if      (0x00 <= value && value <= 0x7F)    retval = value;
    else if (0xC0 <= value && value <= 0xDF)    retval = value & 0x1F;
    else if (0xE0 <= value && value <= 0xEF)    retval = value & 0xF;
    else if (0xF0 <= value && value <= 0xF7)    retval = value & 0x7;
    else                                        retval = 0;

    return retval;
}

uint8_t decode(uint8_t bytes[4])
{
    int sum = 0;
    uint8_t numOfBytes;
    uint8_t isCont;

    // check if first byte is valid
    if (bytes[0] > 0xFF) return sum;

    // check for # of continuation bytes to read
    numOfBytes = bytesToRead(bytes[0]);
    if (numOfBytes == 0) return sum;

    // elimiate framing bits from the first byte
    bytes[0] = eliminateBits(bytes[0]);

    // add first byte to sum
    sum += bytes[0];

    for (int i = 1; i <= numOfBytes; i++)
    {
        // check if the ith bit is EOF
        if (bytes[i] == 0) break;

        // verify that it is a continuation bit
        isCont = isContinuation(bytes[i]);
        if (isCont == 0) break;

        // removing framing bits
        bytes[i] = bytes[i] & 0x3F;

        // add to sum
        sum = sum << 6;
        sum += bytes[i];
    }

    // print the decoded value
    printf("U+%04X\n", sum);
    return sum;
}

int main(void)
{
    uint8_t input[4];
    for (int i = 0; i < 4; i++)
    {
        printf("Enter byte #%d: ", i+1);
        scanf("%hhx", &input[i]);
        if (input[i] == 0) break;
    }
    decode(input);
    return 0;
}
