#include "address_map_arm.h"

#include<stdlib.h>
#include<time.h>

//Note: Each pixel is a half word.
//First 9 bits is X, next 9 is y offset. Display is 320x240 (0b000000011 0b000000001) (1,3)
//5 bits red, 6 bits green, 5 bits blue.

volatile short * display = (short *)FPGA_PIXEL_BUF_BASE; //Short so we can access each half word (1 pixel)
volatile int * timer = (int *)TIMER_BASE;
volatile int * buttons = (int *)KEY_BASE;
volatile int * led = (int *)LED_BASE;

short colour = 0b1111111111111111; //Keeps track of the colour we're using to draw things (default white). 
const int winwidth = 319, winheight = 239; //screen width and height

enum Direction{up,down,left,right}dir = right; //Indicate the direction of the snake

int x = 0, y = 0;
const int boarddim=15;

//hoists
void drawRect(int x1, int y1, int x2, int y2);
void initTimer(int interval);
	 
int main(void) {
	
	int i, j; //Used for counting.
	int board[boarddim][boarddim]; //the playable area for the snake.
	
	//Initialize the board so it's empty.
	for(i = 0; i < boarddim; i++)
		for(j = 0; j < boarddim; j++)
			board[i][j] = 0;
	//Place the snake near the top
	int s_x = 1, s_y = 1; //the snake position.

	int blockdim = winheight/boarddim; //this is how big each board tile will be.

	int offsetX = winwidth/2-(boarddim*blockdim)/2;
	int lifespan = 5; //How many blocks are on screen at once
	int death = 0, spawnApple = 1;
	//Game loop
	initTimer(400);
	while(1)
	{
		//Process key input.
		if(((*buttons) & 1) == 1) dir = down; 	 //btn 0
		if(((*buttons) & 2) == 2) dir = up;	 //btn 1
		if(((*buttons) & 4) == 4) dir = right; 	 //btn 2
		if(((*buttons) & 8) == 8) dir = left; 	 //btn 3

		if(*timer == 3)
		{
			colour = 0; //black
			drawRect(0,0,winwidth,winheight); //clear screen.
			
			//Clear the board to whatever colour
			colour = 0b0001101100101101;
			for(i = 0; i < boarddim; i++)
				for(j = 0; j < boarddim; j++)
					drawRect(j*blockdim+offsetX, i*blockdim, blockdim, blockdim);

			//spawn an apple if needed
			if(spawnApple)
			{
				board[rand()%(boarddim-1)][rand()%(boarddim-1)] = -1;
				spawnApple = 0;
			}

			//handle direction based on keybaord input.
			if(dir == right) s_x++;
			if(dir == left) s_x--;
			if(dir == up) s_y--;
			if(dir == down) s_y++;

			if( (s_x >= boarddim) | (s_x < 0) | (s_y >= boarddim) | (s_y < 0) )
			{
				death = 1;
				s_x = 0; s_y = 0; //so not to break it when checking the indices
			}

			//Collision with part of snake!
			if(board[s_x][s_y] > 0 | death) //game over!
			{
				//Reset it all!
				for(i = 0; i < boarddim; i++)
					for(j = 0; j < boarddim; j++)
						board[i][j] = 0;
				s_x = 1;
				s_y = 1;
				death = 0;
				lifespan = 5;
				spawnApple = 1;
			}

			if(board[s_x][s_y] == -1) //apple!
			{
				lifespan++;
				spawnApple = 1;
			}

			board[s_x][s_y] = lifespan; //set it as a snake with max lifespan

			//Draw the board.
			colour = 0xffff; //white
			for(i = 0; i < boarddim; i++)
				for(j = 0; j < boarddim; j++)
					if(board[i][j] != 0)
					{
						if(board[i][j] == -1)
							colour = 0b11111000000000000; //red for apple!
						else board[i][j]--; //remove one from the lifespan if its not an apple.
						drawRect(j*blockdim+offsetX, i*blockdim, blockdim, blockdim); //draw the snake
						colour = 0xffff; //white
					}

			(*timer) = 2; //reset TO bit, keep the RUN bit
		}
	}

	return 0;
}

//Initialize the timer to begin (interval is in msec)
void initTimer(int interval) 
{
	interval *= 100000; //turn it into msec
	//Period (Tick every 1 sec, 10^8)
	*(timer+3) = interval >> 16;
	*(timer+2) = interval & 0xffff;
	*(timer+1) = 6; //start the timer!
}

//Draws a rectangle at the given screen coordinates.
void drawRect(int x0, int y0, int width, int height)
{
	//some error cases.
	if(width < 0 || height < 0 || x0+width > winwidth || y0+height > winheight || x0 < 0 || y0 < 0) return;
	int x, y;
	for(y = y0; y <= (y0+height); y++)
		for(x = x0; x <= (x0+width); x++) 
			*(display + ((y<<9) | x )) = colour;
}