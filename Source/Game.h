//************************************************************************************************************************
//* Othello			Martin Butler	November 2025
//*
//* Game to play Othello against the computer.
//*
//* Game_Private header.
//*
//* Application Program Interface (API) for the Othello game processing mainly Game.c but also computerMove.
//*
//************************************************************************************************************************
#pragma once

#include <stdbool.h>				// To use booleans.

//#define PLAYSELF					// Defined constant used to adjust build for optimisation, rather than for human play.

void clearGameTable(void);			// Clears the game table ready for a new game.

bool putMove(int xi, int yi);		// Put the player's move (identified by column x and row y numbers) into the game Table, returns true if move valid.

char getGameTable(int x, int y);	// Get a character from the game table for display. Identified by column and row.
									// The positions are numbered x 1-8 and y 1-8.
									// 1,1 is top left, 8,1 is top right, 1,8 is bottom left and 8,8 is bottom right.

void validRedMoves(void);			// Identify valid moves for Red (player) on the game table.

void computerMove(void);			// Call to calculate the computer move which is added to the game table, in the same manner as for the player.

void validGreenMoves(void);			// Identify valid moves for Green (computer) on the game table.

char checkBoard(char pl, unsigned int* red, unsigned int* green);	// Check the state of the board 'B' is for both players, 'R' for red and 'G' for green.
									// For both players the function returns whether the game has ended 'E' or ' ' for not ended.
									// If 'R' or 'G' is selected it returns 'M' for miss a turn if there are no valid moves for that player, or ' ' if there are valid moves.
									// In all cases the red and green piece counts are updated.

