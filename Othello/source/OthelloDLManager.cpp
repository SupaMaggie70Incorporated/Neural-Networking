#include "OthelloDLManager.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <new>



/// <summary>
/// Positive is in favor of player 1, negative in favor of player 2
/// </summary>
/// <returns></returns>
signed char OthelloDLManager::PlayPair() {
	Point FirstMoves[60];
	int startMoveCount = rand() % 61; // Between 0 and 60
	char lastStalled = false;
	for (int i = 0; i < startMoveCount; i++) {
		int bestValue = -1;
		Point bestSpot = { 127, 127 };
		for (char x = 0; x < 8; x++) {
			for (char y = 0; y < 8; y++) {
				Point p = { x, y };
				if (!Board->TryGetAfterMove(p)) continue;
				int value = rand();
				if (value > bestValue) {
					bestValue = value;
					bestSpot = p;
				}
			}
		}
		if (bestSpot.x == 127) {
			FirstMoves[i] = { 127 };
			Board->PlayerTurn = 127;
			if (lastStalled) {
				return 0;
			}
			else {
				lastStalled = true;
			}
			continue;
		}
		else {
			Board->Move(bestSpot);
			FirstMoves[i] = bestSpot;
			lastStalled = false;
		}
	}
	//Board->PrintToConsole();
	signed char result1 = Play();


	NeuralNetwork* currentNetwork = Network1;
	Network1 = Network2;
	Network2 = currentNetwork;

	this->Board = new(Board) OthelloBoard();
	for (int i = 0; i < startMoveCount; i++) {
		if (FirstMoves[i].x == 127) {
			Board->PlayerTurn = (Board->PlayerTurn == PLAYER1) ? PLAYER2 : PLAYER1;
		}
		else {
			Board->Move(FirstMoves[i]);
		}
	}
	signed char result2 = Play();
	return result1 - result2;
}

/// <summary>
/// Change this to play a pair of games, and play some random number of moves from the start so it isnt the same game every time
/// </summary>
/// <returns></returns>
signed char OthelloDLManager::Play() {
	char state;
	char lastStalled = false;
	while ((state = Board->GetWinner()) == 0) {
		char stalled = PlayMove();
		if (stalled && lastStalled) {
			// Count pieces
			int balance = 0;
			for (int x = 0; x < 8; x++) {
				for (int y = 0; y < 8; y++) {
					char piece = Board->BoardState[x][y];
					if (piece == PLAYER1) balance++;
					else if (piece == PLAYER2) balance--;
				}
			}
			if (balance > 0) return 1;
			else if (balance < 0) return -1;
			else if (balance == 0) return 0;
		}
		else {
			lastStalled = stalled;
		}
	}
	if (state == DRAW) return 0;
	else if (state == PLAYER1) return 1;
	else if (state == PLAYER2) return -1;
	else return 0;
}
inline DataType GetTypeFromChar(char piece, char turn) {
	if (piece == turn) return 1;
	else if (piece == NOPIECE) return 0;
	else return -1;
}
char OthelloDLManager::PlayMove() {
	Point bestSpot = { 127, 127 };
	DataType bestValue = -INFINITY;
	DataType otherEval = -69;
	NeuralNetwork* network = (Board->PlayerTurn == PLAYER1) ? Network1 : Network2;
	for (char x = 0; x < 8; x++) {
		for (char y = 0; y < 8; y++) {
			Point p = { x, y };
			if (!Board->TryGetAfterMove(p)) continue;

			for (int x1 = 0; x1 < 8; x1++) {
				for (int y1 = 0; y1 < 8; y1++) {
					InputBuffer[x1 + (y1 * 8)] = GetTypeFromChar(*Board->TemporaryBoardState[x1, y1], Board->PlayerTurn);
				}
			}
			network->Execute((DataType*)&InputBuffer, (DataType*)&OutputBuffer, NeuralNetworkExecutionBuffer);
			DataType value =
				//OutputBuffer[0] - OutputBuffer[2];
				OutputBuffer[0];
			if (value > bestValue) {
				bestSpot = p;
				bestValue = value;
				((Board->PlayerTurn == PLAYER1) ? Network1 : Network2)->Execute(InputBuffer, OutputBuffer, NeuralNetworkExecutionBuffer);
				otherEval = OutputBuffer[0];
			}
		}
	}
	if (bestSpot.x == 127) {
		Board->PlayerTurn = (Board->PlayerTurn == PLAYER1) ? PLAYER2 : PLAYER1;
		return true;
	}
	else {
		Board->Move(bestSpot);
		
	}
	return false;
}

void OthelloDLManager::Initialize(DLDomain* domain, NeuralNetwork* network1, NeuralNetwork* network2, DataType* executionBuffer, OthelloBoard* board) {
	
	board->~OthelloBoard();
	//this->Board = new(board) OthelloBoard();
	//this->Board = new OthelloBoard();
	this->Network1 = network1;
	this->Network2 = network2;
	this->NeuralNetworkExecutionBuffer = executionBuffer;
}