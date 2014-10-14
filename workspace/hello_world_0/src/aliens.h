/*
 * aliens.h
 *
 *  Created on: Oct 1, 2014
 *      Author: superman
 */

#ifndef ALIENS_H_
#define ALIENS_H_

#include "global.h"
#include "xbasic_types.h"

/***********************************
 *		  Alien Macros
 **********************************/
#define ALIEN_HEIGHT 16
#define ALIEN_WIDTH 32
#define VERTICAL_MOVE 10
#define HORIZONTAL_MOVE 5
#define ALIEN_HALF_WIDTH 12
#define ALIEN_VERTICAL_SPACING 10
#define ALIEN_HORIZONAL_SPACING 10
#define MAX_ALIEN_SPEED 8
#define SPEED_UP_AMOUNT 2
#define EXPLOSION_WIDTH 24
#define MAX_BULLET_RATE 70

#define TOP_ALIEN_WIDTH 16
#define MIDDLE_ALIEN_WIDTH 22
#define BOTTOM_ALIEN_WIDTH 24

/***********************************
 *		  Type Definitions
 **********************************/

/**
 * Aliens:
 * -------
 * Alien block information:
 *
 * 	- Direction: current move direction of the alien block
 * 	- prev_position: previous position of the alien block (for erasing)
 * 	- position : current position of the alien block
 * 	- alive: boolean array telling us which aliens are alive and dead
 * 	- columns: the number of alive alien columns
 * 	- rows: the number of alive alien rows
 * 	- colStart: start column of the alien block
 * 	- aliensIn: current bitmap state of the aliens
 * 	- lowestAlien : array of the lowest alien in each column
 * 	- movingDown : true if the aliens just moved down
 * 	- killCount : used to keep track of how many aliens have been killed
 */
typedef struct {
	Direction moveDir;
	pos_t prev_position;
	pos_t position;
	Boolean alive[55];
	Boolean alienIn;
	Boolean movingDown;
	unsigned short columns;
	unsigned short rows;
	unsigned short colStart;
	u8 rowHits [5];
	int lowestAlien[11];
	u8 killCount;
	unsigned short bottomRowPos;
}Aliens;

/***************************************
 *	  Alien function definitions
 ***************************************/
void initAliens(void);
void killAlien(int index);
void drawAliens(void);
void moveAliens(void);
void launchAlienBullet(void);
void drawAlienExplosion(unsigned short row, unsigned short col);
void deleteAlienPart(int x, int y, int alienWidth);
void eraseAlienExplosion(void);
unsigned short getAlienIndex(unsigned short x, unsigned short y);

#endif /* ALIENS_H_ */