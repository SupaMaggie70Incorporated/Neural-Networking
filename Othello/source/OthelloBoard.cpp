#include "OthelloBoard.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>



void LongLongBitwise::Clear() {
	i1 = 0;
	i2 = 0;
}

void LongLongBitwise::XorAtBit(unsigned char bit) {
	if(bit > 64) i2 ^= 1 << (bit - 64);
	else i1 ^= 1 << bit;
}
bool LongLongBitwise::HasAtBit(unsigned char bit) {
	if(bit > 64) return i2 & (1 << (bit - 64));
	else return i1 & (1 << bit);
}
void LongLongBitwise::SetAtBit(unsigned char bit) {
	if(bit > 64) i2 |= 1 << (bit - 64);
	else i1 |= 1 << bit;
}
void LongLongBitwise::UnsetAtBit(unsigned char bit) {
	if(bit > 64) i2 &= (ULLONG_MAX - 1 << (bit - 64));
	else i1 &= (ULLONG_MAX - 1 << bit);
}

inline bool SameSign(signed char c1, signed char c2) {
	bool value = (bool)((c1 == c2) || (c1 > 0 && c2 > 0) || (c1 < 0 && c2 < 0));
	return value;
	//return (bool)((c1 == c2) || (c1 > 0 && c2 > 0) || (c1 < 0 && c2 < 0));
}

void BoardPosition::CopyTo(BoardPosition* dest) {
	memcpy(dest->State, State, sizeof(State));
	memcpy(dest->History, History, sizeof(History));
	dest->MoveNumber = MoveNumber;
	dest->GameEnded = GameEnded;
	dest->LastMoveStalled = LastMoveStalled;
}
StaticEvalSet BoardPosition::GetWinner() { 
	if(!GameEnded) return StaticEvalSet {0, Incomplete};
	signed int p1 = 0;
	signed int p2 = 0;
	for(int i = 0;i < 64;i++) {
		signed char val = ((signed char*)State)[i];
		if(val > 0) p1++;
		else if(val < 0) p2++;
	}
	if(p1 == p2) return StaticEvalSet {0, Draw};
	else if(p1 == 0) return StaticEvalSet {0, Player2ForceWin};
	else if(p2 == 0) return StaticEvalSet {0, Player1ForceWin};
	else if(p1 > p2) return StaticEvalSet {(DataType)(p1 - p2), Player1Win};
	else if(p1 < p2) return StaticEvalSet {(DataType)(p2 - p1), Player2Win};
	return StaticEvalSet {0, ForceBad};
}
void BoardPosition::InitializeOutputBuffer() {
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
}
bool BoardPosition::HasValidMoves() {
	return !SkipHistory.HasAtBit(MoveNumber);
}

void BoardPosition::CheckForValidMoves() {
	for(int x = 0;x < 8;x++) {
		for(int y = 0;y < 8;y++) {
			if(IsMoveValid(Point {(signed char)x, (signed char)y})) {
				SkipHistory.UnsetAtBit(MoveNumber);
				GameEnded = false;
				return;
			}
		}
	}
	SkipHistory.SetAtBit(MoveNumber);
	if(LastMoveStalled) GameEnded = true;
}
bool BoardPosition::IsMoveValid(Point spot) {
	signed char player = (MoveNumber & 1) ? -1 : 1;
	// Down left
	if(spot.x != -1 && State[spot.x][spot.y] != NOPIECE) return false;
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
	if(GameEnded) return false;
	if(spot.x == -1) {
		MoveNumber++;
		if(LastMoveStalled) {
			GameEnded = true;
			LastMoveStalled = true;
		}
		else {
			LastMoveStalled = true;
			CheckForValidMoves();
		}
		return true;
	}
	if(State[spot.x][spot.y] != 0) return false;
	if(MoveNumber & 1) State[spot.x][spot.y] = -MoveNumber - 1;
	else State[spot.x][spot.y] = MoveNumber + 1;
	signed char player = (MoveNumber & 1) ? -1 : 1;

	bool DoneAnything = false;


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
				History[square2.x][square2.y].SetAtBit(MoveNumber);
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
				History[square2.x][square2.y].SetAtBit(MoveNumber);
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
				History[square2.x][square2.y].SetAtBit(MoveNumber);
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
				History[square2.x][square2.y].SetAtBit(MoveNumber);
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
				History[square2.x][square2.y].SetAtBit(MoveNumber);
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
				History[square2.x][square2.y].SetAtBit(MoveNumber);
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
				History[square2.x][square2.y].SetAtBit(MoveNumber);
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
				History[square2.x][square2.y].SetAtBit(MoveNumber);
			}
			if(i > 1) DoneAnything = true;
			break;
		}
	}
	if(DoneAnything) {
		MoveNumber++;
		CheckForValidMoves();
		LastMoveStalled = false;
		return true;
	}
	else {
		State[spot.x][spot.y] = 0;
		return false;
	}
}
void BoardPosition::UndoMove() {
	for(int i = 0;i < 64;i++) {
		if(((char*)State)[i] == MoveNumber || ((char*)State)[i] == -MoveNumber) {
			((char*)State)[i] = NOPIECE;
		}
		else if(((LongLongBitwise*)History)[i].HasAtBit(MoveNumber - 1)) {
			((LongLongBitwise*)History)[i].XorAtBit(MoveNumber - 1);
			((char*)State)[i] *= -1;
		}
	}
	SkipHistory.UnsetAtBit(MoveNumber - 1);
	MoveNumber--;
	GameEnded = false;
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