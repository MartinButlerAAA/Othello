//************************************************************************************************************************
//* Othello			Martin Butler	November 2025
//*
//* Game to play Othello against the computer.
//*
//* computerMove
//*
//* Logic for the processing intelligence to work out the computer move.
//* This includes a dummy version of the human move that is mainly random to try out the computerMove.
//* It also includes self-play to play the computerMove against the dummy human to try out different weightings.
//* The weightings are updated each time they reduce the number of lost games, aiming to optimise the constant values used.
//*
//************************************************************************************************************************
#include <stdlib.h>			// For rand.

#include "Game.h"			// Game API.
#include "computerMove.h"	// Access to other Game functions to support computer move.

#ifdef PLAYSELF
#include <iostream>			// For std::cout. Only needed for optimisation.
#endif

// For the computer to analyse valid moves, a data type is needed.  
typedef struct validMove validMove_t;

//Valid move data type showing position and data to aid selection process.
struct validMove
{
	unsigned int x;	// x position on board 1-8 left to right.
	unsigned int y;	// y position on board 1-8 top to bottom.
	int score;		// Calculated score used to select valid moves.
};

// Integer constant weightings used for calculating moves. These are variable to support optimisation.
int EDG =   2;	// Score for an edge position.
int EG2 =   1;	// Score for edge next to a corner we have.
int CNR =  10;	// Score for a corner position.
int CNO = -11;	// Score for possible corner for opponent.
int CN2 =   7;	// Score for possible corner next move.
int NCN =  -7;	// Score for playing next to an available corner.
int JIN =   3;	// Score for a position in the quadrant near a corner the computer holds.
int BTW =   1;	// Score for playing between opponent pieces (harder to flip). 
int BTO =   2;	// Score for playing between own pieces to fill in the gaps.
int DIG =  13;	// Score for playing to take advantage of the opponent playing next to a corner.

float losses = 1000.0f;	// Count to check how many games lost in optimisation run.

// Calculate the computer move. Find all valid moves for the current play and assess each move to give it a score.
// The select the move with the highest score. Various aspects of the move are considered such as on an edge or corner.
void computerMove(void)
{
	validMove_t validMoves[60];	// Array to store valid moves (60 is the maximum number of available spaces on the board at the start of the game).
	unsigned int moveN = 0;		// Valid Move count for validMoves array.
	unsigned int selN  = 0;		// Selected valid move.
	int captN = 0;				// Used to record the highest score to select the best move.

	// Go through the entire board looking for valid moves 'V's and log the board positions in the validMoves array.
	// Note board positions are labelled 1-8 left to right and 1-8 top to bottom, 1,1 is top left.
	// Then process move to determine a score for how good that move is.
	for (unsigned int x = 1; x <= 8; x++)
	{
		for (unsigned int y = 1; y <= 8; y++)
		{
			// If a valid move is found capture the position.
			if (gameTable[x][y] == 'V')
			{
				validMoves[moveN].x = x;
				validMoves[moveN].y = y;
				validMoves[moveN].score = 0;	// Start the score at 0.

				// If the valid move is on an edge then increase the score as edges are preferable as they are harder to flip.
				if (x == 1) { validMoves[moveN].score = validMoves[moveN].score + EDG; }
				if (x == 8) { validMoves[moveN].score = validMoves[moveN].score + EDG; }
				if (y == 1) { validMoves[moveN].score = validMoves[moveN].score + EDG; }
				if (y == 8) { validMoves[moveN].score = validMoves[moveN].score + EDG; }

				// If the valid move is one of the four corners increase score as corners cannot be flipped.
				if ((x == 1) && (y == 1)) { validMoves[moveN].score = validMoves[moveN].score + CNR; }
				if ((x == 1) && (y == 8)) { validMoves[moveN].score = validMoves[moveN].score + CNR; }
				if ((x == 8) && (y == 1)) { validMoves[moveN].score = validMoves[moveN].score + CNR; }
				if ((x == 8) && (y == 8)) { validMoves[moveN].score = validMoves[moveN].score + CNR; }

				// If we have a corner, favour moves along edges next to the corner.
				if ((gameTable[1][1] == 'G') && ((x == 1) || (y == 1))) { validMoves[moveN].score = validMoves[moveN].score + EG2; }
				if ((gameTable[1][8] == 'G') && ((x == 1) || (y == 8))) { validMoves[moveN].score = validMoves[moveN].score + EG2; }
				if ((gameTable[8][1] == 'G') && ((x == 8) || (y == 1))) { validMoves[moveN].score = validMoves[moveN].score + EG2; }
				if ((gameTable[8][8] == 'G') && ((x == 8) || (y == 8))) { validMoves[moveN].score = validMoves[moveN].score + EG2; }

				// Avoid giving away a corner, by playing a position next to an available corner.
				if ((x == 2) && (y == 2) && (gameTable[1][1] == ' ')) { validMoves[moveN].score = validMoves[moveN].score + NCN; }
				if ((x == 1) && (y == 2) && (gameTable[1][1] == ' ')) { validMoves[moveN].score = validMoves[moveN].score + NCN; }
				if ((x == 2) && (y == 1) && (gameTable[1][1] == ' ')) { validMoves[moveN].score = validMoves[moveN].score + NCN; }
				if ((x == 7) && (y == 7) && (gameTable[8][8] == ' ')) { validMoves[moveN].score = validMoves[moveN].score + NCN; }
				if ((x == 7) && (y == 8) && (gameTable[8][8] == ' ')) { validMoves[moveN].score = validMoves[moveN].score + NCN; }
				if ((x == 8) && (y == 7) && (gameTable[8][8] == ' ')) { validMoves[moveN].score = validMoves[moveN].score + NCN; }
				if ((x == 7) && (y == 2) && (gameTable[8][1] == ' ')) { validMoves[moveN].score = validMoves[moveN].score + NCN; }
				if ((x == 7) && (y == 1) && (gameTable[8][1] == ' ')) { validMoves[moveN].score = validMoves[moveN].score + NCN; }
				if ((x == 8) && (y == 2) && (gameTable[8][1] == ' ')) { validMoves[moveN].score = validMoves[moveN].score + NCN; }
				if ((x == 2) && (y == 7) && (gameTable[1][8] == ' ')) { validMoves[moveN].score = validMoves[moveN].score + NCN; }
				if ((x == 1) && (y == 7) && (gameTable[1][8] == ' ')) { validMoves[moveN].score = validMoves[moveN].score + NCN; }
				if ((x == 2) && (y == 8) && (gameTable[1][8] == ' ')) { validMoves[moveN].score = validMoves[moveN].score + NCN; }

				// Try to gain a corner if opponent has played a space next to that corner, by putting a piece in line. X axis top and bottom.
				if ((gameTable[1][1] == ' ') && (gameTable[2][1] == 'R') && (x == 3) && (y == 1)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[1][1] == ' ') && (gameTable[2][1] == 'R') && (gameTable[3][1] == 'R') && (x == 4) && (y == 1)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[1][1] == ' ') && (gameTable[2][1] == 'R') && (gameTable[3][1] == 'R') && (gameTable[4][1] == 'R') && (x == 5) && (y == 1)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[1][1] == ' ') && (gameTable[2][1] == 'R') && (gameTable[3][1] == 'R') && (gameTable[4][1] == 'R') && (gameTable[5][1] == 'R') && (x == 6) && (y == 1)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[8][1] == ' ') && (gameTable[7][1] == 'R') && (gameTable[6][1] == 'R') && (gameTable[5][1] == 'R') && (gameTable[4][1] == 'R') && (x == 3) && (y == 1)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[8][1] == ' ') && (gameTable[7][1] == 'R') && (gameTable[6][1] == 'R') && (gameTable[5][1] == 'R') && (x == 4) && (y == 1)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[8][1] == ' ') && (gameTable[7][1] == 'R') && (gameTable[6][1] == 'R') && (x == 5) && (y == 1)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[8][1] == ' ') && (gameTable[7][1] == 'R') && (x == 6) && (y == 1)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[1][8] == ' ') && (gameTable[2][8] == 'R') && (x == 3) && (y == 8)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[1][8] == ' ') && (gameTable[2][8] == 'R') && (gameTable[3][8] == 'R') && (x == 4) && (y == 8)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[1][8] == ' ') && (gameTable[2][8] == 'R') && (gameTable[3][8] == 'R') && (gameTable[4][8] == 'R') && (x == 5) && (y == 8)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[1][8] == ' ') && (gameTable[2][8] == 'R') && (gameTable[3][8] == 'R') && (gameTable[4][8] == 'R') && (gameTable[5][8] == 'R') && (x == 6) && (y == 8)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[8][8] == ' ') && (gameTable[7][8] == 'R') && (gameTable[6][8] == 'R') && (gameTable[5][8] == 'R') && (gameTable[4][8] == 'R') && (x == 3) && (y == 8)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[8][8] == ' ') && (gameTable[7][8] == 'R') && (gameTable[6][8] == 'R') && (gameTable[5][8] == 'R') && (x == 4) && (y == 8)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[8][8] == ' ') && (gameTable[7][8] == 'R') && (gameTable[6][8] == 'R') && (x == 5) && (y == 8)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[8][8] == ' ') && (gameTable[7][8] == 'R') && (x == 6) && (y == 8)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }

				// Try to gain a corner if opponent has played a space next to that corner, by putting a piece in line. Y axis left and right.
				if ((gameTable[1][1] == ' ') && (gameTable[1][2] == 'R') && (x == 1) && (y == 3)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[1][1] == ' ') && (gameTable[1][2] == 'R') && (gameTable[1][3] == 'R') && (x == 1) && (y == 4)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[1][1] == ' ') && (gameTable[1][2] == 'R') && (gameTable[1][3] == 'R') && (gameTable[1][4] == 'R') && (x == 1) && (y == 5)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[1][1] == ' ') && (gameTable[1][2] == 'R') && (gameTable[1][3] == 'R') && (gameTable[1][4] == 'R') && (gameTable[1][5] == 'R') && (x == 1) && (y == 6)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[1][8] == ' ') && (gameTable[1][7] == 'R') && (gameTable[1][6] == 'R') && (gameTable[1][5] == 'R') && (gameTable[1][4] == 'R') && (x == 1) && (y == 3)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[1][8] == ' ') && (gameTable[1][7] == 'R') && (gameTable[1][6] == 'R') && (gameTable[1][5] == 'R') && (x == 1) && (y == 4)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[1][8] == ' ') && (gameTable[1][7] == 'R') && (gameTable[1][6] == 'R') && (x == 1) && (y == 5)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[1][8] == ' ') && (gameTable[1][7] == 'R') && (x == 1) && (y == 6)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[8][1] == ' ') && (gameTable[8][2] == 'R') && (x == 8) && (y == 3)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[8][1] == ' ') && (gameTable[8][2] == 'R') && (gameTable[8][3] == 'R') && (x == 8) && (y == 4)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[8][1] == ' ') && (gameTable[8][2] == 'R') && (gameTable[8][3] == 'R') && (gameTable[8][4] == 'R') && (x == 8) && (y == 5)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[8][1] == ' ') && (gameTable[8][2] == 'R') && (gameTable[8][3] == 'R') && (gameTable[8][4] == 'R') && (gameTable[8][5] == 'R') && (x == 8) && (y == 6)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[8][8] == ' ') && (gameTable[8][7] == 'R') && (gameTable[8][6] == 'R') && (gameTable[8][5] == 'R') && (gameTable[8][4] == 'R') && (x == 8) && (y == 3)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[8][8] == ' ') && (gameTable[8][7] == 'R') && (gameTable[8][6] == 'R') && (gameTable[8][5] == 'R') && (x == 8) && (y == 4)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[8][8] == ' ') && (gameTable[8][7] == 'R') && (gameTable[8][6] == 'R') && (x == 8) && (y == 5)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[8][8] == ' ') && (gameTable[8][7] == 'R') && (x == 8) && (y == 6)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }

				// Try to gain a corner if opponent has played a space next to that corner, by putting a piece in line. Y axis left and right. Diagonals.
				if ((gameTable[1][1] == ' ') && (gameTable[2][2] == 'R') && (x == 3) && (y == 3)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[1][1] == ' ') && (gameTable[2][2] == 'R') && (gameTable[3][3] == 'R') && (gameTable[4][4] == 'R') && (gameTable[5][5] == 'R') && (x == 6) && (y == 6)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[8][8] == ' ') && (gameTable[7][7] == 'R') && (x == 6) && (y == 6)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[8][8] == ' ') && (gameTable[7][7] == 'R') && (gameTable[6][6] == 'R') && (gameTable[5][5] == 'R') && (gameTable[4][4] == 'R') && (x == 3) && (y == 3)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[1][8] == ' ') && (gameTable[2][7] == 'R') && (x == 3) && (y == 6)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[1][8] == ' ') && (gameTable[2][7] == 'R') && (gameTable[3][6] == 'R') && (gameTable[4][5] == 'R') && (gameTable[5][3] == 'R') && (x == 6) && (y == 3)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[8][1] == ' ') && (gameTable[7][2] == 'R') && (x == 6) && (y == 3)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }
				if ((gameTable[8][1] == ' ') && (gameTable[7][2] == 'R') && (gameTable[6][3] == 'R') && (gameTable[5][4] == 'R') && (gameTable[3][5] == 'R') && (x == 3) && (y == 6)) { validMoves[moveN].score = validMoves[moveN].score + DIG; }

				// Favour positions in the quadrant of a corner you have already captured.
				if ((x <= 3) && (y <= 3) && (gameTable[1][1] == 'G')) { validMoves[moveN].score = validMoves[moveN].score + JIN; }
				if ((x >= 5) && (y >= 5) && (gameTable[8][8] == 'G')) { validMoves[moveN].score = validMoves[moveN].score + JIN; }
				if ((x <= 3) && (y <= 5) && (gameTable[1][8] == 'G')) { validMoves[moveN].score = validMoves[moveN].score + JIN; }
				if ((x >= 5) && (y <= 3) && (gameTable[8][1] == 'G')) { validMoves[moveN].score = validMoves[moveN].score + JIN; }

				// Favour positions that are already surrounded by the opponent.
				if ((gameTable[x - 1][y] == 'R') && (gameTable[x + 1][y] == 'R')) { validMoves[moveN].score = validMoves[moveN].score + BTW; }
				if ((gameTable[x][y - 1] == 'R') && (gameTable[x][y + 1] == 'R')) { validMoves[moveN].score = validMoves[moveN].score + BTW; }
				if ((gameTable[x - 1][y - 1] == 'R') && (gameTable[x + 1][y + 1] == 'R')) { validMoves[moveN].score = validMoves[moveN].score + BTW; }
				if ((gameTable[x + 1][y - 1] == 'R') && (gameTable[x - 1][y + 1] == 'R')) { validMoves[moveN].score = validMoves[moveN].score + BTW; }

				// Logic had been included to avoid playing next to an opponents edge pieces, but optimisation set to this to zero or negative on several occasions, so it has been removed.

				// Favour positions that are already surrounded by your own pieces.
				if ((gameTable[x - 1][y] == 'G') && (gameTable[x + 1][y] == 'G')) { validMoves[moveN].score = validMoves[moveN].score + BTO; }
				if ((gameTable[x][y - 1] == 'G') && (gameTable[x][y + 1] == 'G')) { validMoves[moveN].score = validMoves[moveN].score + BTO; }
				if ((gameTable[x - 1][y - 1] == 'G') && (gameTable[x + 1][y + 1] == 'G')) { validMoves[moveN].score = validMoves[moveN].score + BTO; }
				if ((gameTable[x + 1][y - 1] == 'G') && (gameTable[x - 1][y + 1] == 'G')) { validMoves[moveN].score = validMoves[moveN].score + BTO; }

				// Try out the move to see if it lets the opponent get a corner. If it does reduce the score for that move.
				// While doing this also check if it increases the chance for the computer to get a corner later on.
				tableToWorking();					// Copy the current game to the working table for processing.	
				clearValidWorking();				// Clear valid moves as these will get in the way of working out best move.
				workingTable[x][y] = 'G';			// Try the move in the working table.
				captureRedWork(x, y, workingTable);	// Capture the pieces on the working table.

				// The captured pieces were counted and used as part of the score. However, removing this improved the computer play.
				// As pieces can be flipped back, going for the most flips is rarely the best move.

				// The best way to capture a corner is to flip a diagonal piece as this allows capturing part of a diagonal including a corner.
				if ((gameTable[1][1] == ' ') && ((gameTable[3][3] == 'g') || (gameTable[4][4] == 'g') || (gameTable[5][5] == 'g') || (gameTable[6][6] == 'g'))) 
					{ validMoves[moveN].score = validMoves[moveN].score + CN2; }
				if ((gameTable[8][8] == ' ') && ((gameTable[3][3] == 'g') || (gameTable[4][4] == 'g') || (gameTable[5][5] == 'g') || (gameTable[6][6] == 'g')))
					{ validMoves[moveN].score = validMoves[moveN].score + CN2; }
				if ((gameTable[1][8] == ' ') && ((gameTable[6][3] == 'g') || (gameTable[5][4] == 'g') || (gameTable[4][5] == 'g') || (gameTable[3][6] == 'g')))
					{ validMoves[moveN].score = validMoves[moveN].score + CN2; }
				if ((gameTable[8][1] == ' ') && ((gameTable[6][3] == 'g') || (gameTable[5][4] == 'g') || (gameTable[4][5] == 'g') || (gameTable[3][6] == 'g')))
					{ validMoves[moveN].score = validMoves[moveN].score + CN2; }


				clearFlips();	// Clear flips now they have been counted, so that the don't impact opponent valid move processing.
				validRedMovesWork(workingTable);	// See what valid moves this gives to the opponent.
				// If the move means the opponent can get a corner, reduce the score.
				if (workingTable[1][1] == 'V') { validMoves[moveN].score = validMoves[moveN].score + CNO; }
				if (workingTable[1][8] == 'V') { validMoves[moveN].score = validMoves[moveN].score + CNO; }
				if (workingTable[8][1] == 'V') { validMoves[moveN].score = validMoves[moveN].score + CNO; }
				if (workingTable[8][8] == 'V') { validMoves[moveN].score = validMoves[moveN].score + CNO; }

				clearValidWorking();				// Clear the possible red moves to check the possible green moves.
				validGreenMovesWork(workingTable);	// See what valid moves this gives us.
				// If the move means that we have a chance to capture a corner increase the score.
				if (workingTable[1][1] == 'V') { validMoves[moveN].score = validMoves[moveN].score + CN2; }
				if (workingTable[1][8] == 'V') { validMoves[moveN].score = validMoves[moveN].score + CN2; }
				if (workingTable[8][1] == 'V') { validMoves[moveN].score = validMoves[moveN].score + CN2; }
				if (workingTable[8][8] == 'V') { validMoves[moveN].score = validMoves[moveN].score + CN2; }

				moveN++;	// Go on to the next valid move.
			}
		}
	}

	// Now that all valid moves have been analysed, select the valid move with the highest score.
	captN = -1;
	for (unsigned int a = 0; a < moveN; a++)
	{
		if (validMoves[a].score > captN)
		{
			captN = validMoves[a].score;
			selN = a;
		}
	}

	// Play the selected valid move.
	clearValid();	// Get rid of potential move markers now move has been chosen.
	gameTable[validMoves[selN].x][validMoves[selN].y] = 'G';	// Play the selected move.
	captureRed(validMoves[selN].x, validMoves[selN].y);			// Capture the pieces.
	return;
}

#ifdef PLAYSELF	// Only needed for Optimisation.
// Dummy human move to replace the player move to support optimisation.
// This function favours edges and corners, but otherwise plays randomly.
void putMoveDev(void)
{
	validMove_t validMoves[60];	// Array to store valid moves (60 is the maximum number of available spaces on the board at the start of the game).
	unsigned int moveN = 0;		// Valid Move count.
	unsigned int selN = 0;		// Selected valid move.
	int captN = 0;				// Used to record the highest score to select the best move.

	// Go through the entire board looking for valid moves 'V's and log the board positions in the validMoves array.
	// Note board positions are labelled 1-8 left to right and 1-8 top to bottom, 1,1 is top left.
	for (unsigned int x = 1; x <= 8; x++)
	{
		for (unsigned int y = 1; y <= 8; y++)
		{
			// If a valid move is found capture the position.
			if (gameTable[x][y] == 'V')
			{
				validMoves[moveN].x = x;
				validMoves[moveN].y = y;
				validMoves[moveN].score = rand() % 8;		// Add a random element so that the dummy player plays differently for each game.

				// If the valid move is on an edge then increase the score. 
				if (x == 1) { validMoves[moveN].score = validMoves[moveN].score + 2; }
				if (x == 8) { validMoves[moveN].score = validMoves[moveN].score + 2; }
				if (y == 1) { validMoves[moveN].score = validMoves[moveN].score + 2; }
				if (y == 8) { validMoves[moveN].score = validMoves[moveN].score + 2; }

				// If the valid move is one of the four corners increase score.
				if ((x == 1) && (y == 1)) { validMoves[moveN].score = validMoves[moveN].score + 10; }
				if ((x == 1) && (y == 8)) { validMoves[moveN].score = validMoves[moveN].score + 10; }
				if ((x == 8) && (y == 1)) { validMoves[moveN].score = validMoves[moveN].score + 10; }
				if ((x == 8) && (y == 8)) { validMoves[moveN].score = validMoves[moveN].score + 10; }

				moveN++;	// Next valid move.
			}
		}
	}

	// Select the move based on the score.
	for (unsigned int a = 0; a < moveN; a++)
	{
		if (validMoves[a].score > captN)
		{
			captN = validMoves[a].score;
			selN = a;
		}
	}

	// Play the selected valid move.
	clearValid();	// Get rid of potential move markers now move has been chosen.
	gameTable[validMoves[selN].x][validMoves[selN].y] = 'R';	// Add the move.
	captureGreen(validMoves[selN].x, validMoves[selN].y);		// Capture the pieces.
	return;
}

// This is a version of the game play loop from the main program, that plays the computer player against the dummy human player many times.
// The score weightings are randomly tweaked each time and played against the dummy human player. If the number of games lost reduces, the new weightings are retained.
// In this way the computer move calculations are optimised to lose the fewest games (ideally none).
void Optimise(void)
{
	unsigned int red = 2, green = 2;	// Counts for how many pieces each player has.
	float rWin = 0.0f, gWin = 0.0f;		// Counts for each player game wins. Floating point is used so that draws can be awarded as 0.5 each.

	// Local copies of the calculation constants to keep the best values found by optimisation.
	int EDGb, EG2b, CNRb, CNOb, CN2b, NCNb, JINb, BTWb, BTOb, DIGb;

	clearGameTable();	// Set up the game table.

	// Set best to match starting values before optimisation.
	EDGb = EDG;
	EG2b = EG2;
	CNRb = CNR;
	CNOb = CNO;
	CN2b = CN2;
	NCNb = NCN;
	JINb = JIN;
	BTWb = BTW;
	BTOb = BTO;
	DIGb = DIG;

	// Check the board to get the pieces counts before the first display.
	checkBoard('R', &red, &green);

	for (int a = 0; a < 5000; a++)	// Try many different variations of the weightings.
	{
		for (int b = 0; b < 500; b++)	// Try 1000 games alternating starting player.
		{
			for (;;)	// Play with player first until game over.
			{
				validRedMoves();	// Identify all of the valid moves that the human player can make.

				// If there are valid moves available to the player get their move.
				if (checkBoard('R', &red, &green) != 'M')
				{
					putMoveDev();	// Used for computer to play self to build database.
				}

				checkBoard('G', &red, &green);	// Check the board to get the pieces counts before display.

				// Check if the game is over.
				if (checkBoard('B', &red, &green) != ' ') { break; }

				validGreenMoves();	// Identify the valid moves for the computer.

				// If there are valid moves available to the computer get their move.
				if (checkBoard('G', &red, &green) != 'M') { computerMove(); }

				checkBoard('R', &red, &green); // Check the board to get the pieces counts before display.

				// Check if the game is over.
				if (checkBoard('B', &red, &green) != ' ') { break; }
			}

			// Work out who won and update the score count.
			if (red > green) { rWin = rWin + 1.0f; }
			else if (red < green) { gWin = gWin + 1.0f; }
			else { rWin = rWin + 0.5f; gWin = gWin + 0.5f; }	// Need to consider the players can draw.

			clearGameTable();	// Set up the game table.

			// Check the board to get the pieces counts before the first display.
			checkBoard('G', &red, &green);

			for (;;)	// Play with computer first until game over.
			{
				validGreenMoves();	// Identify all of the valid moves for the computer.

				// If there are valid moves available to the computer get their move.
				if (checkBoard('G', &red, &green) != 'M') { computerMove(); }

				checkBoard('R', &red, &green);	// Check the board to get the pieces counts before display.

				// Check if the game is over.
				if (checkBoard('B', &red, &green) != ' ') { break; }

				validRedMoves();	// Identify the valid moves for the player.

				// If there are valid moves available to the player get their move.
				if (checkBoard('G', &red, &green) != 'M')
				{
					putMoveDev();	// Used for computer to play self to build database.
				}

				// Check if the game is over.
				checkBoard('G', &red, &green);	// Check the b7 4oard to get the pieces counts before display.

				// Check if the game is over.
				if (checkBoard('B', &red, &green) != ' ') { break; }
			}

			// Work out who won and update the score count.
			if (red > green) { rWin = rWin + 1.0f; }
			else if (red < green) { gWin = gWin + 1.0f; }
			else { rWin = rWin + 0.5f; gWin = gWin + 0.5f; }	// Need to consider the players can draw.

			clearGameTable();	// Set up the game table.

			// Check the board to get the pieces counts before the first display.
			checkBoard('R', &red, &green);
		}

		std::cout << "Red Wins: " << rWin << " Green Wins: " << gWin << "\n";	// Display number of wins.

		// If the opponent won fewer games, these weightings are an improvemnt, so keep them and display them.
		if (rWin < losses)
		{
			// Set best to match current values;
			EDGb = EDG;
			EG2b = EG2;
			CNRb = CNR;
			CNOb = CNO;
			CN2b = CN2;
			NCNb = NCN;
			JINb = JIN;
			BTWb = BTW;
			BTOb = BTO;
			DIGb = DIG;
			std::cout << "EDG: " << EDG << " EG2: " << EG2 << " CNR: " << CNR << " CNO: " << CNO << " CN2: " << CN2 << " NCN: " << NCN << " JIN: " << JIN << " BTW: " << BTW << " BTO: " << BTO << " DIG: " << DIG << "\n";	// Display weightings.

			// Set the new expectation for losses, ready to test the next set of weightings.
			losses = rWin;
		}

		// Set the current settings back to the best values (for the case where the trial weightings were worse than the best).
		EDG = EDGb;
		EG2 = EG2b;
		CNR = CNRb;
		CNO = CNOb;
		CN2 = CN2b;
		NCN = NCNb;
		JIN = JINb;
		BTW = BTWb;
		BTO = BTOb;
		DIG = DIGb;

		// Randomly tweak some of the values to trial these against the dummy human player.
		if ((rand() % 5) == 0) { EDG = EDGb + (rand() % 7) - 3; }
		if ((rand() % 5) == 0) { EG2 = EG2b + (rand() % 7) - 3; }
		if ((rand() % 5) == 0) { CNR = CNRb + (rand() % 7) - 3; }
		if ((rand() % 5) == 0) { CNO = CNOb + (rand() % 7) - 3; }
		if ((rand() % 5) == 0) { CN2 = CN2b + (rand() % 7) - 3; }
		if ((rand() % 5) == 0) { NCN = NCNb + (rand() % 7) - 3; }
		if ((rand() % 5) == 0) { JIN = JINb + (rand() % 7) - 3; }
		if ((rand() % 5) == 0) { BTW = BTWb + (rand() % 7) - 3; }
		if ((rand() % 5) == 0) { BTO = BTOb + (rand() % 7) - 3; }
		if ((rand() % 5) == 0) { DIG = DIGb + (rand() % 7) - 3; }

		// Clean the win counts ready for next optimisation run.
		rWin = 0.0f;
		gWin = 0.0f;
	}
	// Show the final weightings after optimisation.
	std::cout << "EDG: " << EDGb << " EG2: " << EG2b << " CNR: " << CNRb << " CNO: " << CNOb << " CN2: " << CN2b << " NCN: " << NCNb << " JIN: " << JINb << " BTW: " << BTWb << " BTO: " << BTOb << " DIG: " << DIGb << "\n";	// Display weightings.
}
#endif

