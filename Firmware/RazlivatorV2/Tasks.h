/*
 * Tasks.h
 *
 * Created: 14.10.2013 17:21:42
 *  Author: HMHamster
 */ 


#ifndef TASKS_H_
#define TASKS_H_

#ifndef MAX_TASKS
#define MAX_TASKS 3
#endif

#ifndef TICK_MS
#define TICK_MS 4
#endif

struct Task{
	unsigned int time_counter;
	void (*function)();
};

void TasksTimerTick();
signed char AddTask(unsigned int time_ms,void (*func)());
struct Task tasks[MAX_TASKS];


#endif /* TASKS_H_ */