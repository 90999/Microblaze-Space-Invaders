/*
 * global.h
 *
 *  Created on: Oct 9, 2014
 *      Author: superman
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

extern unsigned int * framePointer0;

/**
 * Bug list to fix:
 *
 * 1.)Check entire bullet, not just move amount for collision detection
 * 2.)Start from bottom of bullet to top for hit detection
 * 3.)Artifacts in bunkers occasionally?  look into this more
 * 		This is because erasing bullets can erase other bunkers...
 * 4.)Leftmost column extending all the way to the left of the screen
 * 5.)When writing 1 on the score, it messes up the score
 * 6.)When a column is completely destroyed, do not fire alien bullets from it!
 * 7.)Paint bullets off white/
 */

 /*****************************************************
 * 				     Pack functions
 * 	Used to create 32, 16, and 8 bit values for bitmaps
 *******************************************************/
#define packWord32(b31,b30,b29,b28,b27,b26,b25,b24,b23,b22,b21,b20,b19,b18,b17,b16,b15,b14,b13,b12,b11,b10,b9,b8,b7,b6,b5,b4,b3,b2,b1,b0) \
((b31 << 31) | (b30 << 30) | (b29 << 29) | (b28 << 28) | (b27 << 27) | (b26 << 26) | (b25 << 25) | (b24 << 24) |						  \
 (b23 << 23) | (b22 << 22) | (b21 << 21) | (b20 << 20) | (b19 << 19) | (b18 << 18) | (b17 << 17) | (b16 << 16) |						  \
 (b15 << 15) | (b14 << 14) | (b13 << 13) | (b12 << 12) | (b11 << 11) | (b10 << 10) | (b9  << 9 ) | (b8  << 8 ) |						  \
 (b7  << 7 ) | (b6  << 6 ) | (b5  << 5 ) | (b4  << 4 ) | (b3  << 3 ) | (b2  << 2 ) | (b1  << 1 ) | (b0  << 0 ) )

#define packWord16(b15,b14,b13,b12,b11,b10,b9,b8,b7,b6,b5,b4,b3,b2,b1,b0) \
((b15 << 15) | (b14 << 14) | (b13 << 13) | (b12 << 12) | (b11 << 11) | (b10 << 10) | (b9  << 9 ) | (b8  << 8 ) |						  \
 (b7  << 7 ) | (b6  << 6 ) | (b5  << 5 ) | (b4  << 4 ) | (b3  << 3 ) | (b2  << 2 ) | (b1  << 1 ) | (b0  << 0 ) )

#define packWord8(b7,b6,b5,b4,b3,b2,b1,b0) \
 ((b7  << 7 ) | (b6  << 6 ) | (b5  << 5 ) | (b4  << 4 ) | (b3  << 3 ) | (b2  << 2 ) | (b1  << 1 ) | (b0  << 0 ) )

/***********************************
 *		  Global Macros
 **********************************/
/*Colors*/
#define GREEN 0x0000FF00
#define WHITE 0x00FFFFFF
#define RED   0x00FF0000;
#define BLACK 0
#define OFF_WHITE 0x00FFFFFD //Used for alien bullets to prevent tank bullets detecting a collision

/*Score macros*/
#define SCORE_START_X 5
#define SCORE_START_Y (5 + 70)

/*Point directly below the bunkers*/
#define BELOW_THE_BUNKERS (360 + 36)


/***********************************
 *      Function definitions
 ***********************************/
void initScene(void);
void initGame(void);
void render(void);
void clearScreen(void);
void setFramePointer(unsigned int * framePointer);
void incrementScore(unsigned amount);
unsigned getRandomNumber(void);
void game_over(void);
void gameEnded(void);
/****************************
 *    Struct definitions
 ****************************/

/**
 * Boolean value
 */
typedef enum {
	F, T
}Boolean;

/**
 * Direction:
 * ----------
 * Alien move direction
 */
typedef enum {
	LEFT, RIGHT
}Direction;

/**
 * Represents a position on the screen:
 *	 - x corresponds to a row
 *   - y corresponds to a column
 */
typedef struct {
	unsigned short x;
	unsigned short y;
}pos_t;


#endif /* GLOBAL_H_ */
