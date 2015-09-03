/*
 * RazlivatorV2.c
 *
 * Created: 29.07.2013 13:47:07
 *  Author: HMHamster
 */ 

#define F_CPU 12000000UL

#include "gpio.h"
#include "RazlivatorV2.h"
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdbool.h>

#include "Ports.h"

#define TIMER0_MAX 256
#define TIMER0_DIVIDER 256
#define TIMER0_FREQ  (F_CPU/TIMER0_DIVIDER/TIMER0_MAX)
#define TIMER0_MS  (1000/TIMER0_FREQ)

#define TICK_MS TIMER0_MS

#include "Tasks.h"

#define BUTTONS_COUNT 7
#define CUP_COUNT 6

bool button_states[BUTTONS_COUNT];
bool need_to_fill[CUP_COUNT];

#define CUP_0_BUTTON_INDEX 0
#define CUP_1_BUTTON_INDEX 1
#define CUP_2_BUTTON_INDEX 2
#define CUP_3_BUTTON_INDEX 3
#define CUP_4_BUTTON_INDEX 4
#define CUP_5_BUTTON_INDEX 5
#define START_BUTTON_INDEX 6

//Servo constants
#define F_CPU_MHZ (F_CPU/1000000)
#define PWM_TIMER_DIVIDER 8
#define PWM_FREQ 50
#define ICR_MAX (F_CPU/PWM_FREQ)/PWM_TIMER_DIVIDER
#define PWM_STEP_FREQ (F_CPU_MHZ/PWM_TIMER_DIVIDER)

#define SERVO_MIN_TIME 880 //6th cup position
#define SERVO_MAX_TIME 3300 //idle position
#define SERVO_RANGE_TIME (SERVO_MAX_TIME - SERVO_MIN_TIME)

#define FIRST_CUP_POSITION 210

//Timings
#define SERVO_MOVE_TIME 500
#define BEFORE_FILL_TIME 200
#define AFTER_FILL_TIME 200
#define ARM_MOVING_TIME 100

#define MILILITER_TIME_MS 30

#define BUTTONS_PULLING_TIME 100

unsigned char resistorValue = 0;
signed char currnt_arm_position;

void SetArmToCupPosition(signed char cup);

signed char GetNearestPositionToFill(){
	char i;
	signed char nearest_position = 127;
	signed char nearest_position_index=-1;
	for(i=0;i<6;i++)
	{
		if(need_to_fill[i]){
			signed char relPos = abs(currnt_arm_position - i);
			if(relPos<nearest_position)
			{
				nearest_position = relPos;
				nearest_position_index = i;
			}
		}
	}
	return nearest_position_index;
}

bool CheckButton(char button)
{
	switch(button)
	{
		case CUP_0_BUTTON_INDEX:
		return PIN_IS_CLEAR(CUP_0_BUTTON);
		case CUP_1_BUTTON_INDEX:
		return PIN_IS_CLEAR(CUP_1_BUTTON);
		case CUP_2_BUTTON_INDEX:
		return PIN_IS_CLEAR(CUP_2_BUTTON);
		case CUP_3_BUTTON_INDEX:
		return PIN_IS_CLEAR(CUP_3_BUTTON);
		case CUP_4_BUTTON_INDEX:
		return PIN_IS_CLEAR(CUP_4_BUTTON);
		case CUP_5_BUTTON_INDEX:
		return PIN_IS_CLEAR(CUP_5_BUTTON);
		case START_BUTTON_INDEX:
		return PIN_IS_CLEAR(START_BUTTON);
		default:
		return false;
	}
}

ISR(TIMER0_OVF_vect)
{
	TasksTimerTick();
}

unsigned int GetPumpingTime()
{
	unsigned int mililiters = 5+35*resistorValue/255;
	return mililiters * MILILITER_TIME_MS;
}

void CheckNextCup()
{
	signed char nearest_position_index = GetNearestPositionToFill();
	signed char position_diff = abs(currnt_arm_position - nearest_position_index);
	SetArmToCupPosition(nearest_position_index);
	if(currnt_arm_position==-1)
	{
		PIN_CLEAR(PUMP_OUT);
	}
	AddTask(position_diff*ARM_MOVING_TIME,Moving);
}

void Moving()
{
	if(currnt_arm_position !=-1)
	{
		need_to_fill[currnt_arm_position]=0;
		if(!CheckButton(currnt_arm_position))
		{
			AddTask(0,CheckNextCup);
		}
		else
		{
			PIN_SET(PUMP_OUT);
			AddTask(BEFORE_FILL_TIME,OpenValve);
		}
	}
}

void OpenValve()
{
	PIN_SET(VALVE_OUT);
	AddTask(GetPumpingTime(),CloseValve);
}

void CloseValve()
{
	PIN_CLEAR(VALVE_OUT);
	AddTask(AFTER_FILL_TIME,CheckNextCup);
}

ISR(ADC_vect)
{
	resistorValue = ADCH;		
}

void SetArmToCupPosition(signed char cup)
{
	unsigned char servoVal;
	currnt_arm_position = cup;
	if(cup == -1)
		servoVal = 255;
	else
		servoVal = FIRST_CUP_POSITION - cup * FIRST_CUP_POSITION / 5;
		
	OCR1A = SERVO_MIN_TIME*PWM_STEP_FREQ + (SERVO_RANGE_TIME*PWM_STEP_FREQ )*servoVal/255;
}

void ButtonPressed(char button)
{
	if(button == START_BUTTON_INDEX)
	{
		
	}
	else
	{
		need_to_fill[button] = 1;
	}
}

void ButtonReliased(char button)
{
	if(button == START_BUTTON_INDEX)
	{
		AddTask(0, CheckNextCup);
	}
	else
	{
		need_to_fill[button] = 0;
	}
}

bool buttons[BUTTONS_COUNT];

void ButtonsPullingFirstCheck()
{
	char i;
	for(i=0;i<BUTTONS_COUNT;i++)
	{
		buttons[i] = CheckButton(i);
	}
	AddTask(BUTTONS_PULLING_TIME, ButtonsPullingSecondCheck);
}
	
void ButtonsPullingSecondCheck(){
	char i;
	for(i=0;i<BUTTONS_COUNT;i++)
	{
		if(CheckButton(i)==buttons[i] && buttons[i]!=button_states[i])
		{
			button_states[i] = buttons[i];
			if(buttons[i])
				ButtonPressed(i);
			else
				ButtonReliased(i);
		}
	}
	AddTask(0,ButtonsPullingFirstCheck);
}

int main(void)
{
	sei();
	
	TCCR0B = (1<<CS02);
	
	TIMSK0 = (1<<TOIE0);
	
	ICR1 = ICR_MAX; 
	
	TCCR1A = (1<<WGM11)|(1<<COM1A1) ;
	TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<<CS11);
	
	INIT_PIN(VALVE_OUT, OUT, LOW);
	INIT_PIN(PUMP_OUT, OUT, LOW);
	INIT_PIN(PWM_OUT, OUT, LOW);
	
	INIT_PIN(CUP_0_BUTTON, IN,PULL_UP);
	INIT_PIN(CUP_1_BUTTON, IN,PULL_UP);
	INIT_PIN(CUP_2_BUTTON, IN,PULL_UP);
	INIT_PIN(CUP_3_BUTTON, IN,PULL_UP);
	INIT_PIN(CUP_4_BUTTON, IN,PULL_UP);
	INIT_PIN(CUP_5_BUTTON, IN,PULL_UP);
	INIT_PIN(START_BUTTON, IN,PULL_UP);

	
	ADMUX = (1<<ADLAR)|(1<<MUX2)|(1<<MUX1);
	ADCSRA = (1<<ADEN)|(1<<ADIE)|(1<<ADSC)|(3<<ADPS0)|(1<<ADATE);
	
	char i;
	for(i=0;i< CUP_COUNT;i++)
	{
		need_to_fill[i] =0;
	}
	for(i=0;i< BUTTONS_COUNT;i++)
	{
		buttons[i]=1;
		button_states[i]=1;
	}		
	
	SetArmToCupPosition(-1);
	
	AddTask(0,ButtonsPullingFirstCheck);
	
    while(1)
    {		

    }
}