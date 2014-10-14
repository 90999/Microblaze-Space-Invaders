/*
 * spaceship.c
 *
 *  Created on: Oct 11, 2014
 *      Author: superman
 */

#include "spaceship.h"
#include "charGen.h"

static Direction moveDir;
static unsigned yPos;
static unsigned randomScore[7] = {
		50, 100, 150, 200, 250, 300, 350,
};

static u8 animationState = 0;
static unsigned currentBonus = 0;
unsigned spaceshipTimerExpired = SHOW_SPACESHIP;

const int alienSpaceship[SPACESHIP_HEIGHT] = {
	packWord32(0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0),
	packWord32(0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0),
	packWord32(0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0),
	packWord32(0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0),
	packWord32(0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0),
	packWord32(0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0),
	packWord32(0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0),
	packWord32(0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0),
	packWord32(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
	packWord32(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1),
	packWord32(0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0),
	packWord32(0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0),
	packWord32(0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0),
	packWord32(0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0),
};


/**
 *
 */
void placeSpaceship(void){

	spaceshipTimerExpired = MOVE_SPACESHIP;
	moveDir = (getRandomNumber() % 2) ? RIGHT : LEFT;
	yPos = (moveDir == RIGHT) ? START_LEFT : START_RIGHT ;
	drawSpaceship();
}

/*
 *
 */
void drawSpaceship(void){
	int row, col, i, j;

	for( row = SPACESHIP_ROW, i = 0 ; row < SPACESHIP_ROW + SPACESHIP_HEIGHT; row++, i++)
		for(col = yPos, j = 0; col < yPos + SPACESHIP_WIDTH; col++, j++) {
			framePointer0[row*640 + col] = !(alienSpaceship[i] & (1 << j)) ? BLACK : RED;  //todo: maybe instead of drawing black, draw nothing..
	}
}

/**
 *
 */
void moveSpaceship(void){
	if(moveDir == RIGHT)
		moveSpaceshipRight();
	else
		moveSpaceshipLeft();
}

/*
 *
 */
void moveSpaceshipRight(void){
	int row, col;

	if ( yPos > START_RIGHT )
		eraseSpaceship(F); //may need to do other thing here
	else {
		//erase part of the spaceship
		for( row = SPACESHIP_ROW; row < SPACESHIP_ROW + SPACESHIP_HEIGHT; row++)
			for(col = yPos; col < yPos + SPACESHIP_MOVE_AMOUNT; col++)
				framePointer0[row*640 + col] = BLACK;

		yPos += SPACESHIP_MOVE_AMOUNT;

		drawSpaceship();
	}
}
/*
 *
 */
void moveSpaceshipLeft(void){
	int row, col;

	if ( yPos < START_LEFT )
		eraseSpaceship(F);
	else {
		//erase part of the spaceship
		for( row = SPACESHIP_ROW; row < SPACESHIP_ROW + SPACESHIP_HEIGHT; row++)
			for(col = yPos + SPACESHIP_WIDTH - SPACESHIP_MOVE_AMOUNT ; col < yPos + SPACESHIP_WIDTH; col++)
				framePointer0[row*640 + col] = BLACK;

		yPos -= SPACESHIP_MOVE_AMOUNT;

		drawSpaceship();
	}
}

/*
 *
 */
void eraseSpaceship(Boolean hit){
	int row, col, i, j;

	for( row = SPACESHIP_ROW, i = 0 ; row < SPACESHIP_ROW + SPACESHIP_HEIGHT; row++, i++)
		for(col = yPos, j = 0; col < yPos + SPACESHIP_WIDTH; col++, j++)
			framePointer0[row*640 + col] = BLACK;

	spaceshipTimerExpired = (hit == T) ? ANIMATE_BONUS : SHOW_SPACESHIP;
}

/**
 *
 */
void spaceshipHit(void){

	eraseSpaceship(T);
	currentBonus = randomScore[getRandomNumber()%7] ; //maybe change this to be a lower score
	incrementScore(currentBonus);
}

/**
 *
 */
void animateBonus(void){

	if(++animationState & 1 || (animationState > 5 && animationState < 16)){
		setTextColor(OFF_WHITE);
		printScore(currentBonus, SPACESHIP_ROW, yPos, F);
	}
	else
		eraseNumber(3, SPACESHIP_ROW, yPos);

	if(animationState == 16) {
		spaceshipTimerExpired = SHOW_SPACESHIP;
		animationState = 0;
	}

}

Boolean spaceshipInAir(void){
	if(spaceshipTimerExpired == MOVE_SPACESHIP)
		return T;

	return F;
}


