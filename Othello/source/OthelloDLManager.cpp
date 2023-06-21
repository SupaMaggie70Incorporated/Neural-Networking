#include "OthelloDLManager.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <new>



/// <summary>
/// Positive is in favor of player 1, negative in favor of player 2
/// </summary>
/// <returns></returns>
signed char OthelloDLManager::PlayPair(int evaluationsPerMove) {
}

/// <summary>
/// Im too lazy to delete this line  to play a pair of games, and play some random number of moves from the start so it isnt the same game every time
/// </summary>
/// <returns></returns>
StaticEvalSet OthelloDLManager::Play(int evaulationsPerMove) {
	//printf("Playing a game, starting at move #%d\n", Position.MoveNumber);
	//Position.PrintToConsole();
	while(!Position.GameEnded) {
		PlayMove(evaulationsPerMove);
	}
	return Position.GetWinner();
}
// Return true if player 2 wins
signed char OthelloDLManager::PlayTrainingPair(int evaluationsPerMove) {
	//printf("Playing a training pair\n");
	BoardPosition tempBoard;
	bool shouldContinue = true;
	while(shouldContinue) {
		shouldContinue = false;
		tempBoard.Initialize();
		int randMoveCount = rand() % 8;
		for(int i = 0;i < randMoveCount;i++) {
			Point currentBestMove = {-1, -1};
			int bestValue = 0;
			for(char x = 0;x < 8;x++) {
				for(char y = 0;y < 8;y++) {
					if(!tempBoard.IsMoveValid(Point {x, y})) continue;
					int r = rand();
					if(r > bestValue) {
						bestValue = r;
						currentBestMove = {x, y};
					}
				}
			}
			if(currentBestMove.x == -1) {
				shouldContinue = true;
				break;
			}
			tempBoard.MakeMove(currentBestMove);
			if(tempBoard.GameEnded) {
				shouldContinue = true;
				break;
			}
		}
	}
	tempBoard.CopyTo(&Position);
	StaticEvalSet r1 = Play(evaluationsPerMove);
	tempBoard.CopyTo(&Position);
	StaticEvalSet r2 = Play(evaluationsPerMove);
	if(r1.WinnerState == r2.WinnerState) {
		if(r1.WinnerState == Draw) return 0;
		else if(r1.WinnerState == Player1Win || r1.WinnerState == Player1ForceWin) return 1;
		else if(r1.WinnerState == Player2Win || r1.WinnerState == Player2ForceWin) return -1;
	}
	else if((r1.WinnerState == Player1ForceWin && r2.WinnerState != Player2ForceWin) || (r2.WinnerState == Player1ForceWin && r1.WinnerState != Player2ForceWin)) return 1;
	else if((r1.WinnerState == Player2ForceWin && r2.WinnerState != Player1ForceWin) || (r2.WinnerState == Player2ForceWin && r1.WinnerState != Player1ForceWin)) return -1;
	else if((r1.WinnerState == Player1Win && r2.WinnerState != Player2Win) || (r2.WinnerState == Player1Win && r1.WinnerState != Player2Win)) return 1;
	else if((r1.WinnerState == Player2Win && r2.WinnerState != Player1Win) || (r2.WinnerState == Player2Win && r1.WinnerState != Player1Win)) return -1;
	else if(r1.WinnerState == Player1Win && r2.WinnerState == Player2Win) {
		if(r1.Evaluation > r2.Evaluation) return 1;
		else if(r1.Evaluation < r2.Evaluation) return -1;
		else return 0;
	}
	else if(r1.WinnerState == Player2Win && r2.WinnerState == Player1Win) {
		if(r1.Evaluation > r2.Evaluation) return -1;
		else if(r1.Evaluation < r2.Evaluation) return 1;
		else return 0;
	}
	//printf("Finished a training pair\n");
}
inline DataType GetTypeFromChar(char piece, char turn) {
	if (piece == turn) return 1;
	else if (piece == NOPIECE) return 0;
	else return -1;
}
void OthelloDLManager::PlayMove(int evaluations) {
	//printf("Playing move number #%d\n", Position.MoveNumber);
	Tree->Clear();
	Tree->Evaluate(evaluations, (Position.MoveNumber & 1) ? Network2 : Network1);
	Point bestMove = Tree->Branches[0].BestMove;
	if(!Position.IsMoveValid(bestMove)) {
		Position.InitializeOutputBuffer();
		Position.PrintToConsole();
		printf("Invalid move recommended: %d, %d\n", (int)bestMove.x, (int)bestMove.y);
		exit(EXIT_FAILURE);
	}
	Position.MakeMove(bestMove);
	//printf("Played a move, (%d, %d), move #%d\n", (int)bestMove.x, (int)bestMove.y, Position.MoveNumber);
	//Tree->Board->PrintToConsole();

}

void OthelloDLManager::Initialize(DLDomain* domain, OptimizedNetwork* network1, OptimizedNetwork* network2, DataType* executionBuffer) {
	this->Domain = domain;
	this->Network1 = network1;
	this->Network2 = network2;
	this->NeuralNetworkExecutionBuffer = executionBuffer;
	Position.Initialize();
	Position.InitializeOutputBuffer();
}
void OthelloDLManager::InitSearchTree(int hashSize) {
	Tree = new SearchTree(hashSize, &Position);
}