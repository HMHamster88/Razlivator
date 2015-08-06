#ifndef SERVICE
#define SERVICE
#include <avr/io.h>
//#include <inavr.h>

///////////////////////////
//Частота тактирования МК
///////////////////////////

//#define F_CPU 16000000ul

////////////////////////////
//макросы для задержек
////////////////////////////

//#define _delay_us(us) 	__delay_cycles((F_CPU / 1000000) * (us));
//#define _delay_ms(ms) 	__delay_cycles((F_CPU / 1000) * (ms));


/////////////////////////////
//макросы для работы с битами
/////////////////////////////

#define Bit(bit)  (1<<(bit))

#define ClearBit(reg, bit)       reg &= (~(1<<(bit)))
//пример: ClearBit(PORTB, 1); //сбросить 1-й бит PORTB

#define SetBit(reg, bit)          reg |= (1<<(bit))	
//пример: SetBit(PORTB, 3); //установить 3-й бит PORTB

//#define SetBitVal(reg, bit, val) do{if ((val&1)==0) reg &= (~(1<<(bit)));\
//                                  else reg |= (1<<(bit));}while(0)

#define SetBitVal(reg, bit, val) do{if (val==0) reg &= (~(1<<(bit)));\
                                  else reg |= (1<<(bit));}while(0)

//пример: SetBitVal(PORTB, 3, 1); //установить 3-й бит PORTB
//	  SetBitVal(PORTB, 2, 0); //сбросить 2-й бит PORTB

#define BitIsClear(reg, bit)    ((reg & (1<<(bit))) == 0)
//пример: if (BitIsClear(PORTB,1)) {...} //если бит очищен

#define BitIsSet(reg, bit)       ((reg & (1<<(bit))) != 0)
//пример: if(BitIsSet(PORTB,2)) {...} //если бит установлен

#define InvBit(reg, bit)	  reg ^= (1<<(bit))
//пример: InvBit(PORTB, 1); //инвертировать 1-й бит PORTB


typedef unsigned char uint8;
typedef signed char int8;
typedef unsigned int uint16;
typedef signed int int16;
typedef unsigned long uint32;
typedef signed long int32;

#define nop() __no_operation();

////////////////////////////////
/////======ПРОТОТИПИ======//////
////////////////////////////////

unsigned char dec2bcd(unsigned char d);
unsigned int bcd2dec(unsigned int d);

#endif