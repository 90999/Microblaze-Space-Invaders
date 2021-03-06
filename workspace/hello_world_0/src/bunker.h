/*
 * bunker.h
 *
 *  Created on: Oct 1, 2014
 *      Author: superman
 */

#ifndef BUNKER_H_
#define BUNKER_H_

#include "global.h"
#include "xbasic_types.h"

/***********************************
 *		  Bunker Macros
 **********************************/
#define BLOCK_SIZE 12

#define BUNKER_START 100
#define BUNKER_SPACE 120
#define BUNKER_WIDTH 48

/***********************************
 *		  Struct Definitions
 **********************************/

/**
 * 	Represents a bunker object on the screen
 * 		- pos_t: position of the bunker on the screen
 * 		- bunkerState: the erosion state of each bunker on screen
 */

typedef struct {
	pos_t position;
	u8 bunkerState[3][4];
}Bunker;


/***********************************
 *		  Function Definitions
 **********************************/
void initBunkers(void);
void drawBunker(u8 num);
void erodeBunker(int x, int y);
void drawBunkerPart(int num, int i, int j);

#endif /* BUNKER_H_ */
