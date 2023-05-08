#include "OthelloBoard.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



inline bool SameSign(signed char c1, signed char c2) {
	return (c1 == c2) || (c1 > 0 && c2 > 0) || (c1 < 0 && c2 < 0);
}
enum GameState BoardPosition::GetWinner() {
	if(GameEnded) return Undetermined;
	int balance = 0;
	for(int i = 0;i < 64;i++) {
		signed char val = ((signed char*)State)[i];
		if(val > 0) balance++;
		else if(val < 0) balance--;
	}
	if(balance == 0) return Draw;
	else if(balance > 0) return Player1;
	else if(balance < 0) return Player2;
}
void BoardPosition::Initialize() {
	memset(State, NOPIECE, sizeof(State));
	memset(History, 0, sizeof(History));
	State[3][3] = 127;
	State[4][4] = 127;
	State[4][3] = -127;
	State[3][4] = -127;
	MoveNumber = 0;
	GameEnded = false;
	LastMoveStalled = false;
	HasValidMoves = Unchecked;

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
}
bool BoardPosition::CheckForValidMoves() {
	if(HasValidMoves == ValidMoves) return true;
	if(HasValidMoves == NoMoves) return false;
	for(int x = 0;x < 8;x++) {
		for(int y = 0;y < 8;y++) {
			Point square = {x, y};
			if(IsMoveValid(square)) {
				HasValidMoves = ValidMoves;
				return true;
			}
		}
	}
	HasValidMoves = NoMoves;
	return false;
}
bool BoardPosition::IsMoveValid(Point spot) {
	signed char player = (MoveNumber & 1) ? -1 : 1;
	// Down left
	for(int i = 1;i < 8;i++) {
		Point square = {spot.x - i, spot.y - i};
		if(square.x < 0 || square.y < 0) break;
		signed char squareState = State[square.x][square.y];
		if(squareState == 0) break;
		else if(SameSign(squareState, player)) {
			if(i > 1) return true;
			break;
		}
	}
	// Down
	for(int i = 1;i < 8;i++) {
		Point square = {spot.x, spot.y - i};
		if(square.y < 0) break;
		signed char squareState = State[square.x][square.y];
		if(squareState == 0) break;
		else if(SameSign(squareState, player)) {
			if(i > 1) return true;
			break;
		}
	}
	// Down right
	for(int i = 1;i < 8;i++) {
		Point square = {spot.x + i, spot.y - i};
		if(square.x > 7 || square.y < 0) break;
		signed char squareState = State[square.x][square.y];
		if(squareState == 0) break;
		else if(SameSign(squareState, player)) {
			if(i > 1) return true;
			break;
		}
	}
	// Right
	for(int i = 1;i < 8;i++) {
		Point square = {spot.x + i, spot.y};
		if(square.x > 7) break;
		signed char squareState = State[square.x][square.y];
		if(squareState == 0) break;
		else if(SameSign(squareState, player)) {
			if(i > 1) return true;
			break;
		}
	}
	// Upper right
	for(int i = 1;i < 8;i++) {
		Point square = {spot.x + i, spot.y + i};
		if(square.x > 7 || square.y > 7) break;
		signed char squareState = State[square.x][square.y];
		if(squareState == 0) break;
		else if(SameSign(squareState, player)) {
			if(i > 1) return true;
			break;
		}
	}
	// Up
	for(int i = 1;i < 8;i++) {
		Point square = {spot.x, spot.y + i};
		if(square.y > 7) break;
		signed char squareState = State[square.x][square.y];
		if(squareState == 0) break;
		else if(SameSign(squareState, player)) {
			if(i > 1) return true;
			break;
		}
	}
	// Upper left
	for(int i = 1;i < 8;i++) {
		Point square = {spot.x - i, spot.y + i};
		if(square.x < 0 || square.y > 7) break;
		signed char squareState = State[square.x][square.y];
		if(squareState == 0) break;
		else if(SameSign(squareState, player)) {
			if(i > 1) return true;
			break;
		}
	}
	// Left
	for(int i = 1;i < 8;i++) {
		Point square = {spot.x - i, spot.y};
		if(square.x < 0) break;
		signed char squareState = State[square.x][square.y];
		if(squareState == 0) break;
		else if(SameSign(squareState, player)) {
			if(i > 1) return true;
			break;
		}
	}
	return false;
}
bool BoardPosition::MakeMove(Point spot) {
	if(spot.x < 0) {
		if(LastMoveStalled) {
			GameEnded = true;
			LastMoveStalled = true;
		}
		else LastMoveStalled = true;
		return true;
	}
	if(MoveNumber & 1) State[spot.x][spot.y] = -MoveNumber - 1;
	else State[spot.x][spot.y] = MoveNumber + 1;
	uint64_t hash = 1 << MoveNumber;
	signed char player = (MoveNumber & 1) ? -1 : 1;

	bool DoneAnything = false;
	LastMoveStalled = false;


	// Down left
	for(int i = 1;i < 8;i++) {
		Point square = {spot.x - i, spot.y - i};
		if(square.x < 0 || square.y < 0) break;
		signed char squareState = State[square.x][square.y];
		if(squareState == 0) break;
		else if(SameSign(squareState, player)) {
			for(int j = 1;j < i;j++) {
				Point square2 = {spot.x - j, spot.y - j};
				State[square2.x][square2.y] *= -1;
				History[square2.x][square2.y] |= hash;
			}
			if(i > 1) DoneAnything = true;
			break;
		}
	}
	// Down
	for(int i = 1;i < 8;i++) {
		Point square = {spot.x, spot.y - i};
		if(square.y < 0) break;
		signed char squareState = State[square.x][square.y];
		if(squareState == 0) break;
		else if(SameSign(squareState, player)) {
			for(int j = 1;j < i;j++) {
				Point square2 = {spot.x, spot.y - j};
				State[square2.x][square2.y] *= -1;
				History[square2.x][square2.y] |= hash;
			}
			if(i > 1) DoneAnything = true;
			break;
		}
	}
	// Down right
	for(int i = 1;i < 8;i++) {
		Point square = {spot.x + i, spot.y - i};
		if(square.x > 7 || square.y < 0) break;
		signed char squareState = State[square.x][square.y];
		if(squareState == 0) break;
		else if(SameSign(squareState, player)) {
			for(int j = 1;j < i;j++) {
				Point square2 = {spot.x + j, spot.y - j};
				State[square2.x][square2.y] *= -1;
				History[square2.x][square2.y] |= hash;
			}
			if(i > 1) DoneAnything = true;
			break;
		}
	}
	// Right
	for(int i = 1;i < 8;i++) {
		Point square = {spot.x + i, spot.y};
		if(square.x > 7) break;
		signed char squareState = State[square.x][square.y];
		if(squareState == 0) break;
		else if(SameSign(squareState, player)) {
			for(int j = 1;j < i;j++) {
				Point square2 = {spot.x + j, spot.y};
				State[square2.x][square2.y] *= -1;
				History[square2.x][square2.y] |= hash;
			}
			if(i > 1) DoneAnything = true;
			break;
		}
	}
	// Upper right
	for(int i = 1;i < 8;i++) {
		Point square = {spot.x + i, spot.y + i};
		if(square.x > 7 || square.y > 7) break;
		signed char squareState = State[square.x][square.y];
		if(squareState == 0) break;
		else if(SameSign(squareState, player)) {
			for(int j = 1;j < i;j++) {
				Point square2 = {spot.x + j, spot.y + j};
				State[square2.x][square2.y] *= -1;
				History[square2.x][square2.y] |= hash;
			}
			if(i > 1) DoneAnything = true;
			break;
		}
	}
	// Up
	for(int i = 1;i < 8;i++) {
		Point square = {spot.x, spot.y + i};
		if(square.y > 7) break;
		signed char squareState = State[square.x][square.y];
		if(squareState == 0) break;
		else if(SameSign(squareState, player)) {
			for(int j = 1;j < i;j++) {
				Point square2 = {spot.x, spot.y + j};
				State[square2.x][square2.y] *= -1;
				History[square2.x][square2.y] |= hash;
			}
			if(i > 1) DoneAnything = true;
			break;
		}
	}
	// Upper left
	for(int i = 1;i < 8;i++) {
		Point square = {spot.x - i, spot.y + i};
		if(square.x < 0 || square.y > 7) break;
		signed char squareState = State[square.x][square.y];
		if(squareState == 0) break;
		else if(SameSign(squareState, player)) {
			for(int j = 1;j < i;j++) {
				Point square2 = {spot.x - j, spot.y + j};
				State[square2.x][square2.y] *= -1;
				History[square2.x][square2.y] |= hash;
			}
			if(i > 1) DoneAnything = true;
			break;
		}
	}
	// Left
	for(int i = 1;i < 8;i++) {
		Point square = {spot.x - i, spot.y};
		if(square.x < 0) break;
		signed char squareState = State[square.x][square.y];
		if(squareState == 0) break;
		else if(SameSign(squareState, player)) {
			for(int j = 1;j < i;j++) {
				Point square2 = {spot.x - j, spot.y};
				State[square2.x][square2.y] *= -1;
				History[square2.x][square2.y] |= hash;
			}
			if(i > 1) DoneAnything = true;
			break;
		}
	}
	HasValidMoves = Unchecked;
	return DoneAnything;
}
void BoardPosition::UndoMove() {
	uint64_t hash = 1 << (MoveNumber - 1);
	for(int i = 0;i < 64;i++) {
		if(((char*)State)[i] == MoveNumber || ((char*)State)[i] == -MoveNumber) {
			((char*)State)[i] = NOPIECE;
		}
		if(((uint64_t*)History)[i] & hash) {
			((uint64_t*)History)[i] ^ hash;
			((char*)State)[i] *= -1;
		}
	}
	HasValidMoves = Unchecked;
	MoveNumber--;
}

void BoardPosition::PrintToConsole() {
	for (int row = 7; row >= 0; row--) {
		for (int column = 0; column < 8; column++) {
			OutputBuffer[GetOutputIndex(column, row)] = BoardstateToChar(State[column][row]);
		}
	}
	printf("%s", OutputBuffer);
	printf("\n");
}