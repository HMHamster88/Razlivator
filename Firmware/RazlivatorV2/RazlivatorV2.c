/*
 * RazlivatorV2.c
 *
 * Created: 29.07.2013 13:47:07
 *  Author: HMHamster
 */ 

#define F_CPU 16000000UL

#include "gpio.h"
#include "RazlivatorV2.h"
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include "Ports.h"

#define TIMER0_MAX 256
#define TIMER0_DIVIDER 256
#define TIMER0_FREQ  (F_CPU/TIMER0_DIVIDER/TIMER0_MAX)
#define TIMER0_MS  (1000/TIMER0_FREQ)

#define TICK_MS TIMER0_MS

#include "Tasks.h"

#define bool char

bool button_states[6];
bool need_to_fill[6];

//Servo constants
#define F_CPU_MHZ (F_CPU/1000000)
#define PWM_TIMER_DIVIDER 8
#define PWM_FREQ 50
#define ICR_MAX (F_CPU/PWM_FREQ)/PWM_TIMER_DIVIDER
#define PWM_STEP_FREQ (F_CPU_MHZ/PWM_TIMER_DIVIDER)

#define SERVO_MIN_TIME 600 //6th cup position
#define SERVO_MAX_TIME 2300 //idle position
#define SERVO_RANGE_TIME (SERVO_MAX_TIME - SERVO_MIN_TIME)

#define FIRST_CUP_POSITION 225

//Timings
#define SERVO_MOVE_TIME 500
#define BEFORE_FILL_TIME 200
#define AFTER_FILL_TIME 200
#define ARM_MOVING_TIME 100

#define MILILITER_TIME_MS 30


unsigned char resistorValue = 0;
signed char currnt_arm_position;

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
	if(button==0)
	return PIN_IS_CLEAR(BUTTON_1);
	if(button==1)
	return PIN_IS_CLEAR(BUTTON_2);
	if(button==2)
	return PIN_IS_CLEAR(BUTTON_3); 
	if(button==3)
	return PIN_IS_CLEAR(BUTTON_4);
	if(button==4)
	return PIN_IS_CLEAR(BUTTON_5);
	if(button==5)
	return PIN_IS_CLEAR(BUTTON_6);
	return 0;
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


void Idle(){
	signed char nearest_position_index = GetNearestPositionToFill();
	
	if(nearest_position_index==0)
	{
		AddTask(0,CheckNextCup);
	}
	else
	{
		AddTask(200,Idle);
	}
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
	if(currnt_arm_position==-1)
	{
		AddTask(200,Idle);
	}
	else
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
		servoVal = FIRST_CUP_POSITION - cup*FIRST_CUP_POSITION/5;
		
	OCR1A = SERVO_MIN_TIME*PWM_STEP_FREQ + (SERVO_RANGE_TIME*PWM_STEP_FREQ )*servoVal/255;
}

void ButtonPressed(char button){
	need_to_fill[button] = 1;
}

void ButtonReliased(char button){
}

bool buttons[6];

void ButtonsPullingFirstCheck(){
	char i;
	for(i=0;i<6;i++)
	{
		buttons[i] = CheckButton(i);
	}
	AddTask(500,ButtonsPullingSecondCheck);
	}
	
void ButtonsPullingSecondCheck(){
	char i;
	for(i=0;i<6;i++)
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
	
	TCCR0 = (1<<CS02);
	
	TIMSK = (1<<TOIE0);
	
	ICR1 = ICR_MAX; 
	
	TCCR1A = (1<<WGM11)|(1<<COM1A1) ;
	TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<<CS11);
	
	INIT_PIN(VALVE_OUT, OUT, LOW);
	INIT_PIN(PUMP_OUT, OUT, LOW);
	INIT_PIN(PWM_OUT, OUT, LOW);
	
	INIT_PIN(CUP_IN, IN,PULL_UP);
	INIT_PIN(BUTTON_1, IN,PULL_UP);
	INIT_PIN(BUTTON_2, IN,PULL_UP);
	INIT_PIN(BUTTON_3, IN,PULL_UP);
	INIT_PIN(BUTTON_4, IN,PULL_UP);
	INIT_PIN(BUTTON_5, IN,PULL_UP);
	INIT_PIN(BUTTON_6, IN,PULL_UP);

	
	ADMUX = (1<<ADLAR);// |(1<<MUX0);
	ADCSRA = (1<<ADEN)|(1<<ADIE)|(1<<ADSC)|(3<<ADPS0)|(1<<ADFR);
	
	char i;
	for(i=0;i<6;i++)
	{
		need_to_fill[i] =0;
		buttons[i]=1;
		button_states[i]=1;
	}		
	
	SetArmToCupPosition(-1);
	
	AddTask(0,Idle);
	AddTask(0,ButtonsPullingFirstCheck);
	
    while(1)
    {		

    }
}