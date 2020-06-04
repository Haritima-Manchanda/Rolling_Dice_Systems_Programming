/* NAME: HARITIMA MANCHANDA 
 * SECTION: CISC 210 (HONORS)
 * LAB 5 (PROJECT 1)
 */


/*
 * DESCRIPTION: This program simulates a set of four 6 sided dice. 
 * To roll the dice, the user will shake the raspberry pi.
 * Using the accelerometer, the program detects this shaking and select 4 random numbers between 1 and 6.
 * This program then draws the two dice on the screen as dots. 
 * the program also uses a joystick to change the color of dots and redraws the current screen in the new color.
 * The program exits cleanly on ctrl-c.
 */


#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sense/sense.h>
#include <linux/input.h>

/*SENSITIVITY detects a change in acceleration on shaking. 
Shaking must produce an acceleration of greater that 2, otherwise the accelerometer will not detect the acceleration.
*/
#define SENSITIVITY 2

pi_framebuffer_t* fb;

/* GLOBAL VARIABLES
 * runJoystick: Keeps an account whether the joystick was pressed or not.
 * 		If yes then changes its value to 1.
 *
 * run: Keeps account of whether the program is running.
 */
int runJoystick = 0; 
int run =1;

/* function to exit the program cleanly on pressing ctrl C */
void handler(int sig)
{
	printf("\nEXITING...\n");
	run = 0;
}

/* callbackFn: a function of the form void fn(int) that is called when an event occurs or when the joystick is pressed.
 * 		If the joystick is pressed, it changes the value of runjoystick to 1.
 */
void callbackFn(unsigned int code)
{
	switch(code)
	{
		case KEY_UP:
		case KEY_DOWN:
		case KEY_RIGHT:
		case KEY_LEFT:
			runJoystick = 1;
			break;
		default:
			run = 0;
			break;
	}
}

/* drawDice:  accepts the random number generated along with startingX and startingY and draw the number in the form of dots.
 * startingX and startingY determine the corner 3X3 box in which the dice is to be drawn. 
 * Used so that each random number is drawn in a separate 3X3 box.
 */
void drawDice(sense_fb_bitmap_t *screen, int num1,int startingX,int startingY,int color)
{
	switch(num1)
	{
		case 1:
			setPixel(screen,startingX + 1,startingY + 1,color);
			break;
		case 2:
			setPixel(screen,startingX,startingY,color);
			setPixel(screen,startingX+2,startingY+2,color);
			break;
		case 3:
			setPixel(screen,startingX,startingY,color);
			setPixel(screen,startingX+1,startingY+1,color);
			setPixel(screen,startingX+2,startingY+2,color);
			break;
		case 4:
			setPixel(screen,startingX,startingY,color);
			setPixel(screen,startingX+2,startingY+2,color);
			setPixel(screen,startingX,startingY+2,color);
			setPixel(screen,startingX+2,startingY,color);
			break;
		case 5:
			setPixel(screen,startingX,startingY,color);
			setPixel(screen,startingX+1,startingY+1,color);
			setPixel(screen,startingX+2,startingY+2,color);
			setPixel(screen,startingX,startingY+2,color);
			setPixel(screen,startingX+2,startingY,color);
			break;
		case 6:
			for(int i=(startingX); i<(startingX+3);i++)
			{
				for(int j=startingY+1;j<(startingY+3);j++)
				{
					setPixel(screen,i,j,color);
				}
			}
			break;
	}
}

/*generateRandomNumbers: draws a plus sign to separate the four 3X3 boxes.
 * return : int random number generated.
 */
int  generateRandomNumbers(sense_fb_bitmap_t *screen)
{	
	clearBitmap(screen,getColor(0,0,0));

	//code to draw a plus sign that separates the 3X3 boxes
	for(int i=0; i < 8; i++)
	{
		for(int j=3; j< 5; j++)
		{
			setPixel(screen,i,j,getColor(255,0,0));
		}
	}
	for(int i = 3; i < 5; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			setPixel(screen,i,j,getColor(255,0,0));
		}
	}
	return (rand() % 6 + 1); 

}

/* detectShake: accepts the accelData and detects a change in acceleration when the dice is shaken
 */
int detectShake( coordinate_t *accelData)
{
	if( accelData->y > SENSITIVITY || accelData->z > SENSITIVITY)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int main(int argc,char* argv[])
{

	printf("\n SET THE PI ON THE GROUND FOR CALIBERATION....\n");

	pi_i2c_t *device;
	coordinate_t data;
	signal(SIGINT,handler);
	fb = getFBDevice();
	pi_joystick_t* joystick = getJoystickDevice();

	if(!fb)
	{
		return 0;
	}

	clearBitmap(fb->bitmap,getColor(0,0,0));

	device = geti2cDevice();

	if(device){
			configureAccelGyro(device);
			printf("\n PICK THE DICE AND ROLL. PRESS CTRL C TO EXIT....\n");
			while(run)
			{
				usleep(2000);
				while(run && getAccelData(device, &data))
				{
					int y =detectShake(&data);
					int random1,random2,random3,random4;
					if(y == 1)
					{
						 random1 = generateRandomNumbers(fb->bitmap);
						 random2 = generateRandomNumbers(fb->bitmap);
						 random3 = generateRandomNumbers(fb->bitmap);
						 random4 = generateRandomNumbers(fb->bitmap);

						 drawDice(fb->bitmap,random1,0,0,getColor(0,0,255));
						 drawDice(fb->bitmap,random2,0,5,getColor(0,0,255));
						 drawDice(fb->bitmap,random3,5,0,getColor(0,0,255));
						 drawDice(fb->bitmap,random4,5,5,getColor(0,0,255));
					}

					pollJoystick(joystick,callbackFn,0);
					if(runJoystick == 1)
					{	
						//draws the dots in a different color: from blue to green
						drawDice(fb->bitmap,random1,0,0,getColor(0,255,0));
						drawDice(fb->bitmap,random2,0,5,getColor(0,255,0));
						drawDice(fb->bitmap,random3,5,0,getColor(0,255,0));
						drawDice(fb->bitmap,random4,5,5,getColor(0,255,0));
						runJoystick = 0;
					}
					
				}
			}
			freei2cDevice(device);
		}
	clearBitmap(fb->bitmap,0);
	freeFrameBuffer(fb);
	freeJoystick(joystick);
	return 0;
}
