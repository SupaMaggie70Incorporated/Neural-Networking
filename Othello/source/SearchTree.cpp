

#include "SearchTree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


// Returning true means that the second option is better
inline bool Compare(StaticEvalSet eval1, StaticEvalSet eval2, char turn) {
	if(eval2.WinnerState == ForceBad) return false;
	else if(eval1.WinnerState == ForceBad) return true;

	if(eval1.WinnerState == eval2.WinnerState) {
		if(eval1.WinnerState == Player1ForceWin || eval1.WinnerState == Player2ForceWin || eval1.WinnerState == Draw) return false;
		if(eval1.WinnerState == Player1Win) {
			if(turn & 1) {
				return eval2.Evaluation < eval1.Evaluation;
			}
			else {
				return eval2.Evaluation > eval1.Evaluation;
			}
		}
		if(eval1.WinnerState == Player2Win) {
			if(turn & 1) {
				return eval2.Evaluation > eval1.Evaluation;
			}
			else {
				return eval2.Evaluation < eval1.Evaluation;
			}
		}
		if(eval1.WinnerState == Incomplete) {
			if(turn & 1) {
				return eval2.Evaluation < eval1.Evaluation;
			}
			else {
				return eval2.Evaluation > eval1.Evaluation;
			}
		}
	}
	if((eval1.WinnerState == Player1ForceWin && eval2.WinnerState != Player1ForceWin) || (eval1.WinnerState != Player2ForceWin && eval2.WinnerState == Player2ForceWin)) return turn & 1;
	if((eval1.WinnerState != Player1ForceWin && eval2.WinnerState == Player1ForceWin) || (eval1.WinnerState == Player2ForceWin && eval2.WinnerState != Player2ForceWin)) return !(turn & 1);

	if((eval1.WinnerState == Player1Win && eval2.WinnerState != Player1Win) || (eval1.WinnerState != Player2Win && eval2.WinnerState == Player2Win)) return turn & 1;
	if((eval1.WinnerState != Player1Win && eval2.WinnerState == Player1Win) || (eval1.WinnerState == Player2Win && eval2.WinnerState != Player2Win)) return !(turn & 1);
	return false;
}

int TreeBranch::Evaluate(SearchTree* tree, int maxEvaluations, OptimizedNetwork* network) {
	//printf("Evaluating\n");
	//printf("Evaluating the following position, move number %d\n", tree->Board->MoveNumber);
	//tree->Board->PrintToConsole();
	if(StaticEval.WinnerState != Incomplete) {
		return 0;
	}
	int evalsLeft = maxEvaluations - EvaluateDirectChildren(tree, maxEvaluations, network);
	//printf("Finished evaluating direct children, %d\n", tree->Board->MoveNumber);
	if(StalledMove != -1) {
		tree->Board->MakeMove(Point {-1, -1});
		//printf("(One-Mover) ");
		int n = tree->Branches[StalledMove].Evaluate(tree, evalsLeft, network);
		evalsLeft -= n;
		tree->Board->UndoMove();
		//printf("Finished evaluating a no-mover, %d\n", tree->Board->MoveNumber);
		return maxEvaluations - evalsLeft;
	}
	else {
		int evalsAllowed = evalsLeft;
		BestEval.WinnerState = ForceBad;
		DataType total = 0;
		for(signed char x = 0;x < 8;x++) {
			for(signed char y = 0;y < 8;y++) {
				if(FollowingMoves[x][y] == -1) continue;
				TreeBranch* branches = tree->Branches;
				TreeBranch* branch = branches + FollowingMoves[x][y];
				DataType searchImportance = branch->SearchImportance;
				total += abs(searchImportance);
			}
		}
		for(signed char x = 0;x < 8;x++) {
			for(signed char y = 0;y < 8;y++) {
				if(FollowingMoves[x][y] >= 0) {
					if(!tree->Board->MakeMove({x, y})) continue; // This should not happen
					int maxTimes = (int)(tree->Branches[FollowingMoves[x][y]].SearchImportance * (DataType)evalsAllowed / (DataType)total);
					evalsLeft -= tree->Branches[FollowingMoves[x][y]].Evaluate(tree, maxTimes, network);
					//printf("Finished an evaluation, %d\n", tree->Board->MoveNumber);
					if(Compare(BestEval, tree->Branches[FollowingMoves[x][y]].BestEval, tree->Board->MoveNumber - 1)) {
						BestEval = tree->Branches[FollowingMoves[x][y]].BestEval;
						BestMove = {x, y};
					}
					tree->Board->UndoMove();
				}
			}
		}
	}
	return maxEvaluations - evalsLeft;
}
void TreeBranch::StaticEvaluate(SearchTree* tree, OptimizedNetwork* network) {
	//printf("Static evaluating\n");
	//printf("Statically evaluating the following position, move number %d\n", tree->Board->MoveNumber);
	//tree->Board->PrintToConsole();
	tree->TotalEvaluations++;
	if(tree->Board->GameEnded) {
		int player1Pieces = 0;
		int player2Pieces = 0;
		for(signed char x = 0;x < 8;x++) {
			for(signed char y = 0;y < 8;y++) {
				signed char state = tree->Board->State[x][y];
				if(state > 0) player1Pieces++;
				else if(state < 0) player2Pieces++;
			}
		}
		if(player1Pieces == 0 && player2Pieces > 0) {
			StaticEval.WinnerState = Player2ForceWin;
		}
		else if(player1Pieces > 0 && player2Pieces == 0) {
			StaticEval.WinnerState = Player1ForceWin;
		}
		else if(player1Pieces > player2Pieces) {
			StaticEval.Evaluation = player1Pieces - player2Pieces;
			StaticEval.WinnerState = Player1Win;
		}
		else if(player1Pieces < player2Pieces) {
			StaticEval.Evaluation = player2Pieces - player1Pieces;
			StaticEval.WinnerState = Player2Win;
		}
		else {
			StaticEval.Evaluation = 0;
			StaticEval.WinnerState = Draw;
		}
		BestEval = StaticEval;
		SearchImportance = 0;
		return;
	}
	DataType Inputs[128];
	for(int i = 0; i < 64;i++) {
		Inputs[i] = (((signed char*)(tree->Board->State))[i] == 1) ? 1 : 0;
		Inputs[i + 64] = (((signed char*)(tree->Board->State))[i] == -1) ? 1 : 0;
	}
	DataType Outputs[2];
	network->Execute(Inputs, Outputs);
	StaticEval.WinnerState = Incomplete;
	StaticEval.Evaluation = Outputs[0];
	BestEval = StaticEval;
	SearchImportance = Outputs[1];
}
int TreeBranch::EvaluateDirectChildren(SearchTree* tree, int maxEvaluations, OptimizedNetwork* network) {
	//printf("Evaluating direct children\n");
	//printf("Evaluating direct children of the following position, move %d\n", tree->Board->MoveNumber);
	//tree->Board->PrintToConsole();
	if(EvaluatedAllFollowing || maxEvaluations == 0 || StalledMove != -1) return 0;
	int evaluationsUsed = 0;
	bool validMoves = false;

	for(signed char x = 0;x < 8;x++) {
		for(signed char y = 0;y < 8;y++) {
			if(FollowingMoves[x][y] == -1 && tree->Board->MakeMove(Point {x, y})) {
				int branch = tree->ReserveBranch();
				FollowingMoves[x][y] = branch;
				evaluationsUsed++;
				tree->Branches[branch].StaticEvaluate(tree, network);
				if(Compare(BestEval, tree->Branches[branch].StaticEval, tree->Board->MoveNumber - 1)) {
					BestMove = {x, y};
					BestEval = tree->Branches[branch].StaticEval;
				}
				tree->Board->UndoMove();
				validMoves = true;
			}
			else if(FollowingMoves[x][y] != -1) validMoves = true;
			if(evaluationsUsed >= maxEvaluations) return evaluationsUsed;
		}
	}
	if(!validMoves && StalledMove == -1) {
		tree->Board->MakeMove(Point {-1, -1});
		StalledMove = tree->ReserveBranch();
		tree->Branches[StalledMove].StaticEvaluate(tree, network);
		BestMove = {-1, -1};
		BestEval = tree->Branches[StalledMove].BestEval;
		tree->Board->UndoMove();
		evaluationsUsed++;
	}
	EvaluatedAllFollowing = true;
	return evaluationsUsed;
}


void SearchTree::Evaluate(int times, OptimizedNetwork* network) {
	int used = Branches[0].EvaluateDirectChildren(this, times, network);
	printf("Evaluating with low focus\n");
	if(used == 1 || Branches[0].BestMove.x == -1) return;
	printf("Passed test 1\n");
	WinnerState bestState = Branches[Branches[0].FollowingMoves[Branches[0].BestMove.x][Branches[0].BestMove.y]].StaticEval.WinnerState;
	if(bestState == Player1Win || bestState == Player2Win || bestState == Player1ForceWin || bestState == Player2ForceWin) return;
	printf("Evaluating in depth\n");
	Branches[0].Evaluate(this, times - used, network);
}
void SearchTree::Clear() {
	CurrentBranchIndex = 1;
	Branches[0].BestMove = {-1, -1};
	Branches[0].SearchImportance = 0;
	Branches[0].StaticEval.Evaluation = 0;
	Branches[0].StaticEval.WinnerState = Incomplete;
	Branches[0].BestEval.Evaluation = 0;
	Branches[0].BestEval.WinnerState = ForceBad;
	Branches[0].EvaluatedAllFollowing = false;
	Branches[0].StalledMove = -1;
	for (int i = 0; i < 64; i++) {
		((int*)(Branches[0].FollowingMoves))[i] = -1;
	}
}
inline int SearchTree::ReserveBranch() {
	if(CurrentBranchIndex == MaxBranches) return -1;
	for(int i = 0;i < 64;i++) {
		((int*)Branches[CurrentBranchIndex].FollowingMoves)[i] = -1;
	}
	Branches[CurrentBranchIndex].BestEval.WinnerState = ForceBad;
	Branches[CurrentBranchIndex].EvaluatedAllFollowing = false;
	Branches[CurrentBranchIndex].StalledMove = -1;
	Branches[CurrentBranchIndex].StaticEval.WinnerState = Incomplete;
	Branches[CurrentBranchIndex].BestEval.WinnerState = Incomplete;
	return CurrentBranchIndex++;
}
SearchTree::SearchTree(int hashsize, BoardPosition* board) {
	Board = board;
	TotalEvaluations = 0;
	MaxBranches = hashsize;
	Branches = (TreeBranch*)malloc(MaxBranches * sizeof(TreeBranch));
	Clear();
}
SearchTree::~SearchTree() {
	free(Branches);
}