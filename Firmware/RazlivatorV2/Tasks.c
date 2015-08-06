/*
 * Tasks.c
 *
 * Created: 14.10.2013 17:26:08
 *  Author: HMHamster
 */ 
#include "Tasks.h"

signed char AddTask(unsigned int time_ms,void (*func)()){
	char i;
	for(i=0;i<MAX_TASKS;i++){
		if(tasks[i].function==0){
			tasks[i].time_counter = time_ms/TICK_MS;
			tasks[i].function = func;
			return i;
		}
	}
	return -1;
}

void TasksTimerTick(){
	char  i;
	for(i=0;i<MAX_TASKS;i++){
		if(tasks[i].function!=0){
			if(tasks[i].time_counter==0){
				void (*func)()=tasks[i].function;
				tasks[i].function = 0;
				func();
			}else{
				tasks[i].time_counter--;
			}
		}
	}
}