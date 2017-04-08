/* 
 * File:   crc16.h
 * Author: kpit
 *
 * Created on February 18, 2017, 2:35 AM
 */

#ifndef CRC16_H
#define	CRC16_H

#ifdef	__cplusplus
extern "C" {
#endif

/** @ingroup util_crc16
    Processor-independent CRC-16 calculation.

    Polynomial: x^16 + x^15 + x^2 + 1 (0xA001)<br>
    Initial value: 0xFFFF

    This CRC is normally used in disk-drive controllers.

    @param unsigned int crc (0x0000..0xFFFF)
    @param unsigned char a (0x00..0xFF)
    @return calculated CRC (0x0000..0xFFFF)
*/
static unsigned int crc16_update(unsigned int crc, unsigned char a)
{
  int i;

  crc ^= a;
  for (i = 0; i < 8; ++i)
  {
    if (crc & 1)
      crc = (crc >> 1) ^ 0xA001;
    else
      crc = (crc >> 1);
  }

  return crc;
}



#ifdef	__cplusplus
}
#endif

#endif	/* CRC16_H */

