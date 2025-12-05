//************************************************************************************************************************
//* Othello			Martin Butler	November 2025
//*
//* Game to play Othello against the computer.
//*
//* Game
//*
//* This processing of moves and checking, along with logic to determine player counts and game end.
//*
//************************************************************************************************************************
#include "Game.h"	// Game API.

// The game board is 8x8. The game table has a border all the way round of 'B' for boundary.
// This allows indices of 1 to 8 to be used as these are easier to understand and match the 8 by 8 board. 
// Having edges allows logic to search all round each board position without exceeding the array size.
char     gameTable[10][10];

// Used as a working copy of the game board, so that processing does not corrupt the actual game table.
char	workingTable[10][10];

// This is a small function to clear all places in the game table ready for a new game.
void clearGameTable(void)
{
	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			gameTable[x][y] = ' ';
		}
	}
	// Mark a border round the outside 
	for (int x = 0; x < 10; x++)
	{
		gameTable[x][0] = 'B';
		gameTable[x][9] = 'B';
		gameTable[0][x] = 'B';
		gameTable[9][x] = 'B';
	}

	// The starting four pieces are placed in the middle of the board.
	gameTable[4][4] = 'R';
	gameTable[4][5] = 'G';
	gameTable[5][5] = 'R';
	gameTable[5][4] = 'G';

	return;
}

// Copy the current gameTable to the working table, so that game processing, does not corrupt the game board.
void tableToWorking(void)
{
	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			workingTable[x][y] = gameTable[x][y];
		}
	}
}

// Get the specified contents from the game table to support board display.
char getGameTable(int x, int y)
{
	// If the pass parameters are in range of the gameTable, return the corresponding character for the game content.
	if ((x >= 1) && (x <= 8) && (y >= 1) && (y <= 8))
	{
		return (gameTable[x][y]);
	}
	return (' ');	// Otherwise return blank.
}

// Flipped counters are shown in lowercase to identify the pieces that have been captured by a move.
// Once the captured pieces have been displayed these need to be replaced with uppercase to simplify the rest of game processing.
void clearFlips(void)
{
	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			// Replace lower case red or green pieces with upper case, in both the game table and working copy.
			if (gameTable[x][y] == 'g') gameTable[x][y] = 'G';
			if (gameTable[x][y] == 'r') gameTable[x][y] = 'R';
			if (workingTable[x][y] == 'g') workingTable[x][y] = 'G';
			if (workingTable[x][y] == 'r') workingTable[x][y] = 'R';
		}
	}
}

// Clear out valid move markers (once the move has been selected and played).
void clearValid(void)
{
	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			if (gameTable[x][y] == 'V') gameTable[x][y] = ' ';
		}
	}
}

// Clear out valid move markers from the working copy.
void clearValidWorking(void)
{
	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			if (workingTable[x][y] == 'V') workingTable[x][y] = ' ';
		}
	}
}

// This function identifies all of the green pieces that have been flipped by a red move, marking them as lower case r.
// This function relies on clearValid being used to get rid of any V markers before it is run.
// The processing is similar to searching for valid moves. The function searches in all directions from the position selected.
void captureGreen(int xi, int yi)
{
	int xe, ye;		// Local processing indices.

	// Search right from the new piece to see if there is another Red 'R' piece.
	xe = 0;
	for (int x = xi + 1; x < 9; x++)
	{ 
		if (gameTable[x][yi] == ' ') { break; }		// If there is a gap stop searching.
		// If another Red piece is found capture it's position.
		if (gameTable[x][yi] == 'R') { xe = x; break; }
	}
	// If the search found another Red piece then go through the pieces in between and mark them to be flipped.
	if (xe != 0)
	{
		for (int x = xi + 1; x < xe; x++)
		{
			if (gameTable[x][yi] == 'G') { gameTable[x][yi] = 'r'; }
		}
	}
	// Search left from the moved piece to see if there is another Red 'R' piece.
	xe = 0;
	for (int x = xi - 1; x > 0; x--)
	{
		if (gameTable[x][yi] == ' ') { break; }		// If there is a gap stop searching.
		// If another Red piece is found capture it's position.
		if (gameTable[x][yi] == 'R') { xe = x; break; }
	}
	// If the search found another Red piece then go through the pieces in between and mark them to be flipped.
	if (xe != 0)
	{
		for (int x = xi - 1; x > xe; x--)
		{
			if (gameTable[x][yi] == 'G') { gameTable[x][yi] = 'r'; }
		}
	}
	// Search down from the moved piece to see if there is another Red 'R' piece.
	ye = 0;
	for (int y = yi + 1; y < 9; y++)
	{
		if (gameTable[xi][y] == ' ') { break; }		// If there is a gap stop searching.
		// If another Red piece is found capture it's position.
		if (gameTable[xi][y] == 'R') { ye = y; break; }
	}
	// If the search found another Red piece then go through the pieces in between and mark them to be flipped.
	if (ye != 0)
	{
		for (int y = yi + 1; y < ye; y++)
		{
			if (gameTable[xi][y] == 'G') { gameTable[xi][y] = 'r'; }
		}
	}
	// Search up from the moved piece to see if there is another Red 'R' piece.
	ye = 0;
	for (int y = yi - 1; y > 0; y--)
	{
		if (gameTable[xi][y] == ' ') { break; }		// If there is a gap stop searching.
		// If another Red piece is found capture it's position.
		if (gameTable[xi][y] == 'R') { ye = y; break; }
	}
	// If the search found another Red piece then go through the pieces in between and mark them to be flipped.
	if (ye != 0)
	{
		for (int y = yi - 1; y > ye; y--)
		{
			if (gameTable[xi][y] == 'G') { gameTable[xi][y] = 'r'; }
		}
	}
	// Search diagonal down/right from the moved piece to see if there is another Red 'R' piece.
	xe = 0;
	ye = 0;
	for (int a = 1; a < 8; a++)
	{
		// If x or y has got to the end of the board stop searching.
		if (((xi + a) > 8) || ((yi + a) > 8)) { break; }
		if (gameTable[xi + a][yi + a] == ' ') { break; }		// If there is a gap stop searching.
		// If another Red piece is found capture it's position.
		if (gameTable[xi + a][yi + a] == 'R') { xe = xi + a; ye = yi + a; break; }
	}
	// If the search found another Red piece then go through the pieces in between and mark them to be flipped.
	// Note xe and ye are set at the same time so only xe is checked.
	if (xe != 0) 
	{
		// Count along to change captured pieces.
		for (int a = 1; a < 8; a++)
		{
			// If x or y has got to the end of the line stop capturing.
			if (((xi + a) >= xe) || ((yi + a) >= ye)) { break; }
			// change the opponent pieces.
			if (gameTable[xi + a][yi + a] == 'G') { gameTable[xi + a][yi + a] = 'r'; }
		}
	}
	// Search diagonal up/left from the moved piece to see if there is another Red 'R' piece.
	xe = 0;
	ye = 0;
	for (int a = 1; a < 8; a++)
	{
		// If x or y has got to the end of the board stop searching.
		if (((xi - a) < 1) || ((yi - a) < 1)) { break; }
		if (gameTable[xi - a][yi - a] == ' ') { break; }		// If there is a gap stop searching.
		// If another Red piece is found capture it's position.
		if (gameTable[xi - a][yi - a] == 'R') { xe = xi - a; ye = yi - a; break; }
	}
	// If the search found another Red piece then go through the pieces in between and mark them to be flipped.
	// Note xe and ye are set at the same time so only xe is checked.
	if (xe != 0)
	{
		// Count along to change captured pieces.
		for (int a = 1; a < 8; a++)
		{
			// If x or y has got to the end of the line stop capturing.
			if (((xi - a) <= xe) || ((yi - a) <= ye)) { break; }
			// change the opponent pieces.
			if (gameTable[xi - a][yi - a] == 'G') { gameTable[xi - a][yi - a] = 'r'; }
		}
	}
	// Search diagonal down/left from the moved piece to see if there is another Red 'R' piece.
	xe = 0;
	ye = 0;
	for (int a = 1; a < 8; a++)
	{
		// If x or y has got to the end of the board stop searching.
		if (((xi - a) < 1) || ((yi + a) > 8)) { break; }
		if (gameTable[xi - a][yi + a] == ' ') { break; }		// If there is a gap stop searching.
		// If another Red piece is found capture it's position.
		if (gameTable[xi - a][yi + a] == 'R') { xe = xi - a; ye = yi + a; break; }
	}
	// If the search found another Red piece then go through the pieces in between and mark them to be flipped.
	// Note xe and ye are set at the same time so only xe is checked.
	if (xe != 0)
	{
		// Count along to change captured pieces.
		for (int a = 1; a < 8; a++)
		{
			// If x or y has got to the end of the line stop capturing.
			if (((xi - a) <= xe) || ((yi + a) >= ye)) { break; }
			// change the opponent pieces.
			if (gameTable[xi - a][yi + a] == 'G') { gameTable[xi - a][yi + a] = 'r'; }
		}
	}
	// Search diagonal up/right from the moved piece to see if there is another Red 'R' piece.
	xe = 0;
	ye = 0;
	for (int a = 1; a < 8; a++)
	{
		// If x or y has got to the end of the board stop searching.
		if (((xi + a) > 8) || ((yi - a) < 1)) { break; }
		if (gameTable[xi + a][yi - a] == ' ') { break; }		// If there is a gap stop searching.
		// If another Red piece is found capture it's position.
		if (gameTable[xi + a][yi - a] == 'R') { xe = xi + a; ye = yi - a; break; }
	}
	// If the search found another Red piece then go through the pieces in between and mark them to be flipped.
	// Note xe and ye are set at the same time so only xe is checked.
	if (xe != 0)
	{
		// Count along to change captured pieces.
		for (int a = 1; a < 8; a++)
		{
			// If x or y has got to the end of the line stop capturing.
			if (((xi + a) >= xe) || ((yi - a) <= ye)) { break; }
			// change the opponent pieces.
			if (gameTable[xi + a][yi - a] == 'G') { gameTable[xi + a][yi - a] = 'r'; }
		}
	}
	return;
}

// This function checks the move entered and puts it into gameTable if it is a valid move.
bool putMove(int xi, int yi)
{
	// Check the move entered is in range.
	if ((xi >= 1) && (xi <= 8) && (yi >= 1) && (yi <= 8))
	{
		// If the move selected is valid make that move.
		if (gameTable[xi][yi] == 'V')
		{
			// Make the move.
			gameTable[xi][yi] = 'R';
			clearValid();	// Get rid of potential move markers

			// Call function to capture all of the pieces to be flipped by the valid red move.
			captureGreen(xi, yi);
			return true;	// Valid move.
		}
	}
	return false;	// Not a valid move.
}

// Identify valid red moves in the table passed in to the function.
// The table is passed in so that the function can be used working copies of the game table.
void validRedMovesWork(char table[10][10])
{
	clearFlips();	// Tidy up flipped pieces.
	// Go through each of the 64 positions looking for the possible start of a valid move (adjacent to a green piece).
	for (int x = 1; x <= 8; x++)
	{
		for (int y = 1; y <= 8; y++)
		{
			// If the place is empty it is a possible valid move.
			if (table[x][y] == ' ')
			{
				// Check each of the 8 valid directions to see if the position is next to an opponent piece.

				// If left is next to an opponent piece search for a player piece in that direction.
				if (table[x - 1][y] == 'G')
				{
					// If a player piece is found on that line it is a valid move.
					for (int a = x - 2; a > 0; a--)
					{
						if ((table[a][y] == ' ') || (table[a][y] == 'V')) { break; } // If there is a gap it doesn't count.
						if (table[a][y] == 'R') { table[x][y] = 'V'; break; }
					}
				}
				// If right is next to an opponent piece search for a player piece in that direction.
				if (table[x + 1][y] == 'G')
				{
					for (int a = x + 2; a < 10; a++)
					{
						if ((table[a][y] == ' ') || (table[a][y] == 'V')) { break; } // If there is a gap it doesn't count.
						if (table[a][y] == 'R') { table[x][y] = 'V'; break; }
					}
				}
				// If up is next to an opponent piece search for a player piece in that direction.
				if (table[x][y - 1] == 'G')
				{
					for (int a = y - 2; a > 0; a--)
					{
						if ((table[x][a] == ' ') || (table[x][a] == 'V')) { break; } // If there is a gap it doesn't count.
						if (table[x][a] == 'R') { table[x][y] = 'V'; break; }
					}
				}
				// If down is next to an opponent piece search for a player piece in that direction.
				if (table[x][y + 1] == 'G')
				{
					for (int a = y + 2; a < 10; a++)
					{
						if ((table[x][a] == ' ') || (table[x][a] == 'V')) { break; } // If there is a gap it doesn't count.
						if (table[x][a] == 'R') { table[x][y] = 'V'; break; }
					}
				}
				// If diagonal left and up is next to an opponent piece search for a player piece in that direction.
				if (table[x - 1][y - 1] == 'G')
				{
					int x1 = x - 1;
					int y1 = y - 1;
					// If a player piece is found on that line it is a valid move.
					for (int a = 1; a < 8; a++)
					{
						x1 = x1 - 1;
						y1 = y1 - 1;
						// If the search has run off the end of the board stop searching.
						if ((x1 <= 0) || (y1 <= 0)) { break; }
						else if ((table[x1][y1] == ' ') || (table[x1][y1] == 'V')) { break; } // If there is a gap it doesn't count.
						else if (table[x1][y1] == 'R') { table[x][y] = 'V'; break; }
					}
				}
				// If diagonal down and right is next to an opponent piece search for a player piece in that direction.
				if (table[x + 1][y + 1] == 'G')
				{
					int x1 = x + 1;
					int y1 = y + 1;
					// If a player piece is found on that line it is a valid move.
					for (int a = 1; a < 8; a++)
					{
						x1 = x1 + 1;
						y1 = y1 + 1;
						// If the search has run off the end of the board stop searching.
						if ((x1 >= 9) || (y1 >= 9)) { break; }
						else if ((table[x1][y1] == ' ') || (table[x1][y1] == 'V')) { break; } // If there is a gap it doesn't count.
						else if (table[x1][y1] == 'R') { table[x][y] = 'V'; break; }
					}
				}
				// If diagonal down and left is next to an opponent piece search for a player piece in that direction.
				if (table[x - 1][y + 1] == 'G')
				{
					int x1 = x - 1;
					int y1 = y + 1;
					// If a player piece is found on that line it is a valid move.
					for (int a = 1; a < 8; a++)
					{
						x1 = x1 - 1;
						y1 = y1 + 1;
						// If the search has run off the end of the board stop searching.
						if ((x1 <= 0) || (y1 >= 9)) { break; }
						else if ((table[x1][y1] == ' ') || (table[x1][y1] == 'V')) { break; } // If there is a gap it doesn't count.
						else if (table[x1][y1] == 'R') { table[x][y] = 'V'; break; }
					}
				}
				// If diagonal up and right is next to an opponent piece search for a player piece in that direction.
				if (table[x + 1][y - 1] == 'G')
				{
					int x1 = x + 1;
					int y1 = y - 1;
					// If a player piece is found on that line it is a valid move.
					for (int a = 1; a < 8; a++)
					{
						x1 = x1 + 1;
						y1 = y1 - 1;
						// If the search has run off the end of the board stop searching.
						if ((x1 >= 9) || (y1 <= 0)) { break; }
						else if ((table[x1][y1] == ' ') || (table[x1][y1] == 'V')) { break; } // If there is a gap it doesn't count.
						else if (table[x1][y1] == 'R') { table[x][y] = 'V'; break; }
					}
				}
			}
		}
	}
	return;
}

// Identify valid moves for Red.
void validRedMoves(void)
{
	// Call the function to indentify the valid moves on the main game table so that these can be displayed.
	validRedMovesWork(gameTable);
}

// Identify valid red moves in the table passed in to the function.
// The table is passed in so that the function can be used working copies of the game table.
// This is identical to validRedMovesWork, I felt it was better to have separate functions for red and green, rather than one function with more complex logic.
void validGreenMovesWork(char table[10][10])
{
	clearFlips();	// Tidy up flipped pieces.
	// Go through each of the 64 positions looking for the possible start of a valid move (adjacent to a red piece).
	for (int x = 1; x <= 8; x++)
	{
		for (int y = 1; y <= 8; y++)
		{
			// If the place is empty it is a possible valid move.
			if (table[x][y] == ' ')
			{
				// Check each of the 8 valid directions to see if the position is next to an opponent piece.
				// If left is next to an opponent piece search for a player piece in that direction.
				if (table[x - 1][y] == 'R')
				{
					// If a player piece is found on that line it is a valid move.
					for (int a = x - 2; a > 0; a--)
					{
						if ((table[a][y] == ' ') || (table[a][y] == 'V')) { break; } // If there is a gap it doesn't count.
						if (table[a][y] == 'G') { table[x][y] = 'V'; break; }
					}
				}
				// If right is next to an opponent piece search for a player piece in that direction.
				if (table[x + 1][y] == 'R')
				{
					for (int a = x + 2; a < 10; a++)
					{
						if ((table[a][y] == ' ') || (table[a][y] == 'V')) { break; } // If there is a gap it doesn't count.
						if (table[a][y] == 'G') { table[x][y] = 'V'; break; }
					}
				}
				// If up is next to an opponent piece search for a player piece in that direction.
				if (table[x][y - 1] == 'R')
				{
					for (int a = y - 2; a > 0; a--)
					{
						if ((table[x][a] == ' ') || (table[x][a] == 'V')) { break; } // If there is a gap it doesn't count.
						if (table[x][a] == 'G') { table[x][y] = 'V'; break; }
					}
				}
				// If down is next to an opponent piece search for a player piece in that direction.
				if (table[x][y + 1] == 'R')
				{
					for (int a = y + 2; a < 10; a++)
					{
						if ((table[x][a] == ' ') || (table[x][a] == 'V')) { break; } // If there is a gap it doesn't count.
						if (table[x][a] == 'G') { table[x][y] = 'V'; break; }
					}
				}
				// If diagonal left and up is next to an opponent piece search for a player piece in that direction.
				if (table[x - 1][y - 1] == 'R')
				{
					int x1 = x - 1;
					int y1 = y - 1;
					// If a player piece is found on that line it is a valid move.
					for (int a = 1; a < 8; a++)
					{
						x1 = x1 - 1;
						y1 = y1 - 1;
						// If the search has run off the end of the board stop searching.
						if ((x1 <= 0) || (y1 <= 0)) { break; }
						else if ((table[x1][y1] == ' ') || (table[x1][y1] == 'V')) { break; } // If there is a gap it doesn't count.
						else if (table[x1][y1] == 'G') { table[x][y] = 'V'; break; }
					}
				}
				// If diagonal down and right is next to an opponent piece search for a player piece in that direction.
				if (table[x + 1][y + 1] == 'R')
				{
					int x1 = x + 1;
					int y1 = y + 1;
					// If a player piece is found on that line it is a valid move.
					for (int a = 1; a < 8; a++)
					{
						x1 = x1 + 1;
						y1 = y1 + 1;
						// If the search has run off the end of the board stop searching.
						if ((x1 >= 9) || (y1 >= 9)) { break; }
						else if ((table[x1][y1] == ' ') || (table[x1][y1] == 'V')) { break; } // If there is a gap it doesn't count.
						else if (table[x1][y1] == 'G') { table[x][y] = 'V'; break; }
					}
				}
				// If diagonal down and left is next to an opponent piece search for a player piece in that direction.
				if (table[x - 1][y + 1] == 'R')
				{
					int x1 = x - 1;
					int y1 = y + 1;
					// If a player piece is found on that line it is a valid move.
					for (int a = 1; a < 8; a++)
					{
						x1 = x1 - 1;
						y1 = y1 + 1;
						// If the search has run off the end of the board stop searching.
						if ((x1 <= 0) || (y1 >= 9)) { break; }
						else if ((table[x1][y1] == ' ') || (table[x1][y1] == 'V')) { break; } // If there is a gap it doesn't count.
						else if (table[x1][y1] == 'G') { table[x][y] = 'V'; break; }
					}
				}
				// If diagonal up and right is next to an opponent piece search for a player piece in that direction.
				if (table[x + 1][y - 1] == 'R')
				{
					int x1 = x + 1;
					int y1 = y - 1;
					// If a player piece is found on that line it is a valid move.
					for (int a = 1; a < 8; a++)
					{
						x1 = x1 + 1;
						y1 = y1 - 1;
						// If the search has run off the end of the board stop searching.
						if ((x1 >= 9) || (y1 <= 0)) { break; }
						else if ((table[x1][y1] == ' ') || (table[x1][y1] == 'V')) { break; } // If there is a gap it doesn't count.
						else if (table[x1][y1] == 'G') { table[x][y] = 'V'; break; }
					}
				}
			}
		}
	}
	return;
}

// Identify valid moves for Green.
void validGreenMoves(void)
{
	// Call the function to indentify the valid moves on the main game table so that these can be displayed.
	validGreenMovesWork(gameTable);
}

// This function identifies all of the red pieces that have been flipped by green move, marking them as lower case g.
// This function relies on clearValid being used to get rid of any V markers before it is run.
// This is identical to captureGreenWork. It was deemed more maintainable to have two functions than one more complex function for both players.
// Th table is passed in so that it can be used a working copy for compuer move processing.
void captureRedWork(int xi, int yi, char table[10][10])
{
	int xe, ye;

	// Search right from the moved piece to see if there is another Green piece.
	xe = 0;
	for (int x = xi + 1; x < 9; x++)
	{
		if (table[x][yi] == ' ') { break; }		// If there is a gap stop searching.
		// If another Green piece is found capture it's position.
		if (table[x][yi] == 'G') { xe = x; break; }
	}
	// If the search found another Green piece then go through the pieces in between and mark them to be flipped.
	if (xe != 0)
	{
		for (int x = xi + 1; x < xe; x++)
		{
			if (table[x][yi] == 'R') { table[x][yi] = 'g'; }
		}
	}
	// Search left from the moved piece to see if there is another Green piece.
	xe = 0;
	for (int x = xi - 1; x > 0; x--)
	{
		if (table[x][yi] == ' ') { break; }		// If there is a gap stop searching.
		// If another Green piece is found capture it's position.
		if (table[x][yi] == 'G') { xe = x; break; }
	}
	// If the search found another Green piece then go through the pieces in between and mark them to be flipped.
	if (xe != 0)
	{
		for (int x = xi - 1; x > xe; x--)
		{
			if (table[x][yi] == 'R') { table[x][yi] = 'g'; }
		}
	}
	// Search down from the moved piece to see if there is another Green piece.
	ye = 0;
	for (int y = yi + 1; y < 9; y++)
	{
		if (table[xi][y] == ' ') { break; }		// If there is a gap stop searching.
		// If another Green piece is found capture it's position.
		if (table[xi][y] == 'G') { ye = y; break; }
	}
	// If the search found another Green piece then go through the pieces in between and mark them to be flipped.
	if (ye != 0)
	{
		for (int y = yi + 1; y < ye; y++)
		{
			if (table[xi][y] == 'R') { table[xi][y] = 'g'; }
		}
	}
	// Search up from the moved piece to see if there is another Green piece.
	ye = 0;
	for (int y = yi - 1; y > 0; y--)
	{
		if (table[xi][y] == ' ') { break; }		// If there is a gap stop searching.
		// If another Green piece is found capture it's position.
		if (table[xi][y] == 'G') { ye = y; break; }
	}
	// If the search found another Green piece then go through the pieces in between and mark them to be flipped.
	if (ye != 0)
	{
		for (int y = yi - 1; y > ye; y--)
		{
			if (table[xi][y] == 'R') { table[xi][y] = 'g'; }
		}
	}
	// Search diagonal down/right from the moved piece to see if there is another Green piece.
	xe = 0;
	ye = 0;
	for (int a = 1; a < 8; a++)
	{
		// If x or y has got to the end board stop searching.
		if (((xi + a) > 8) || ((yi + a) > 8)) { break; }
		if (table[xi + a][yi + a] == ' ') { break; }		// If there is a gap stop searching.
		// If another Green piece is found capture it's position.
		if (table[xi + a][yi + a] == 'G') { xe = xi + a; ye = yi + a; break; }
	}
	// If the search found another Green piece then go through the pieces in between and mark them to be flipped.
	// Note xe and ye are set at the same time so only xe is checked.
	if (xe != 0)
	{
		// Count along to change captured pieces.
		for (int a = 1; a < 8; a++)
		{
			// If x or y has got to the end of the line stop capturing.
			if (((xi + a) >= xe) || ((yi + a) >= ye)) { break; }
			// change the opponent pieces.
			if (table[xi + a][yi + a] == 'R') { table[xi + a][yi + a] = 'g'; }
		}
	}
	// Search diagonal up/left from the moved piece to see if there is another Green piece.
	xe = 0;
	ye = 0;
	for (int a = 1; a < 8; a++)
	{
		// If x or y has got to the end board stop searching.
		if (((xi - a) < 1) || ((yi - a) < 1)) { break; }
		if (table[xi - a][yi - a] == ' ') { break; }		// If there is a gap stop searching.
		// If another Green piece is found capture it's position.
		if (table[xi - a][yi - a] == 'G') { xe = xi - a; ye = yi - a; break; }
	}
	// If the search found another Green piece then go through the pieces in between and mark them to be flipped.
	// Note xe and ye are set at the same time so only xe is checked.
	if (xe != 0)
	{
		// Count along to change captured pieces.
		for (int a = 1; a < 8; a++)
		{
			// If x or y has got to the end of the line stop capturing.
			if (((xi - a) <= xe) || ((yi - a) <= ye)) { break; }
			// change the opponent pieces.
			if (table[xi - a][yi - a] == 'R') { table[xi - a][yi - a] = 'g'; }
		}
	}
	// Search diagonal down/left from the moved piece to see if there is another Green piece.
	xe = 0;
	ye = 0;
	for (int a = 1; a < 8; a++)
	{
		// If x or y has got to the end board stop searching.
		if (((xi - a) < 1) || ((yi + a) > 8)) { break; }
		if (table[xi - a][yi + a] == ' ') { break; }		// If there is a gap stop searching.
		// If another Green piece is found capture it's position.
		if (table[xi - a][yi + a] == 'G') { xe = xi - a; ye = yi + a; break; }
	}
	// If the search found another Green piece then go through the pieces in between and mark them to be flipped.
	// Note xe and ye are set at the same time so only xe is checked.
	if (xe != 0)
	{
		// Count along to change captured pieces.
		for (int a = 1; a < 8; a++)
		{
			// If x or y has got to the end of the line stop capturing.
			if (((xi - a) <= xe) || ((yi + a) >= ye)) { break; }
			// change the opponent pieces.
			if (table[xi - a][yi + a] == 'R') { table[xi - a][yi + a] = 'g'; }
		}
	}
	// Search diagonal up/right from the moved piece to see if there is another Green piece.
	xe = 0;
	ye = 0;
	for (int a = 1; a < 8; a++)
	{
		// If x or y has got to the end board stop searching.
		if (((xi + a) > 8) || ((yi - a) < 1)) { break; }
		if (table[xi + a][yi - a] == ' ') { break; }		// If there is a gap stop searching.
		// If another Green piece is found capture it's position.
		if (table[xi + a][yi - a] == 'G') { xe = xi + a; ye = yi - a; break; }
	}
	// If the search found another Green piece then go through the pieces in between and mark them to be flipped.
	// Note xe and ye are set at the same time so only xe is checked.
	if (xe != 0)
	{
		// Count along to change captured pieces.
		for (int a = 1; a < 8; a++)
		{
			// If x or y has got to the end of the line stop capturing.
			if (((xi + a) >= xe) || ((yi - a) <= ye)) { break; }
			// change the opponent pieces.
			if (table[xi + a][yi - a] == 'R') { table[xi + a][yi - a] = 'g'; }
		}
	}
	return;
}

// Version of captureRed used by the main game processing which uses the actual game table.
void captureRed(int xi, int yi)
{
	captureRedWork(xi, yi, gameTable);
}


// Check the state of the board 'B' is for both players, 'R' for red and 'G' for green.
// For both players the function returns whether the game has ended 'E' or ' ' for not ended.
// If 'R' or 'G' is selected it returns 'M' for miss a turn if there are no valid moves for that player, or ' ' if there are valid moves.
char checkBoard(char pl, unsigned int *red, unsigned int *green )
{
	// Copy the current gameTable to the working copy so that it can be used for game processing.
	tableToWorking();

	unsigned int r = 0, g = 0;	// Working red and green counts.
	char ret = 'M';				// If there are no valid moves the player has to miss a turn.

	// If looking for game ended, need to identify all possible valid moves for either player.
	if (pl == 'B')
	{
		ret = 'E'; //Return value is changed to 'E' for ended when checking for both players.
		// Use the working table to avoid corrupting game table.
		// Mark all valid moves for both players.
		validRedMovesWork(workingTable);
		validGreenMovesWork(workingTable);
	}

	// If we are checking whether a player can make a move any valid moves will already be identified in the game board.
	// If any valid move is found the game is still going.
	// While checking for valid moves also count up the green and red pieces.
	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			if ((workingTable[x][y] == 'R') || (workingTable[x][y] == 'r')) { r++; }
			if ((workingTable[x][y] == 'G') || (workingTable[x][y] == 'g')) { g++; }
			if (workingTable[x][y] == 'V') { ret = ' '; }
		}
	}

	// Pass back the counts of red and green pieces and return parameter.
	*red = r;
	*green = g;
	return ret;
}
