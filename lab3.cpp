/*
 * lab3.cpp
 *
 *  Created on: Mar 13, 2019
 *      Author: Nathan Klapstein
 */
 
#include <predef.h>
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <smarttrap.h>
#include <taskmon.h>
#include <NetworkDebug.h>
#include "AD.h"
#include "DA.h"
#include "LCD.h"

extern "C" {
	void UserMain(void * pd);
	void StartTask1(void);
	void Task1Main( void * pd);
	void StartTask2(void);
	void Task2Main( void * pd);
}

/** 
 * The system works in millivolts and not volts.
 *
 * Thus, these values have been adjusted to accommodate this.
 */
#define USER_MAIN_VOLTS 3000
#define TASK1_VOLTS		2000
#define TASK2_VOLTS		1000

/* User task priorities always based on MAIN_PRIO */
/* The priorities between MAIN_PRIO and the IDLE_PRIO are available */
#define DUMMYTASK1_PRIO 	MAIN_PRIO + 1
#define DUMMYTASK2_PRIO 	MAIN_PRIO + 2

/* Task stacks for all the user tasks */
/* If you add a new task you'll need to add a new stack for that task */
DWORD DummyTask1Stk[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );
DWORD DummyTask2Stk[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );

const char * AppName="Nathan Klapstein, Thomas Lorincz";

static const point origin = {0, 0};

AD myAD;
DA myDA;
LCD myLCD;

/**
 * Compute the photocell level bar length from a given photocell value.
 *
 * @param photocellValue should range between 2000 (min) and 3100 (max)
 *
 * @returns photocell level bar length that corresponds to the given photocell value.
 */
int GetPhotocellBarLength(int photocellValue) {
	if (photocellValue < 2000) {
		return 0;
	} else if(photocellValue < 2100) {
		return 1;
	} else if(photocellValue < 2200) {
		return 2;
	} else if(photocellValue < 2300) {
		return 3;
	} else if(photocellValue < 2400) {
		return 4;
	} else if(photocellValue < 2500) {
		return 5;
	} else if(photocellValue < 2600) {
		return 6;
	} else if(photocellValue < 2700) {
		return 7;
	} else if(photocellValue < 2800) {
		return 8;
	} else if(photocellValue < 2900) {
		return 9;
	} else if(photocellValue < 3000) {
		return 10;
	} else if(photocellValue < 3100) {
		return 11;
	} else {
		return 11;
	}
}

/**
 * Main execution
 *
 * Sets up ADC, DAC, and LCD and begins looping reading the ADC and outputting on the DAC and LCD.
 */
void UserMain(void * pd) {
    InitializeStack();
    OSChangePrio(MAIN_PRIO);
    EnableAutoUpdate();
    StartHTTP();
    EnableTaskMonitor();

    #ifndef _DEBUG
    EnableSmartTraps();
    #endif

    #ifdef _DEBUG
    InitializeNetworkGDB_and_Wait();
    #endif

    iprintf("Application started\n");
    myLCD.Init();
    myLCD.Home();
    myLCD.Clear();

    // print the photocell text and set the level bar to empty on the LCD
    myLCD.DrawString("PHOTOCELL:");
	myLCD.DrawBarGraph(1, 0);

    myAD.Init();
    myDA.Init();
    myDA.DACOutput(0);

    StartTask1();
    StartTask2();

	while (1) {
		// DA stuff start
		myDA.Lock();
		myDA.DACOutput(USER_MAIN_VOLTS);
		// DA stuff end

		// AD stuff here
		myAD.StartAD();

		while (!myAD.ADDone()) {

		}

		myLCD.Clear();
		myLCD.Move(origin);
		myLCD.DrawString("PHOTOCELL:");
		myLCD.DrawBarGraph(1, GetPhotocellBarLength(myAD.GetADResult(0)));

		myAD.StopAD();
		// AD stuff ends

		// DA stuff start
		myDA.DACOutput(0);
		myDA.Unlock();
		// DA stuff end

		OSTimeDly(TICKS_PER_SECOND / 20);
    }
}

/**
 * Creates the task main loop.
 */
void StartTask1(void) {
	BYTE err = OS_NO_ERR;

	err = display_error("StartTask1 fail:",
					OSTaskCreatewName(Task1Main,
					(void *)NULL,
				 	(void *) &DummyTask1Stk[USER_TASK_STK_SIZE],
				 	(void *) &DummyTask1Stk[0],
				 	DUMMYTASK1_PRIO, "Dummy Task 1"));
}

/**
 * NOP loop that can be timed
 *
 * @note Does nothing useful
 *
 * @param void * pd -- pointer to generic data . Currently unused.
 */
void	Task1Main( void * pd) {
	DWORD count = 0 ;

	/* place semaphore usage code inside the loop */
	while (1) {
		myDA.Lock();
		myDA.DACOutput(TASK1_VOLTS);
		for (int i = 0; i < 40000 ; i++) {
			count = count + 1;
		}
		myDA.DACOutput(0);
		myDA.Unlock();
		OSTimeDly(TICKS_PER_SECOND / 20); // changed from / 200 // single tick delay is the smallest possible sleep time
	}
}

/**
 * Creates the task main loop.
 */
void StartTask2(void) {
	BYTE err = OS_NO_ERR;

	err = display_error("StartTask2 fail:",
					OSTaskCreatewName(Task2Main,
					(void *)NULL,
				 	(void *) &DummyTask2Stk[USER_TASK_STK_SIZE],
				 	(void *) &DummyTask2Stk[0],
				 	DUMMYTASK2_PRIO, "Dummy Task 2"));
}

/**
 * NOP loop that can be time
 *
 * @note Does nothing useful
 *
 * @param void * pd -- pointer to generic data . Currently unused.
 */
void	Task2Main( void * pd) {
	DWORD count = 0;

	while (1) {
		myDA.Lock();
		myDA.DACOutput(TASK2_VOLTS);
		for (int i = 0; i < 20000 ; i++) {
			count = count + 1;
		}
		myDA.DACOutput(0);
		myDA.Unlock();
		OSTimeDly(TICKS_PER_SECOND / 20); // changed from / 200
	}
}
