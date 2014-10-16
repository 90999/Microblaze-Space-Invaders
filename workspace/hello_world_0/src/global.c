/*
 * global.c
 *
 *  Created on: Oct 1, 2014
 *      Author: superman
 */

#include "global.h"
#include "bunker.h"
#include "tank.h"
#include "aliens.h"
#include "bullet.h"
#include "charGen.h"
#include "spaceship.h"
#include <time.h>
#include <stdlib.h>
#include "mb_interface.h"

unsigned int * framePointer0;
unsigned score = 0;
Boolean gameOver;


/**
 *	Set the frame pointer that will be written to, so that all files will have access to it
 */
void setFramePointer(unsigned int * framePointer){
	framePointer0 = framePointer;
}


/**
 * clearScreen:
 * ------------
 * Prints black to each pixel in the frame pointer
 */
void clearScreen(void){
	int row=0, col=0;
	//clear screen
	for( row=0; row<480; row++)
		for(col=0; col<640; col++)
			framePointer0[row*640 + col] = BLACK;
}


/**
 * initGame:
 * ---------
 * This function contains things that should only happen once per program
 */
void initGame(void){
	gameOver = F;
	srand(time(NULL)); //seed the random number generator
}

/**
 * Function: initScene
 * -------------------
 * 	Initializes the tank, aliens, bunkers, and bullets when the program
 * 	is programmed to the board.
 *
 */
void initScene(void){
	initTankPos();
	initAliens();
	initBunkers();
	initAlienBullets();
}


/**
 * Function: render
 * -------------------
 * 	Draws the bunker, aliens, and tank to the screen after the program has been initialized
 */
void render(void) {
	drawBunker(0);
	drawBunker(1);
	drawBunker(2);
	drawBunker(3);
	drawAliens();
	drawTank();
	initializeScoreAndLives();
	printNumber(0, 5, 5 + 70);
	drawTankLife(0);
	drawTankLife(1);
	drawTankLife(2);
}

/**
 * incrementScore:
 * ---------------
 * Increases the score by the specified amount, and prints it to the screen
 */
void incrementScore(unsigned amount) {
	setTextColor(GREEN);
	printScore(score+=amount, SCORE_START_X, SCORE_START_Y, T);
}

/**
 * getRandomNumber:
 * ----------------
 * Returns a random number
 */
unsigned getRandomNumber(void){
	return rand();
}

/**
 * gameEnded:
 * ----------
 * Sets the gameOver flag to T
 */

void gameEnded(void){
	gameOver = T;
}

/**
 * game_over:
 * ----------
 * Clears the screen, and prints "GAME_OVER"
 */
void game_over(void){
	microblaze_disable_interrupts(); //no more timer interrupts
	clearScreen();
	printGameOver();
}


