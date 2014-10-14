/*
 * Copyright (c) 2009 Xilinx, Inc.  All rights reserved.
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

/*
 * helloworld.c: simple test application
 */

#include <stdio.h>
#include "platform.h"
#include "xparameters.h"
#include "xaxivdma.h"
#include "xio.h"
#include "time.h"
#include "xuartlite_l.h"
#include "unistd.h"
#include "xgpio.h"
#include "mb_interface.h"   // provides the microblaze interrupt enables, etc.
#include "xintc_l.h"
#include "global.h"
#include "aliens.h"
#include "bunker.h"
#include "tank.h"
#include "bullet.h"
#include "spaceship.h"

#define DEBUG
void print(char *str);

#define FRAME_BUFFER_0_ADDR 0xC1000000  // Starting location in DDR where we will store the images that we display.
#define MAX_SILLY_TIMER 3000000;

XGpio gpPB;   // This is a handle for the push-button GPIO block.
u32 buttonState;

#define LEFT 0x8
#define FIRE_TANK_BULLET 0x1
#define RIGHT 0x2


/**************************************************
 * 	 Timers for each moving part on the screen
 **************************************************/
unsigned alienTimer = 0;
extern unsigned alienSpeed;
Boolean moveAlienBlock = F;

unsigned tankTimer = 0;
#define MOVE_TANK 4
Boolean moveTank = F;

unsigned tankBulletTimer = 0;
#define MOVE_TANK_BULLETS 1
Boolean updateTankBullet = F;

unsigned alienBulletTimer = 0;
#define MOVE_ALIEN_BULLETS 4
Boolean updateAlienBullets = F;

unsigned launchAlienBulletTimer = 0;
extern unsigned alienFireRate;//#define LAUNCH_ALIEN_BULLET 75//200
Boolean fireAlienBullet = F;

unsigned tankAnimationTimer = 0;
#define ANIMATE_TANK 10
extern Boolean tankIsDead;
Boolean shouldAnimateTank = F;
u8 tankAnimationCount = 0;

unsigned spaceshipTimer = 0;
extern unsigned spaceshipTimerExpired;
Boolean takeSpaceshipAction = F;

unsigned alienExplosionTimer = 0;
#define ALIEN_EXPLOSION_DONE 6
extern Boolean alienExplosionInProgress;
Boolean shouldEraseAlienExplosion = F;

extern Boolean gameOver;


/**
 *
 */
void timer_interrupt_handler() {

	if(tankIsDead == F) {
		if(++alienTimer == alienSpeed)
			moveAlienBlock = T;
		if(++tankTimer == MOVE_TANK){
			moveTank = T;
			buttonState = XGpio_DiscreteRead(&gpPB, 1); //may want to do this outside interrupt handler
		}
		if(++spaceshipTimer == spaceshipTimerExpired )
			takeSpaceshipAction = T;
		if (++tankBulletTimer == MOVE_TANK_BULLETS) {
			updateTankBullet = T;
		}
		if (++alienBulletTimer == MOVE_ALIEN_BULLETS ){
			updateAlienBullets = T;
		}
		if (++launchAlienBulletTimer >= alienFireRate)
			fireAlienBullet = T;
		if(alienExplosionInProgress == T){
			if(++alienExplosionTimer == ALIEN_EXPLOSION_DONE)
				shouldEraseAlienExplosion = T;
		}
	}
	else {
		if(++tankAnimationTimer == ANIMATE_TANK)
			shouldAnimateTank = T;
	}

}

/**
 *
 */
void interrupt_handler_dispatcher(void* ptr) {
	int intc_status = XIntc_GetIntrStatus(XPAR_INTC_0_BASEADDR);

	// Check the FIT interrupt
	if (intc_status & XPAR_FIT_TIMER_0_INTERRUPT_MASK){
		XIntc_AckIntr(XPAR_INTC_0_BASEADDR, XPAR_FIT_TIMER_0_INTERRUPT_MASK);
		timer_interrupt_handler();
	}
}


int main()
{
	init_platform();                   // Necessary for all programs.
	int Status;                        // Keep track of success/failure of system function calls.

	/*************************
	 *  Initialize vdma controller and frame buffer
	 *************************/

	XAxiVdma videoDMAController;
	// There are 3 steps to initializing the vdma driver and IP.
	// Step 1: lookup the memory structure that is used to access the vdma driver.
    XAxiVdma_Config * VideoDMAConfig = XAxiVdma_LookupConfig(XPAR_AXI_VDMA_0_DEVICE_ID);
    // Step 2: Initialize the memory structure and the hardware.
    if(XST_FAILURE == XAxiVdma_CfgInitialize(&videoDMAController, VideoDMAConfig,	XPAR_AXI_VDMA_0_BASEADDR)) {
    	xil_printf("VideoDMA Did not initialize.\r\n");
    }
    // Step 3: (optional) set the frame store number.
    if(XST_FAILURE ==  XAxiVdma_SetFrmStore(&videoDMAController, 2, XAXIVDMA_READ)) {
    	xil_printf("Set Frame Store Failed.");
    }
    // Initialization is complete at this point.

    // Setup the frame counter. We want two read frames. We don't need any write frames but the
    // function generates an error if you set the write frame count to 0. We set it to 2
    // but ignore it because we don't need a write channel at all.
    XAxiVdma_FrameCounter myFrameConfig;
    myFrameConfig.ReadFrameCount = 2;
    myFrameConfig.ReadDelayTimerCount = 10;
    myFrameConfig.WriteFrameCount =2;
    myFrameConfig.WriteDelayTimerCount = 10;
    Status = XAxiVdma_SetFrameCounter(&videoDMAController, &myFrameConfig);
    if (Status != XST_SUCCESS) {
	   xil_printf("Set frame counter failed %d\r\n", Status);
	   if(Status == XST_VDMA_MISMATCH_ERROR)
		   xil_printf("DMA Mismatch Error\r\n");
    }
    // Now we tell the driver about the geometry of our frame buffer and a few other things.
    // Our image is 480 x 640.
    XAxiVdma_DmaSetup myFrameBuffer;
    myFrameBuffer.VertSizeInput = 480;    // 480 vertical pixels.
    myFrameBuffer.HoriSizeInput = 640*4;  // 640 horizontal (32-bit pixels).
    myFrameBuffer.Stride = 640*4;         // Dont' worry about the rest of the values.
    myFrameBuffer.FrameDelay = 0;
    myFrameBuffer.EnableCircularBuf=1;
    myFrameBuffer.EnableSync = 0;
    myFrameBuffer.PointNum = 0;
    myFrameBuffer.EnableFrameCounter = 0;
    myFrameBuffer.FixedFrameStoreAddr = 0;
    if(XST_FAILURE == XAxiVdma_DmaConfig(&videoDMAController, XAXIVDMA_READ, &myFrameBuffer)) {
    	xil_printf("DMA Config Failed\r\n");
     }
    // We need to give the frame buffer pointers to the memory that it will use. This memory
    // is where you will write your video data. The vdma IP/driver then streams it to the HDMI
    // IP.
     myFrameBuffer.FrameStoreStartAddr[0] = FRAME_BUFFER_0_ADDR;
     myFrameBuffer.FrameStoreStartAddr[1] = FRAME_BUFFER_0_ADDR + 4*640*480;

     if(XST_FAILURE == XAxiVdma_DmaSetBufferAddr(&videoDMAController, XAXIVDMA_READ,
    		               myFrameBuffer.FrameStoreStartAddr)) {
    	 xil_printf("DMA Set Address Failed Failed\r\n");
     }
     // Print a sanity message if you get this far.
     xil_printf("Woohoo! I made it through initialization.\n\r");
     // Now, let's get ready to start displaying some stuff on the screen.
     // The variables framePointer and framePointer1 are just pointers to the base address
     // of frame 0 and frame 1.
    // framePointer0 = (unsigned int *) FRAME_BUFFER_0_ADDR;
     setFramePointer((unsigned int *) FRAME_BUFFER_0_ADDR);
     // Just paint some large red, green, blue, and white squares in different
     // positions of the image for each frame in the buffer (framePointer0 and framePointer1).

     // This tells the HDMI controller the resolution of your display (there must be a better way to do this).
     XIo_Out32(XPAR_AXI_HDMI_0_BASEADDR, 640*480);

     // Start the DMA for the read channel only.
     if(XST_FAILURE == XAxiVdma_DmaStart(&videoDMAController, XAXIVDMA_READ)){
    	 xil_printf("DMA START FAILED\r\n");
     }
     int frameIndex = 0;
     // We have two frames, let's park on frame 0. Use frameIndex to index them.
     // Note that you have to start the DMA process before parking on a frame.
     if (XST_FAILURE == XAxiVdma_StartParking(&videoDMAController, frameIndex,  XAXIVDMA_READ)) {
    	 xil_printf("vdma parking failed\n\r");
     }

     /****
      * Initialize GPIO, FIT, and INTC
      */

     Status = XGpio_Initialize(&gpPB, XPAR_PUSH_BUTTONS_5BITS_DEVICE_ID);
     // Set the push button peripheral to be inputs.
     XGpio_SetDataDirection(&gpPB, 1, 0x0000001F);


     microblaze_register_handler(interrupt_handler_dispatcher, NULL);
	 XIntc_EnableIntr(XPAR_INTC_0_BASEADDR,
			(XPAR_FIT_TIMER_0_INTERRUPT_MASK | XPAR_PUSH_BUTTONS_5BITS_IP2INTC_IRPT_MASK));
	 XIntc_MasterEnable(XPAR_INTC_0_BASEADDR);



     //initialize the scene and render the initial state of the scene.
	clearScreen();
	initScene();
	render();

	microblaze_enable_interrupts();

     /*********************
      * Test Code
      **********************/
//     u8 d10;
//	 u8 d1;

     //Code to test our render functions
     while (1) {

    	 if (gameOver == T) {
    		 game_over();
    		 while(1); //put a timer here with reset code afterwards
    	 }
    	 else {
			 if(shouldAnimateTank == T) {
				if(++tankAnimationCount == 10) {
					resetTank();
					tankAnimationCount = 0;
				}
				else {
					animateTank();
				}
					tankAnimationTimer = 0;
					shouldAnimateTank = F;//    resetTank();
			}
			 else {
				 if(moveAlienBlock == T){
					 moveAliens();
					 drawAliens();
					 moveAlienBlock = F;
					 alienTimer = 0;
				 }
				 if(moveTank == T){
					 if(buttonState & LEFT)
						 moveTankLeft();
					 else if (buttonState & RIGHT)
						 moveTankRight();

					 if(buttonState & FIRE_TANK_BULLET){
						 launchTankBullet();
					 }
					 tankTimer = 0;
					 moveTank = F;
				 }
				 if( takeSpaceshipAction == T){
					switch( spaceshipTimerExpired ){
					case SHOW_SPACESHIP:
						placeSpaceship();
						break;
					case ANIMATE_BONUS:
						animateBonus();
						break;
					case MOVE_SPACESHIP:
						moveSpaceship();
						break;
					default:
						break;
					}
					takeSpaceshipAction = F;
					spaceshipTimer = 0;
				 }
				 if(updateTankBullet == T){
					 moveTankBullet();
					 updateTankBullet = F;
					 tankBulletTimer = 0;
				 }
				 if (updateAlienBullets == T){
					 moveAlienBullets();
					 updateAlienBullets = F;
					 alienBulletTimer = 0;
				 }

				 if(fireAlienBullet == T){
					 launchAlienBullet();
					 launchAlienBulletTimer = 0;
					 fireAlienBullet = F;
				 }
				 if(shouldEraseAlienExplosion == T){
					 alienExplosionTimer = 0;
					 shouldEraseAlienExplosion = F;
					 eraseAlienExplosion();
				 }
			 }
    	 }


//    	 u8 test = XUartLite_RecvByte(XPAR_UARTLITE_1_BASEADDR) - 48 ;
//
//    	 switch(test){
//    	 case 0:
//    		 d10 = XUartLite_RecvByte(XPAR_UARTLITE_1_BASEADDR) - 48;
//    		 d1 = XUartLite_RecvByte(XPAR_UARTLITE_1_BASEADDR) - 48;
//    		 killAlien(10*d10 + d1);
//    		 break;
//    	 case 1:
//    	 	 moveSpaceship();
//			 break;
//    	 case 2:
//    		 placeSpaceship();
//			 break;
//    	 case 3:
//    		 launchAlienBullet();
//			 break;
//    	 case 4:
//    		 moveTankLeft();
//			 break;
//    	 case 5:
//    		 launchTankBullet();
//			 break;
//    	 case 6:
//    		 moveTankRight();
//			 break;
//    	 case 7:
//    		 animateTank();
//    		 break;
//    	 case 8:
//    		 moveAliens();
//			 drawAliens();
//			 break;
//    	 case 9:
//    		 moveAlienBullets();
//    		 moveTankBullet();
//    		 break;
//    	 default:
//    		 break;
//    	 }
    }
     cleanup_platform();

    return 0;
}
