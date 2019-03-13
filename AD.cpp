/*
 * AD.cpp
 *
 *  Created on: Feb 9, 2017
 *      Author: nem1
 */

#include "AD.h"
#include <basictypes.h>
#include <sim.h>
#include <stdio.h> // todo: debug

AD::AD() {
	// TODO Auto-generated constructor stub

}

AD::~AD() {
	// TODO Auto-generated destructor stub
}

/**
 * Initializes the entire register map of the AD converters
 */
void AD::Init(void) {
	   volatile WORD vw;

	   //See MCF5441X RM Chapter 29
	   sim2.adc.cr1 = 0;
	   sim2.adc.cr2 = 0;
	   sim2.adc.zccr = 0;
	   sim2.adc.lst1 = 0x3210; //Ch 0....
	   sim2.adc.lst2 = 0x7654; //ch 7 in result 0..7
	   sim2.adc.sdis = 0; //All channels enabled
	   sim2.adc.sr = 0xFFFF;
	   for (int i = 0; i < 8; i++) {
	      vw = sim2.adc.rslt[i];
	      sim2.adc.ofs[i] = 0;
	   }

	   sim2.adc.lsr = 0xFFFF;
	   sim2.adc.zcsr = 0xFFFF;

	   sim2.adc.pwr = 0; //Everything is turned on
	   sim2.adc.cal = 0x0000;
	   sim2.adc.pwr2 = 0x0005;
	   sim2.adc.div = 0x505;
	   sim2.adc.asdiv = 0x13;
}

/**
 * Starts the AD conversion. Only a single sample is onverted.
 */
void AD::StartAD(void) {
	 // Registers to modify for ex2
	  sim2.adc.sr = 0x0000;
	  sim2.adc.cr1 = 0x2000;
}

/**
 * Turn off the AD converter
 */
void AD::StopAD(void) {
	// Registers to modify for ex2
      sim2.adc.cr1 = 0x0000;
}

// stolen
bool isBitSet(int b, int pos) {
   return (b & (1 << pos)) != 0;
}

#define EOSI0_POS 11
#define EOSI1_POS 12

/**
 * This method checks the status of the AD conversion.
 *
 * @returns Should return a boolean: true if the AD conversion is done and false if not done
 */
bool AD::ADDone(void) {
	// Register to check for ex2
	return isBitSet(sim2.adc.sr, EOSI0_POS) &&
			isBitSet(sim2.adc.sr, 0) &&
			isBitSet(sim2.adc.sr, 1) &&
			isBitSet(sim2.adc.sr, 2) &&
			isBitSet(sim2.adc.sr, 3) &&
			isBitSet(sim2.adc.sr, 4) &&
			isBitSet(sim2.adc.sr, 5) &&
			isBitSet(sim2.adc.sr, 6) &&
			isBitSet(sim2.adc.sr, 7);
}

/**
 * Reads the result of the AD conversion once
 * the status register indicates that the conversion is done is ADDone
 */
WORD AD::GetADResult(int ch) {
	// Register to modify for ex2
	// sim2.adc.rslt[ch]
	int result;

	// if the sext
	if (isBitSet(sim2.adc.rslt[ch], 15)) {
		result = -1;
	} else {
		result = 1;
	}
	int mask = 0x7FF8; // 0b0111111111111000;
	result = result * ((sim2.adc.rslt[ch] & mask) >> 3);
	return result;
}
