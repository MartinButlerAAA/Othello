//************************************************************************************************************************
//* Othello			Martin Butler	November 2025
//*
//* Game to play Othello against the computer.
//*
//* computerMove header.
//*
//* Header to make functions from the game processing available to computerMove for move calculation and optimisation.
//*
//************************************************************************************************************************
#pragma once

// Global data and functions from Game.c that are needed for computer move, but are not made available for the main game program.
extern char gameTable[10][10];									// Main game table.
extern char	workingTable[10][10];								// Working copy of game table.

extern void tableToWorking(void);								// Copy the current game board to a working copy for more detailed processing.
extern void clearFlips(void);									// Clear any lower case used to indicate flipped pieces.
extern void clearValid(void);									// Get rid of potential move markers now move has been chosen.
extern void clearValidWorking(void);							// Clear valid moves as these will get in the way of working out best move.
extern void captureRed(int xi, int yi);							// Capture the red pieces to be flipped.
extern void captureGreen(int xi, int yi);						// Capture the green pieces to be flipped.
extern void captureRedWork(int xi, int yi, char table[10][10]);	// Capture the pieces on the working table on the chosen table.
extern void validGreenMovesWork(char table[10][10]);			// Work out which moves green can do, on the chosen table.
extern void validRedMovesWork(char table[10][10]);				// Work out which moves red can do, on the chosen table.
