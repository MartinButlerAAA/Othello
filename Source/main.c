//************************************************************************************************************************
//* Othello			Martin Butler	November 2025
//*
//* Game to play Othello against the computer.
//*
//* Main
//*
//* This is the main program running on Wii U to sequence game, images and sound.
//*
//************************************************************************************************************************
#include <stdio.h>				// For sprintf.

#include <coreinit/screen.h>	// for OSScreen.
#include <coreinit/thread.h>	// for Sleep.
#include <coreinit/time.h>		// To get time in usec.
#include <vpad/input.h>			// For the game pad inputs.
#include <whb/proc.h>			// For the loop and to do home button correctly.
#include <whb/log.h>			// Using the console logging features seems to help set up the screen output.
#include <whb/log_console.h>	// Found neeeded to keep these in the build for the program to display properly.

#include "Draw.h"				// For drawing via OSScreen.
#include "Images/Images.h"		// For the images to be drawn using Draw.h.
#include "Sounds.h"				// For sounds and background music.
#include "Game.h"				// For Othello Game API.
#include "computerMove.h"		// To gain access to the difficulty constant to change difficulty level.

enum gameState_e { SETUP, PLAYERMOVE, PANIMATE, WIIUMOVE, WANIMATE, NEWGAME };	// State machine to control game play.
enum messageState_e { NOMESSAGE, SKIPTURN, REDWIN, GREENWIN, DRAWGAME };		// Messages to be displayed to player.

typedef struct vMove vMove_t;	// Structure to support selecting the player move.
struct vMove					// Valid move data type showing position.
{
	unsigned int x;	// x position on board 1-8 left to right.
	unsigned int y;	// y position on board 1-8 top to bottom.
};
vMove_t vMoves[60];	// Array of possible valid moves. This is set to the available spaces at the start of the game, so it will always be enough.

// Globals to control game operation.
enum gameState_e gameState;			// State to control operation of play.
enum messageState_e messageState;	// Message to be displayed to player.
int stateCnt = 0;					// Counter to time state machine for player to see animation.
int nmoves = 0;						// Number of valid moves at this turn.
int selMove = 0;					// Move selected by the player.
unsigned int red = 0, green = 0;	// Counts for how many pieces each player has.
float rWin = 0.0f, gWin = 0.0f;		// Counts for each player game wins. Floating point is used so that draws can be awarded as 0.5 each.

void drawBorder()
{
	// Put a border round the screen to make a neat edge.
	drawLine(XOFFSET, YOFFSET - 1, XOFFSET + XDISPMAX, YOFFSET - 1, 0x01010100);
	drawLine(XOFFSET, YOFFSET - 2, XOFFSET + XDISPMAX, YOFFSET - 2, 0x01010100);

	drawLine(XOFFSET, YOFFSET + YDISPMAX, XOFFSET + XDISPMAX, YOFFSET + YDISPMAX, 0x01010100);
	drawLine(XOFFSET, YOFFSET + YDISPMAX + 1, XOFFSET + XDISPMAX, YOFFSET + YDISPMAX + 1, 0x01010100);

	drawLine(XOFFSET - 1, YOFFSET, XOFFSET - 1, YOFFSET + YDISPMAX, 0x01010100);
	drawLine(XOFFSET - 2, YOFFSET, XOFFSET - 2, YOFFSET + YDISPMAX, 0x01010100);

	drawLine(XOFFSET + XDISPMAX, YOFFSET, XOFFSET + XDISPMAX, YOFFSET + YDISPMAX, 0x01010100);
	drawLine(XOFFSET + XDISPMAX + 1, YOFFSET, XOFFSET + XDISPMAX + 1, YOFFSET + YDISPMAX, 0x01010100);
}

// Display all elements from the game on the TV screen.
void displayTV()
{
	// Strings to assembe details about games won and number of pieces in this game.
	char sRed[100] = "\0";
	char sGreen[100] = "\0";
	char sRedWin[100] = "\0";
	char sGreenWin[100] = "\0";

	// Assemble display strings as API does not work like printf.
	sprintf(sRed,      "Red   %2i ", red);
	sprintf(sGreen,    "Green %2i ", green);
	sprintf(sRedWin,   "Red Wins   % 2.1f ", rWin);
	sprintf(sGreenWin, "Green Wins % 2.1f ", gWin);

	// Clear the Gamepad to have a grey background.
	OSScreenClearBufferEx(SCREEN_TV, 0x80808000u);

	drawBorder();

	// Put the scores and piece counts on the screen either side of the game board.
	drawText(sRedWin, 0x88001500, 2, 20, 100, SCREEN_TV);
	drawText(sGreenWin, 0xA1FB8E00, 2, 20, 130, SCREEN_TV);
	drawText(sRed, 0x88001500, 2, (XDISPMAX + XOFFSET + 30), 100, SCREEN_TV);
	drawText(sGreen, 0xA1FB8E00, 2, (XDISPMAX + XOFFSET + 30), 130, SCREEN_TV);

	// Display any messages needed.
	if (messageState == SKIPTURN) { drawText("MISS TURN\0", 0xFEFEFE00, 3, 20, 300, SCREEN_TV); }
	if (messageState == REDWIN)   { drawText("RED WIN\0", 0x88001500, 3, 20, 300, SCREEN_TV); }
	if (messageState == GREENWIN) { drawText("GREEN WIN\0", 0xA1FB8E00, 3, 20, 300, SCREEN_TV); }
	if (messageState == DRAWGAME) { drawText("DRAW\0", 0xFEFEFE00, 3, 20, 300, SCREEN_TV); }

	// Show the Othello game board.
	for (int x = 1; x <= 8; x++)
	{
		for (int y = 1; y <= 8; y++)
		{
			// Put the correct sprite on the screen for the state of that space in the game board.
			if (getGameTable(x, y) == ' ') { drawImage(BLKSIZE, BLKSIZE, BlankImage, ((x - 1) * BLKSIZE), ((y -1) * BLKSIZE)); }
			if (getGameTable(x, y) == 'V') { drawImage(BLKSIZE, BLKSIZE, AllowedImage, ((x - 1) * BLKSIZE), ((y - 1) * BLKSIZE)); }
			if (getGameTable(x, y) == 'R') { drawImage(BLKSIZE, BLKSIZE, Red5Image, ((x - 1) * BLKSIZE), ((y - 1) * BLKSIZE)); }
			if (getGameTable(x, y) == 'G') { drawImage(BLKSIZE, BLKSIZE, Green1Image, ((x - 1) * BLKSIZE), ((y - 1) * BLKSIZE)); }

			// This is a fiddle using global variable to animate pieces flipping. 
			// It isn't all that maintainable, but it works and shouldn't need to change.
			if  (getGameTable(x, y) == 'r') { drawImage(BLKSIZE, BLKSIZE, Red5Image, ((x - 1) * BLKSIZE), ((y - 1) * BLKSIZE)); }	// If not animating show as a red piece.
			if ((getGameTable(x, y) == 'r') && (gameState == PANIMATE)) { drawImage(BLKSIZE, BLKSIZE, Green1Image, ((x - 1) * BLKSIZE), ((y - 1) * BLKSIZE)); }
			if ((getGameTable(x, y) == 'r') && (stateCnt ==  3) && (gameState == PANIMATE)) { drawImage(BLKSIZE, BLKSIZE, Green2Image, ((x - 1) * BLKSIZE), ((y - 1) * BLKSIZE)); }
			if ((getGameTable(x, y) == 'r') && (stateCnt ==  4) && (gameState == PANIMATE)) { drawImage(BLKSIZE, BLKSIZE, Green3Image, ((x - 1) * BLKSIZE), ((y - 1) * BLKSIZE)); }
			if ((getGameTable(x, y) == 'r') && (stateCnt ==  5) && (gameState == PANIMATE)) { drawImage(BLKSIZE, BLKSIZE, Green4Image, ((x - 1) * BLKSIZE), ((y - 1) * BLKSIZE)); }
			if ((getGameTable(x, y) == 'r') && (stateCnt ==  6) && (gameState == PANIMATE)) { drawImage(BLKSIZE, BLKSIZE, Green5Image, ((x - 1) * BLKSIZE), ((y - 1) * BLKSIZE)); }
			if ((getGameTable(x, y) == 'r') && (stateCnt ==  7) && (gameState == PANIMATE)) { drawImage(BLKSIZE, BLKSIZE, MiddleImage, ((x - 1) * BLKSIZE), ((y - 1) * BLKSIZE)); }
			if ((getGameTable(x, y) == 'r') && (stateCnt ==  8) && (gameState == PANIMATE)) { drawImage(BLKSIZE, BLKSIZE, Red1Image, ((x - 1) * BLKSIZE), ((y - 1) * BLKSIZE)); }
			if ((getGameTable(x, y) == 'r') && (stateCnt ==  9) && (gameState == PANIMATE)) { drawImage(BLKSIZE, BLKSIZE, Red2Image, ((x - 1) * BLKSIZE), ((y - 1) * BLKSIZE)); }
			if ((getGameTable(x, y) == 'r') && (stateCnt == 10) && (gameState == PANIMATE)) { drawImage(BLKSIZE, BLKSIZE, Red3Image, ((x - 1) * BLKSIZE), ((y - 1) * BLKSIZE)); }
			if ((getGameTable(x, y) == 'r') && (stateCnt == 11) && (gameState == PANIMATE)) { drawImage(BLKSIZE, BLKSIZE, Red4Image, ((x - 1) * BLKSIZE), ((y - 1) * BLKSIZE)); }
			if ((getGameTable(x, y) == 'r') && (stateCnt >= 12) && (gameState == PANIMATE)) { drawImage(BLKSIZE, BLKSIZE, Red5Image, ((x - 1) * BLKSIZE), ((y - 1) * BLKSIZE)); }

			if  (getGameTable(x, y) == 'g') { drawImage(BLKSIZE, BLKSIZE, Green1Image, ((x - 1) * BLKSIZE), ((y - 1) * BLKSIZE)); }	// If not animating show as a green piece.
			if ((getGameTable(x, y) == 'g') && (gameState == WIIUMOVE)) { drawImage(BLKSIZE, BLKSIZE, Red5Image, ((x - 1) * BLKSIZE), ((y - 1) * BLKSIZE)); }
			if ((getGameTable(x, y) == 'g') && (stateCnt == 22) && (gameState == WIIUMOVE)) { drawImage(BLKSIZE, BLKSIZE, Red4Image, ((x - 1) * BLKSIZE), ((y - 1) * BLKSIZE)); }
			if ((getGameTable(x, y) == 'g') && (stateCnt == 23) && (gameState == WIIUMOVE)) { drawImage(BLKSIZE, BLKSIZE, Red3Image, ((x - 1) * BLKSIZE), ((y - 1) * BLKSIZE)); }
			if ((getGameTable(x, y) == 'g') && (stateCnt == 24) && (gameState == WIIUMOVE)) { drawImage(BLKSIZE, BLKSIZE, Red2Image, ((x - 1) * BLKSIZE), ((y - 1) * BLKSIZE)); }
			if ((getGameTable(x, y) == 'g') && (stateCnt == 25) && (gameState == WIIUMOVE)) { drawImage(BLKSIZE, BLKSIZE, Red1Image, ((x - 1) * BLKSIZE), ((y - 1) * BLKSIZE)); }
			if ((getGameTable(x, y) == 'g') && (stateCnt == 26) && (gameState == WIIUMOVE)) { drawImage(BLKSIZE, BLKSIZE, MiddleImage, ((x - 1) * BLKSIZE), ((y - 1) * BLKSIZE)); }
			if ((getGameTable(x, y) == 'g') && (stateCnt == 27) && (gameState == WIIUMOVE)) { drawImage(BLKSIZE, BLKSIZE, Green5Image, ((x - 1) * BLKSIZE), ((y - 1) * BLKSIZE)); }
			if ((getGameTable(x, y) == 'g') && (stateCnt == 28) && (gameState == WIIUMOVE)) { drawImage(BLKSIZE, BLKSIZE, Green4Image, ((x - 1) * BLKSIZE), ((y - 1) * BLKSIZE)); }
			if ((getGameTable(x, y) == 'g') && (stateCnt == 29) && (gameState == WIIUMOVE)) { drawImage(BLKSIZE, BLKSIZE, Green3Image, ((x - 1) * BLKSIZE), ((y - 1) * BLKSIZE)); }
			if ((getGameTable(x, y) == 'g') && (stateCnt == 30) && (gameState == WIIUMOVE)) { drawImage(BLKSIZE, BLKSIZE, Green2Image, ((x - 1) * BLKSIZE), ((y - 1) * BLKSIZE)); }
			if ((getGameTable(x, y) == 'g') && (stateCnt >= 31) && (gameState == WIIUMOVE)) { drawImage(BLKSIZE, BLKSIZE, Green1Image, ((x - 1) * BLKSIZE), ((y - 1) * BLKSIZE)); }
		}
	}
	// Superimpose the selected move onto the table (it is set off the screen when it is not to be displayed).
	drawImage(BLKSIZE, BLKSIZE, SelectedImage, ((vMoves[selMove].x - 1) * BLKSIZE), ((vMoves[selMove].y - 1) * BLKSIZE));

	// Flip the screen buffer to show the new display.
	OSScreenFlipBuffersEx(SCREEN_TV);
	return;
}

// Display information on the Gamepad screen.
void displayGPad()
{
	// Clear the Gamepad to have a grey background.
	OSScreenClearBufferEx(SCREEN_DRC, 0x80808000u);

	// Instructions
	drawText("Othello\0", 0xFEFEFE00, 4, 10, 10, SCREEN_DRC);
	drawText("You play dark red, the computer plays light green.\0", 0xFEFEFE00, 2, 10, 100, SCREEN_DRC);
	drawText("Play for as many red pieces as you can.\0", 0xFEFEFE00, 2, 10, 130, SCREEN_DRC);
	drawText("Use the Joycon or direction buttons to select.\0", 0xFEFEFE00, 2, 10, 160, SCREEN_DRC);
	drawText("Press A to make move.\0", 0xFEFEFE00, 2, 10, 190, SCREEN_DRC);
	if (difficulty == EASY)   { drawText("EASY    Press ZL and ZR to change difficulty.\0", 0xFEFEFE00, 2, 10, 230, SCREEN_DRC); }
	if (difficulty == MEDIUM) { drawText("MEDIUM  Press ZL and ZR to change difficulty.\0", 0xFEFEFE00, 2, 10, 230, SCREEN_DRC); }
	if (difficulty == HARD)   { drawText("HARD    Press ZL and ZR to change difficulty.\0", 0xFEFEFE00, 2, 10, 230, SCREEN_DRC); }

	// Flip the screen buffer to show the new display.
	OSScreenFlipBuffersEx(SCREEN_DRC);
	return;
}

// Find an array of the valid moves to support player using controls to select their move.
void findValidMoves(void)
{
	nmoves = 0;	// Number of valid moves found.
	for (int x = 1; x <= 8; x++)
	{
		for (int y = 1; y <= 8; y++)
		{
			if (getGameTable(x, y) == 'V') { vMoves[nmoves].x = x; vMoves[nmoves].y = y; nmoves++; }
		}
	}
	selMove = nmoves - 1;	// Set the first valid move to the last valid move found.
}

// Allow the player to use the Joycon or direction buttons to select between the valid moves.
bool humanMove(void)
{
	VPADStatus status;		// Status returned for the gamepad button.
	VPADReadError error;	// Error from gamepad.

	VPADRead(VPAD_CHAN_0, &status, 1, &error);	// Get the VPAD button last pressed (including Joycon).
	if (error == VPAD_READ_SUCCESS)				// Only process buttons if no errors (e.g. gamepad lost power).
	{
		// Move backwards through the valid moves.
		if ((status.trigger & VPAD_BUTTON_UP) || (status.trigger & VPAD_STICK_L_EMULATION_UP) || (status.trigger & VPAD_BUTTON_LEFT) || (status.trigger & VPAD_STICK_L_EMULATION_LEFT))
		{
			selMove--;
			if (selMove < 0) { selMove = nmoves - 1; }	// Wrap round back to the top of the list.
		}
		// Move forwards through the valid moves.
		if ((status.trigger & VPAD_BUTTON_DOWN) || (status.trigger & VPAD_STICK_L_EMULATION_DOWN) || (status.trigger & VPAD_BUTTON_RIGHT) || (status.trigger & VPAD_STICK_L_EMULATION_RIGHT))
		{
			selMove++;
			if (selMove >= nmoves) { selMove = 0; }		// Wrap round back to the bottom of the list.
		}
		// Play the selected move.
		if (status.trigger & VPAD_BUTTON_A) 
		{
			putMove(vMoves[selMove].x, vMoves[selMove].y);
			return true;
		}
		// If commanded change the level of difficulty,
		if ((status.trigger & VPAD_BUTTON_ZL) && (status.trigger & VPAD_BUTTON_ZL))
		{
			difficulty++;
			if (difficulty > HARD) { difficulty = EASY;  } // Wrap difficulty back to easy.
			gameState = SETUP;	// As difficulty changed, start a new game.
		}
	}
	return false;
}

int main(int argc, char **argv) 
{
	int tim;					// Variable for processing time of a game cycle.
	OSTime tm1, tm2;			// Times in usec used to time a game cycle.
	int del = 5;				// Delay used to sequence the game.

	WHBProcInit();				// This is the main process and must be in the program at the start for the home button to operate correctly.
    WHBLogConsoleInit();		// ConsoleInit seems to get the display to operate correctly so keep in the build.

	setupSound();

	gameState = SETUP;			// Initial game state.
	messageState = NOMESSAGE;	// No message to display at start of game.

	// There must be a main loop on WHBProc running, for the program to correctly operate with the home button.
	// Home pauses this loop and continues it if resume is selected. There must therefore be one main loop of processing in the main program.
    while (WHBProcIsRunning()) {
		tm1 = OSTicksToMicroseconds(OSGetTick());	// Time before a game cycle processing.

		if (gameState == SETUP)
		{
			clearGameTable();				// Set up the game table.
			checkBoard('R', &red, &green);	// Check the board to get the pieces counts before the first display.
			validRedMoves();				// Identify all of the valid moves that the human player can make.
			findValidMoves();				// Get list of valid moves to support human move.
			gameState = PLAYERMOVE;			// Move on to the player move.
			stateCnt = 0;					// Remember to re-start state count for each change of state.
		}
		else if (gameState == PLAYERMOVE)
		{
			// Check if there is a valid move available.
			if (checkBoard('R', &red, &green) != 'M')
			{
				// If there are valid moves let the player select one.
				if (humanMove() == true) 
				{ 
					selMove = 59;		// Setting the selected move to the end of the table means x and y are 0, so it is no longer displayed.
					putsoundSel(MOVE);	// Make the move sound.
					gameState = PANIMATE;
					stateCnt = 0;		// Remember to re-start state count for each change of state.
				}
			}
			// Otherwise go to the computer move.
			else
			{
				selMove = 59;				// Setting the selected move to the end of the table means x and y are 0, so it is no longer displayed.
				putsoundSel(DRAW);			// Make the move sound.
				gameState = PANIMATE;		// Move to on to animating the player move.
				stateCnt = 0;				// Remember to re-start state count for each change of state.
				messageState = SKIPTURN;	// Show player had to miss a turn.
			}
		}
		else if (gameState == PANIMATE)
		{
			// Allow time for animation to be seen, before doing checking which clears flips.
			if (stateCnt == 20)
			{
				// Check if the game is over.
				if (checkBoard('B', &red, &green) != ' ') 
				{ 
					gameState = NEWGAME; 
					stateCnt = 0;			// Remember to re-start state count for each change of state.
				}
				messageState = NOMESSAGE;	// Remember to clear any message displayed.
			}
			// Move on after time allowed to see move.
			if (stateCnt >= 40) 
			{ 
				validGreenMoves();			// Identify the valid moves for the computer.
				gameState = WIIUMOVE;
				stateCnt = 0;				// Remember to re-start state count for each change of state.
			}
		}
		else if (gameState == WIIUMOVE)
		{
			// Check the move once allowing time for it to be seen.
			if (stateCnt == 20)
			{
				// Check if there is a valid move available and make it
				if (checkBoard('G', &red, &green) != 'M')
				{
					computerMove();
					putsoundSel(MOVE);	// Make the move sound.
				}
				else  // Otherwise show the computer had to miss a go.
				{
					putsoundSel(DRAW);	// Make s skip turn sound.
					messageState = SKIPTURN;
				}
			}
			// Move on after allowing time for the player to see what is going on.
			if (stateCnt > 40)
			{
				gameState = WANIMATE;
				stateCnt = 0;					// Remember to re-start state count for each change of state.
				messageState = NOMESSAGE;
			}
		}
		else if (gameState == WANIMATE)
		{
			// Do this once only.
			if (stateCnt == 1)
			{
				// Check if the game is over.
				if (checkBoard('B', &red, &green) != ' ')
				{
					gameState = NEWGAME;
					stateCnt = 0;		// Remember to re-start state count for each change of state.
				}
			}
			// Move on after allowing time for changes to be seen.
			if (stateCnt > 20)
			{
				validRedMoves();	// Identify all of the valid moves that the human player can make.
				findValidMoves();	// Get list of valid moves to support human move.
				gameState = PLAYERMOVE;
				stateCnt = 0;		// Remember to re-start state count for each change of state.
			}
		}
		else if (gameState == NEWGAME)
		{
			// Only want to calculate the winner once.
			if (stateCnt == 1)
			{
				// Work out who won. 
				if (red > green) { rWin = rWin + 1.0f; putsoundSel(WIN); messageState = REDWIN; }
				else if (red < green) { gWin = gWin + 1.0f; putsoundSel(LOSE); messageState = GREENWIN; }
				else { rWin = rWin + 0.5f; gWin = gWin + 0.5f;  putsoundSel(DRAW);  messageState = DRAWGAME; }	// Need to consider the players can draw.
			}
			// Move on after allowing time for sound to play.
			if (stateCnt > 50)
			{
				clearGameTable();				// Set up the game table.
				checkBoard('B', &red, &green);	// Check the board to get the pieces counts before the first display.
				messageState = NOMESSAGE;		// Clear any messages.

				// Even games have human first, odd games have computer first.
				if ((((int)(rWin + gWin + 0.05f)) % 2) == 0)
				{
					validRedMoves();				// Identify all of the valid moves that the human player can make.
					findValidMoves();				// Get list of valid moves to support human move.
					gameState = PLAYERMOVE;			// Human player first.
					stateCnt = 0;					// Remember to re-start state count for each change of state.
				}
				else
				{
					validGreenMoves();				// Identify the valid moves for the computer.
					gameState = WIIUMOVE;			// Computer first.
					stateCnt = 0;					// Remember to re-start state count for each change of state.
				}
			}
		}

		stateCnt++;									// Keep incrementing state count for timing and animation.

		displayTV();								// Update the TV display.
		displayGPad();								// Update the Gamepad display.

		tm2 = OSTicksToMicroseconds(OSGetTick());	// Time after game cycle processing.

		// The screen update rate is 60Hz, 50ms is 3 screen updates, so the game is running at an update rate of 20Hz.
		tim = ((tm2 - tm1) / 1000);					// Calulate the processing time in msec.
		del = 50 - tim;								// Adjust the game delay for the amount of time used in processing.
		if (del <= 0) { del = 3; }					// Limit delay to sensible values to avoid program getting stuck.
		if (del > 50) { del = 50; }
		OSSleepTicks(OSMillisecondsToTicks(del));	// Delay to keep game operating at the same screen update.
	}

	QuitSound();

	// If we get out of the program clean up and exit.
    WHBLogConsoleFree();
    WHBProcShutdown();
    return 0;
}

