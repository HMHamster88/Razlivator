

#define SET_HIGH(BYTE,BIT)	BYTE |=(1<<BIT)
#define SET_LOW(BYTE,PIN)	BYTE &=~(1<<BIT)

#define IS_LOW(BYTE,BIT)	(BYTE&(1<<BIT))
#define IS_HIGH(BYTE,BIT)	!(BYTE&(1<<BIT))

#define SET_PIN_HIGH PORT,PIN  SET_HIGH(PORT##PORT,PIN)
#define SET_PIN_LOW PORT,PIN	SET_LOW(PORT##PORT,PIN)

#define SET_PIN_OUT PORT,PIN SET_HIGH(DDR##PORT,PIN)
#define SET_PIN_IN  PORT,PIN SET_HIGH(DDR##PORT,PIN)

#define IS_PIN_HIGH PORT,PIN IS_HIGH(PIN##PORT,PIN)
#define IS_PIN_LOW PORT,PIN IS_LOW(PIN##PORT,PIN)