#ifndef SERVICE
#define SERVICE
#include <avr/io.h>
//#include <inavr.h>

///////////////////////////
//������� ������������ ��
///////////////////////////

//#define F_CPU 16000000ul

////////////////////////////
//������� ��� ��������
////////////////////////////

//#define _delay_us(us) 	__delay_cycles((F_CPU / 1000000) * (us));
//#define _delay_ms(ms) 	__delay_cycles((F_CPU / 1000) * (ms));


/////////////////////////////
//������� ��� ������ � ������
/////////////////////////////

#define Bit(bit)  (1<<(bit))

#define ClearBit(reg, bit)       reg &= (~(1<<(bit)))
//������: ClearBit(PORTB, 1); //�������� 1-� ��� PORTB

#define SetBit(reg, bit)          reg |= (1<<(bit))	
//������: SetBit(PORTB, 3); //���������� 3-� ��� PORTB

//#define SetBitVal(reg, bit, val) do{if ((val&1)==0) reg &= (~(1<<(bit)));\
//                                  else reg |= (1<<(bit));}while(0)

#define SetBitVal(reg, bit, val) do{if (val==0) reg &= (~(1<<(bit)));\
                                  else reg |= (1<<(bit));}while(0)

//������: SetBitVal(PORTB, 3, 1); //���������� 3-� ��� PORTB
//	  SetBitVal(PORTB, 2, 0); //�������� 2-� ��� PORTB

#define BitIsClear(reg, bit)    ((reg & (1<<(bit))) == 0)
//������: if (BitIsClear(PORTB,1)) {...} //���� ��� ������

#define BitIsSet(reg, bit)       ((reg & (1<<(bit))) != 0)
//������: if(BitIsSet(PORTB,2)) {...} //���� ��� ����������

#define InvBit(reg, bit)	  reg ^= (1<<(bit))
//������: InvBit(PORTB, 1); //������������� 1-� ��� PORTB


typedef unsigned char uint8;
typedef signed char int8;
typedef unsigned int uint16;
typedef signed int int16;
typedef unsigned long uint32;
typedef signed long int32;

#define nop() __no_operation();

////////////////////////////////
/////======���������======//////
////////////////////////////////

unsigned char dec2bcd(unsigned char d);
unsigned int bcd2dec(unsigned int d);

#endif