#include "OthelloBoard.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


OthelloBoard::~OthelloBoard() {

}
OthelloBoard::OthelloBoard() {
	OutputBuffer[0] = '\n';
	OutputBuffer[129] = 0;
	for (int i = 1; i < sizeof(OutputBuffer) - 1; i += 2) {
		if ((i - 1) % 16 == 0) {
			OutputBuffer[i] = '\n';
		}
		else {
			OutputBuffer[i] = ' ';
		}
	}
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			BoardState[x][y] = NOPIECE;
		}
	}
	BoardState[3][3] = PLAYER1;
	BoardState[4][4] = PLAYER1;
	BoardState[4][3] = PLAYER2;
	BoardState[3][4] = PLAYER2;
	PlayerTurn = PLAYER1;
	MoveNumber = 0;
}

void OthelloBoard::PrintToConsole() {
	for (int row = 7; row >= 0; row--) {
		for (int column = 0; column < 8; column++) {
			OutputBuffer[GetOutputIndex(column, row)] = BoardstateToChar(BoardState[column][row]);
		}
	}
	printf("%s", OutputBuffer);
	printf("\n");
}
void OthelloBoard::Move(Point square) {
	if (Winner != 0) return;
	if(TryGetAfterMove(square)) memcpy(&BoardState, &TemporaryBoardState, sizeof(BoardState));
	PlayerTurn = (PlayerTurn == PLAYER1) ? PLAYER2 : PLAYER1;
	if (++MoveNumber == 60) {
		Winner = GetWinner();
	}
}
char OthelloBoard::GetWinner() {
	int p1 = 0;
	int p2 = 0;
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			char state = BoardState[x][y];
			if (state == PLAYER1) p1++;
			else if (state == PLAYER2) p2++;
			else if (state == NOPIECE) return 0;
		}
	}
	if (p1 > p2) return PLAYER1;
	else if (p2 > p1) return PLAYER2;
	else return DRAW;
}
bool OthelloBoard::TryGetAfterMove(Point square) {
	memcpy(&TemporaryBoardState, &BoardState, sizeof(BoardState));
	if (TemporaryBoardState[square.x][square.y] != NOPIECE) {
		return false;
	}
	TemporaryBoardState[square.x][square.y] = PlayerTurn;
	
	char states = 127;
	bool hasSucceeded = false;

	for (int i = 1; i < 8; i++) {
		// Bottom left
		if ((states & 1) && square.x - i >= 0 && square.y - i >= 0) {
			char x = square.x - i;
			char y = square.y - i;
			char state = TemporaryBoardState[x][y];
			if (state == PlayerTurn) {
				states ^= 1; // Xor it out
				if (i != 1) {
					hasSucceeded = true;
					for (int j = 1; j < i; j++) {
						TemporaryBoardState[square.x - j][square.y - j] = PlayerTurn;
					}
				}
			}
			else if (state == NOPIECE) {
				states ^= 1; // Xor it out
			}
		}
		// Bottom
		if ((states & 2) && square.y - i >= 0) {
			char x = square.x;
			char y = square.y - i;
			char state = TemporaryBoardState[x][y];
			if (state == PlayerTurn) {
				states ^= 2; // Xor it out
				if (i != 1) {
					hasSucceeded = true;
					for (int j = 1; j < i; j++) {
						TemporaryBoardState[square.x][square.y - j] = PlayerTurn;
					}
				}
			}
			else if (state == NOPIECE) {
				states ^= 2; // Xor it out
			}
		}
		// Bottom right
		if ((states & 4) && square.x + i < 8 && square.y - i >= 0) {
			char x = square.x + i;
			char y = square.y - i;
			char state = TemporaryBoardState[x][y];
			if (state == PlayerTurn) {
				states ^= 4; // Xor it out
				if (i != 1) {
					hasSucceeded = true;
					for (int j = 1; j < i; j++) {
						TemporaryBoardState[square.x + j][square.y - j] = PlayerTurn;
					}
				}
			}
			else if (state == NOPIECE) {
				states ^= 4; // Xor it out
			}
		}
		// Right
		if ((states & 8) && square.x + i < 8) {
			char x = square.x + i;
			char y = square.y;
			char state = TemporaryBoardState[x][y];
			if (state == PlayerTurn) {
				states ^= 8; // Xor it out
				if (i != 1) {
					hasSucceeded = true;
					for (int j = 1; j < i; j++) {
						TemporaryBoardState[square.x + j][square.y] = PlayerTurn;
					}
				}
			}
			else if (state == NOPIECE) {
				states ^= 8; // Xor it out
			}
		}
		// Top Right
		if ((states & 16) && square.x + i < 8 && square.y + i < 8) {
			char x = square.x + i;
			char y = square.y + i;
			char state = TemporaryBoardState[x][y];
			if (state == PlayerTurn) {
				states ^= 16; // Xor it out
				if (i != 1) {
					hasSucceeded = true;
					for (int j = 1; j < i; j++) {
						TemporaryBoardState[square.x + j][square.y + j] = PlayerTurn;
					}
				}
			}
			else if (state == NOPIECE) {
				states ^= 16; // Xor it out
			}
		}
		// Top
		if ((states & 32) && square.y + i < 8) {
			char x = square.x;
			char y = square.y + i;
			char state = TemporaryBoardState[x][y];
			if (state == PlayerTurn) {
				states ^= 32; // Xor it out
				if (i != 1) {
					hasSucceeded = true;
					for (int j = 1; j < i; j++) {
						TemporaryBoardState[square.x][square.y + j] = PlayerTurn;
					}
				}
			}
			else if (state == NOPIECE) {
				states ^= 32; // Xor it out
			}
		}
		// Top Left
		if ((states & 64) && square.x - i >= 0 && square.y + i < 8) {
			char x = square.x - i;
			char y = square.y + i;
			char state = TemporaryBoardState[x][y];
			if (state == PlayerTurn) {
				states ^= 64; // Xor it out
				if (i != 1) {
					hasSucceeded = true;
					for (int j = 1; j < i; j++) {
						TemporaryBoardState[square.x - j][square.y + j] = PlayerTurn;
					}
				}
			}
			else if (state == NOPIECE) {
				states ^= 64; // Xor it out
			}
		}
		// Left
		if ((states & 128) && square.x - i >= 0) {
			char x = square.x - i;
			char y = square.y;
			char state = TemporaryBoardState[x][y];
			if (state == PlayerTurn) {
				states ^= 128; // Xor it out
				if (i != 1) {
					hasSucceeded = true;
					for (int j = 1; j < i; j++) {
						TemporaryBoardState[square.x - j][square.y] = PlayerTurn;
					}
				}
			}
			else if (state == NOPIECE) {
				states ^= 128; // Xor it out
			}
		}
	}

	return hasSucceeded;
}