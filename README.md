# UTF-8 Decoding of Unicode

This program decodes a UTF-8 string and returns it's corresponding
Unicode code point (Ex: the string 0xCE, 0xA9 returns `U+03A9`).
The goal for this program is to explain the concept of UTF-8 and
the process of decoding a UTF-8 string into a Unicode character
at the bit level.

## Background
UTF-8 is an encoding standard that is used for electronic communication.
It is most commonly used for encoding characters, using a variable-length
encoding ranging from one to four bytes. It is an extention to the original
ASCII standard and it is also backwards compatible. There was also another
encoding aimed to extend ASCII known as ISO/IEC 8859, however, it was
quickly taken over by UTF-8 due to it's fragmented design. The problem
with ISO 8859 was that it had different versions for specific regions,
which made it impossible to mix different languages in one document.

## Decoding Process
To begin decoding, we must convert our string to binary
and then combine them into 8-bit chunks. This is not done
in the code, but it is shown here to demonstrate the concept.
Ex: CEA9

```
0xCE        0xA9
1100 1110   1010 1001
11001110    10101001
```

The first byte is very important. It will determine how many bytes there are in the string.
This will be necessary to determine the number of continuation bits.
Here is a chart which shows the number of bytes in the string as determined by the first bit.

| First Byte  | bytes   |
|------------:|--------:|
| 0 xxxxxxx   | 1       |
| 110 xxxxx   | 2       |
| 1110 xxxx   | 3       |
| 11110 xxx   | 4       |

Here are the conditions in the code to check for this at the bit level
```c
if      (0x00 <= value && value <= 0x7F)    retval = 1;
else if (0xC0 <= value && value <= 0xDF)    retval = 2;
else if (0xE0 <= value && value <= 0xEF)    retval = 3;
else if (0xF0 <= value && value <= 0xF7)    retval = 4;
else                                        retval = 0;

return retval;
```

In this case, the first byte is prefixed with 110
`1100 - 1110    10 - 101001`

Notice that the last byte is prefixed with 10. This is a continuation byte.
Every continuation byte is prefixed with 10. In the program, we use the following
conditon to check if the continuation bits are valid. But first, we must eliminate
the framing bits from the first byte, which in this case are 110x - xxxx.
The following conditions is used to do this.

```c
if      (0x00 <= value && value <= 0x7F)    retval = value;
else if (0xC0 <= value && value <= 0xDF)    retval = value & 0x1F;
else if (0xE0 <= value && value <= 0xEF)    retval = value & 0xF;
else if (0xF0 <= value && value <= 0xF7)    retval = value & 0x7;
else                                        retval = 0;

return retval;

sum += bytes[0];    // this is done outside of the function
                    // once the framing bits are eliminated.
```

and to check if the continuation bytes are valid

```c
value = value & 0xC0;   // apply a bitwise AND to only keep the 10 in 10xx xxxx
if (value == 0x80) retval = 1;
return retval;
```

Finally, we removing the framing bits from the continuation bytes
and sum all of the bytes, which will equal our Unicode character in hexadecimal.

```c
// removing framing bits
bytes[i] = bytes[i] & 0x3F;

// add to sum
sum = sum << 6;     // apply a left-shift operation to easily add the bits to the sum
sum += bytes[i];    // add the bits to the sum
```

Now we get our final value
```
1110 101001
U+03A9
```
