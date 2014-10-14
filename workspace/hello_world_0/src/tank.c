/*
 * tank.c
 *
 *  Created on: Oct 1, 2014
 *      Author: superman
 */

#include "tank.h"
#include "aliens.h"
#include "bunker.h"
#include "spaceship.h"

static unsigned short tankPos;
static pos_t tankBulletPos;
static Boolean bulletInAir = F;

static pos_t tankLivePos[8];
static u8 tankLives;

Boolean tankIsDead;

static u8 animationState = 0;
extern Boolean spaceshipOnScreen;

/***************************
 *	     Tank bitmaps
 ***************************/
int const tankBitmap[TANK_HEIGHT] =
{
	packWord32(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0),
	packWord32(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0),
	packWord32(0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0),
	packWord32(0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0),
	packWord32(0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0),
	packWord32(0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0),
	packWord32(0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0),
	packWord32(0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0),
	packWord32(0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
	packWord32(0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
	packWord32(0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
	packWord32(0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
	packWord32(0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
	packWord32(0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
	packWord32(0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
	packWord32(0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
};

u8 const tankBullet[BULLET_HEIGHT] =
{
		packWord8(0,0,0,0,0,0,1,1),
		packWord8(0,0,0,0,0,0,1,1),
		packWord8(0,0,0,0,0,0,1,1),
		packWord8(0,0,0,0,0,0,1,1),
		packWord8(0,0,0,0,0,0,1,1),
		packWord8(0,0,0,0,0,0,1,1),
		packWord8(0,0,0,0,0,0,1,1),
		packWord8(0,0,0,0,0,0,1,1),
		packWord8(0,0,0,0,0,0,1,1),
		packWord8(0,0,0,0,0,0,1,1),
};

int const tankExplosion1[TANK_HEIGHT] =
{
	packWord32(0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1),
	packWord32(0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1),
	packWord32(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0),
	packWord32(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0),
	packWord32(0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,0,0),
	packWord32(0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,0,0),
	packWord32(0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0),
	packWord32(0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0),
	packWord32(0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,1,1,1,0,0,0,0,0,0,1,1),
	packWord32(0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,1,1,1,0,0,0,0,0,0,1,1),
	packWord32(0,0,0,0,0,0,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0),
	packWord32(0,0,0,0,0,0,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0),
	packWord32(0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0),
	packWord32(0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0),
	packWord32(0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
	packWord32(0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
};

int const tankExplosion2[TANK_HEIGHT] =
{
	packWord32(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0),
	packWord32(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0),
	packWord32(0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0),
	packWord32(0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0),
	packWord32(0,0,0,0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,0),
	packWord32(0,0,0,0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,0),
	packWord32(0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0),
	packWord32(0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0),
	packWord32(0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,0,0,0,0,1,1,0,0,0,0),
	packWord32(0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,0,0,0,0,1,1,0,0,0,0),
	packWord32(0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1),
	packWord32(0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1),
	packWord32(0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0),
	packWord32(0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0),
	packWord32(0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
	packWord32(0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
};

/***************************
 *	  Tank functions
 ***************************/

/**
 * initTankPos:
 * ------------
 * Sets the tank to its default position when the game begins
 */
void initTankPos(){
	u8 i;
	unsigned startTankLivesX = 0 , startTankLivesY = 400 + 70;

	tankPos = TANK_START_POS;
	tankLives = 3;
	tankIsDead = F;

	for (i = 0; i < 8; i++){
		tankLivePos[i].x = startTankLivesX;
		tankLivePos[i].y = startTankLivesY;
		//drawTankLife(startTankLivesX, startTankLivesY);
		if(i ==3){
			startTankLivesX += TANK_HEIGHT + 5;
			startTankLivesY = 400 + 70;
		}
		else {
			startTankLivesY += TANK_WIDTH + 5;
		}
	}

}

void resetTank (void) {
	eraseTankLife(NULL);
	tankPos = TANK_START_POS;
	drawTank();
	tankIsDead = F;
}


/**
 * drawTank:
 * ---------
 * Draws the tank to the frame buffer at its current location
 *
 * framePointer0: Pointer to the location in memory that the vdma controller uses to write pixels to the screen
 *
 * todo: remove framePointer1;
 */
void drawTank(void) {
	int row, col, i, j;

	for( row = TANK_ROW, i = 0 ; row < TANK_ROW + TANK_HEIGHT; row++, i++)
		 for(col = tankPos, j = 0; col < tankPos + TANK_WIDTH; col++, j++)
			// if (tankBitmap[i] & (1 << j)) //if there is a pixel to be written
				 framePointer0[row*640 + col] = tankBitmap[i] & (1 << j) ? GREEN : BLACK;  //todo: maybe instead of drawing black, draw nothing..
}

/**
 *
 */
void drawTankLife(u8 num) {
	int row, col, i, j;

	for( row = tankLivePos[num].x, i = 0 ; row < tankLivePos[num].x + TANK_HEIGHT; row++, i++) {
		 for(col = tankLivePos[num].y, j = 0; col < tankLivePos[num].y + TANK_WIDTH; col++, j++) {
			 framePointer0[row*640 + col] = tankBitmap[i] & (1 << j) ? GREEN : BLACK;
		 }
	}
}

/**
 *
 */
void addTankLife(void){
	if(tankLives < MAX_TANK_LIVES)
		drawTankLife(tankLives++);
}

/**
 *
 */
void eraseTankLife(u8 num){
	int row, col, i, j, x, y;
	x = (num == NULL) ? TANK_ROW : tankLivePos[num].x;
	y = (num == NULL) ? tankPos : tankLivePos[num].y;

	for( row = x, i = 0 ; row < x + TANK_HEIGHT; row++, i++) {
		 for(col = y, j = 0; col < y + TANK_WIDTH; col++, j++) {
			 framePointer0[row*640 + col] = BLACK;
		 }
	}
}
/**
 * moveTankRight:
 * --------------
 * Moves the tank right by a constant number of pixels
 *
 * framePointer0: Pointer to the location in memory that the vdma controller uses to write pixels to the screen
 *
 * todo: check to see if moving tank collides with alien bullet
 */
void moveTankRight(void){
	int row=0, col=0, i=0, j=0;

	if(tankPos <= 600)
		tankPos += TANK_MOVE_AMOUNT;

	//erase old part of tank
	for( row = TANK_ROW, i = 0 ; row < TANK_ROW + TANK_HEIGHT; row++, i++)
		for(col = tankPos - TANK_MOVE_AMOUNT, j = 0; col < tankPos ; col++, j++)
			framePointer0[row*640 + col] = BLACK;

	//redraw tank
	drawTank();
}

/**
 * moveTankLeft:
 * ------------
 * Moves the tank left by a constant number of pixels
 *
 * framePointer0: Pointer to the location in memory that the vdma controller uses to write pixels to the screen
 *
 * todo: check to see if moving tank collides with alien bullet
 */
void moveTankLeft(void){
	int row=0, col=0, i=0, j=0;

	if(tankPos >= 10)
		tankPos -= TANK_MOVE_AMOUNT;

	//erase old parts of tank
	for( row = TANK_ROW, i= 0 ; row < TANK_ROW + TANK_HEIGHT; row++, i++)
		for(col = tankPos + TANK_WIDTH , j = 0; col < tankPos + TANK_WIDTH + TANK_MOVE_AMOUNT ; col++, j++)
			framePointer0[row*640 + col] = BLACK;

	//redraw tank
	drawTank();
}


/**
 *
 */

void killTank(void){

	if (--tankLives == 0){
		//game over, do something
		animateTank();
		gameEnded();
	}
	else {
		eraseTankLife(tankLives);
		if(spaceshipInAir() == T)
			eraseSpaceship(F);
		animateTank();
		tankIsDead = T;
		//erase all bullets on screen
	}
}


/**
*
*/
void animateTank(void){

	unsigned row, col;
	u8 i, j;
	int * animationBitmap = (++animationState & 1) ? tankExplosion1 : tankExplosion2;

	for( row = TANK_ROW, i = 0 ; row < TANK_ROW + TANK_HEIGHT; row++, i++)
		 for(col = tankPos, j = 0; col < tankPos + TANK_WIDTH; col++, j++)
			 framePointer0[row*640 + col] = animationBitmap[i] & (1 << j) ? GREEN : BLACK;
}

/************************************
 *	  Tank bullet functions
 ************************************/

/**
 * launchTankBullet:
 * -----------------
 * Place the tank bullet directly above the tank cannon if there is not already a bullet in the air
 *
 * framePointer0: Pointer to the location in memory that the vdma controller uses to write pixels to the screen
 */
void launchTankBullet(void){

	if (bulletInAir == F) {
		tankBulletPos.x = TANK_ROW-BULLET_HEIGHT ;
		tankBulletPos.y = tankPos + 14;
		drawTankBullet();
		bulletInAir = T;
	}
}

/**
 * moveTankBullet:
 * ---------------
 * If a tank bullet is in the air, move the tank bullet up by a constant number of pixels
 *
 * framePointer0: Pointer to the location in memory that the vdma controller uses to write pixels to the screen
 *
 * todo: check for collisions after the bullet has been moved
 */
void moveTankBullet(void) {
	int row, col, eraseOffset;

	if (bulletInAir == T) {
		tankBulletPos.x -= BULLET_MOVE_AMOUNT;

		//check for collisions
		if (tankBulletPos.x <= OFFSCREEN  || checkBulletCollision() == T){
			bulletInAir = F;
			eraseOffset = BULLET_MOVE_AMOUNT;
			goto SKIP_DRAWING;
		}
		else {
			eraseOffset = BULLET_HEIGHT;
		}

		//redraw bullet
		drawTankBullet();

SKIP_DRAWING:
		//erase bullet part that shouldn't be shown
		for( row = tankBulletPos.x + eraseOffset; row < tankBulletPos.x  + BULLET_HEIGHT + BULLET_MOVE_AMOUNT; row++)
			 for(col = tankBulletPos.y; col < tankBulletPos.y + TANK_BULLET_WIDTH; col++)
				 framePointer0[row*640 + col] = BLACK;
	}
}

/**
 * drawTankBullet:
 * ---------------
 * Draw the tank bullet at its current position
 *
 * framePointer0: Pointer to the location in memory that the vdma controller uses to write pixels to the screen
 */
void drawTankBullet(void){
	int row, col, i, j, x = tankBulletPos.x, y = tankBulletPos.y;

	for( row = x, i = 0 ; row < x + BULLET_HEIGHT; row++, i++)
		 for(col = y, j = 0; col < y + TANK_BULLET_WIDTH; col++, j++)
			 framePointer0[row*640 + col] = tankBullet[i] & (1 << j) ? OFF_WHITE: BLACK;
}


/**
 * todo: check from bottom to top of bullet, rather than top to bottom.
 * 		 also, make sure the bullet is within the alien block! right now, if it strikes
 * 		 an alien bullet, an alien explosion will appear!
 */
Boolean checkBulletCollision(void){
	int row, col;
	//check for collisions
	for( row = tankBulletPos.x + BULLET_MOVE_AMOUNT + BULLET_HEIGHT - 1; row >= tankBulletPos.x ; row--)
		for(col = tankBulletPos.y; col < tankBulletPos.y + TANK_BULLET_WIDTH; col++)
			switch ( framePointer0[row*640 + col] ) {
			case WHITE: //alien collision
				killAlien(getAlienIndex(row, col));
				return T;
			case GREEN: //bunker collision
				erodeBunker(row, col);
				return T;
			case 0x00FF0000: //spaceship collision
				spaceshipHit();
				return T;
				break;
			default:
				break;
			}

	return F;
}
