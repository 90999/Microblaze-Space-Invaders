/*
 * bullet.h
 *
 *  Created on: Oct 1, 2014
 *      Author: superman
 */

#ifndef BULLET_H_
#define BULLET_H_

#include "global.h"
#include "xbasic_types.h"


/***********************************
 *		  Alien Bullet Macros
 **********************************/
#define ALIEN_BULLET_WIDTH 6
#define BULLET_HEIGHT 10
#define BULLET_MOVE_LENGTH 5

/***********************************
 *		  Type Definitions
 **********************************/
/**
 * alien bullet type
 */
typedef enum {
	CROSS, ZIGZAG
}ALIEN_BULLET_TYPE;

/**
 * Alien Bullet :
 * -------
 * Neede info to launch and fire an alien bullet
 *
 * 	- position: current position of the alien bullet
 * 	- type: type of bullet (either cross or zigzag)
 * 	- bulletState: Tracks the animation of the bullet
 * 	- inFlight: true if the bullet is currently in the air
 */
typedef struct {
	pos_t position;
	ALIEN_BULLET_TYPE type;
	u8 bulletState;
	Boolean inFlight;
}AlienBullet;


/***************************************************
 *		  Alien Bullet Function Definitions
 ***************************************************/
void initAlienBullets(void);
void drawAlienBullet(int num);
void moveAlienBullets(void);
void placeAlienBullet(u8 num, int x, int y, Boolean cross);
u8 getFirstAvailableBullet();
void collision(u8 bulletNum, int x, int y);
void eraseAlienBullet(u8 num);
Boolean launchedIntoBunker(unsigned short startX, unsigned short startY );

#endif /* BULLET_H_ */
