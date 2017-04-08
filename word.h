/* 
 * File:   word.h
 * Author: kpit
 *
 * Created on February 18, 2017, 2:36 AM
 */
//#include <stdbool.h>
#ifndef WORD_H
#define	WORD_H

#ifdef	__cplusplus
extern "C" {
#endif

/** @ingroup util_word
    Return low word of a 32-bit integer.

    @param unsigned long ww (0x00000000..0xFFFFFFFF)
    @return low word of input (0x0000..0xFFFF)
*/
static unsigned int lowWord(unsigned long ww)
{
  return (unsigned int) ((ww) & 0xFFFF);
}


/** @ingroup util_word
    Return high word of a 32-bit integer.

    @param unsigned long ww (0x00000000..0xFFFFFFFF)
    @return high word of input (0x0000..0xFFFF)
*/
static unsigned int highWord(unsigned long ww)
{
  return (unsigned int) ((ww) >> 16);
}

static unsigned char highByte(unsigned int ww)
{
  return (unsigned char) ((ww) >> 8);
}

static unsigned char lowByte(unsigned int ww)
{
  return (unsigned char) ((ww) & 0xFF);
}


#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

static unsigned int word(unsigned char highByte,unsigned char lowByte)
{
    return (unsigned int) ((((unsigned int) highByte)<<8) & 0xFF00) | (((unsigned int) lowByte)&0x00FF);
}


#ifdef	__cplusplus
}
#endif

#endif	/* WORD_H */

