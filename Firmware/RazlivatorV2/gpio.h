#ifndef GPIO_
#define GPIO_

#include "service.h"

#define HIGH 1
#define LOW 0
#define PULL_UP 1
#define HI_Z 0

/*===========================================================================*/
/*================Блок определений для инициализации пина====================*/
/*===========================================================================*/
/*
Формат назначения пина - (#define PIN_N PORT, BIT)
Формат инициализации - (INIT_PIN(PIN_N, DIRECTION, INIT_LEVEL))

например: #define OUT1 A, 0
          INIT_PIN(OUT1, OUT, HIGH);

          #define INPUT D, 7
          INIT_PIN(INPUT, IN, HI_Z);
*/

#define INIT_PIN(pin_settings, dir, lev) GPIO_INIT_PIN(pin_settings, dir, lev)

#define GPIO_INIT_PIN(port, pin, dir, lev) \
        GPIO_INIT_PIN_##dir(port, pin, lev)
//Настройка выхода:
#define GPIO_INIT_PIN_OUT(port, pin, lev) \
{\
  SetBitVal(PORT##port, pin, lev); \
  SetBit(DDR##port, pin); \
}
//Настройка входа:
#define GPIO_INIT_PIN_IN(port, pin, lev) \
{\
  SetBitVal(PORT##port, pin, lev); \
  ClearBit(DDR##port, pin); \
}
/*===========================================================================*/


/*===========================================================================*/
/*=================Блок определений для управления пином=====================*/
/*===========================================================================*/
/*
Формат - (PIN_[ACTION](PIN_N))

например: PIN_SET(OUT1);
          PIN_CLEAR(OUT1);
          PIN_VAL(INPUT, HI_Z);
*/

//Установка:
#define PIN_SET(pin_settings)\
        GPIO_PIN_SET(pin_settings)
#define GPIO_PIN_SET(port, pin)\
{\
  SetBit(PORT##port, pin); \
}

//Очистка:
#define PIN_CLEAR(pin_settings)\
        GPIO_PIN_CLEAR(pin_settings)
#define GPIO_PIN_CLEAR(port, pin)\
{\
  ClearBit(PORT##port, pin); \
}

//Инвертировать:
#define PIN_INV(pin_settings)\
        GPIO_PIN_INV(pin_settings)
#define GPIO_PIN_INV(port, pin)\
{\
  InvBit(PORT##port, pin); \
}

//Присвоить значение:
#define PIN_VAL(pin_settings, value)\
        GPIO_PIN_VAL(pin_settings, value)
#define GPIO_PIN_VAL(port, pin, value)\
{\
  SetBitVal(PORT##port, pin, value); \
}
/*===========================================================================*/


/*===========================================================================*/
/*==============Блок определений для опроса состояния пина===================*/
/*===========================================================================*/
#define PIN_IS_SET(pin_settings)\
        GPIO_PIN_IS_SET(pin_settings)
#define GPIO_PIN_IS_SET(port, pin)\
  (BitIsSet(PIN##port, pin))

#define PIN_IS_CLEAR(pin_settings)\
        GPIO_PIN_IS_CLEAR(pin_settings)
#define GPIO_PIN_IS_CLEAR(port, pin)\
  (BitIsClear(PIN##port, pin))
/*===========================================================================*/


/*===========================================================================*/
/*==============Блок определений для инциализации групы пинов================*/
/*===========================================================================*/
/*
Формат назначения пинов - (#define PINS PORT, Bit(BIT_N1)|Bit(BIT_N2)|Bit(BIT_N3))
Формат инициализации - (INIT_PINS(PINS, DIRECTION, INIT_LEVEL))

например: #define OUTS B, Bit(0)|Bit(1)|Bit(7)
          INIT_PINS(OUTS, OUT, HIGH);

          #define BUTTONS D, Bit(4)|Bit(5)|Bit(6)|Bit(7)
          INIT_PINS(BUTTONS, IN, PULL_UP);
*/

#define INIT_PINS(pins_settings, dir, lev) GPIO_INIT_PINS(pins_settings, dir, lev)

#define GPIO_INIT_PINS(port, pins, dir, lev) \
        GPIO_INIT_PINS_##dir(port, pins, lev)
//Настройка выхода:
#define GPIO_INIT_PINS_OUT(port, pins, lev) \
{\
  if(lev) \
  { \
   PORT##port |= (pins); \
  } \
  else \
  { \
   PORT##port &= ~(pins); \
  }; \
  DDR##port |= pins; \
}
//Настройка входа:
#define GPIO_INIT_PINS_IN(port, pins, lev) \
{\
  if(lev) \
  { \
   PORT##port |= (pins); \
  } \
  else \
  { \
   PORT##port &= ~(pins); \
  }; \
  DDR##port &= ~(pins); \
}
/*===========================================================================*/

#endif