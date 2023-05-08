

#include "SearchTree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int TreeBranch::Evaluate(SearchTree* tree, int maxEvaluations) {
	int evalsLeft = maxEvaluations - EvaluateDirectChildren(tree, maxEvaluations);
	if(StalledMove != NULL) {
		tree->Board->MakeMove(Point {-1, -1});
		int n = StalledMove->Evaluate(tree, evalsLeft);
		tree->Board->UndoMove();
		return maxEvaluations - evalsLeft;
	}
	else {
		DataType total = 0;
		for(signed char x = 0;x < 8;x++) {
			for(signed char y = 0;y < 8;y++) {
				total += FollowingMoves[x][y]->StaticEvaluation.SearchImportance;
			}
		}
		for(signed char x = 0;x < 8;x++) {
			for(signed char y = 0;y < 8;y++) {
				tree->Board->MakeMove(Point {x, y});
				int maxTimes = (int)(FollowingMoves[x][y]->StaticEvaluation.SearchImportance / total) * evalsLeft;
				total -= FollowingMoves[x][y]->StaticEvaluation.SearchImportance;	
				evalsLeft -= FollowingMoves[x][y]->Evaluate(tree, maxTimes);
				tree->Board->UndoMove();
			}
		}
	}
	return maxEvaluations - evalsLeft;
}
void TreeBranch::StaticEvaluate(SearchTree* tree) {
	
}
int TreeBranch::EvaluateDirectChildren(SearchTree* tree, int maxEvaluations) {
	if(StalledMove != NULL) return 0;
	int evaluationsUsed = 0;
	for(signed char x = 0;x < 8;x++) {
		for(signed char y = 0;y < 8;y++) {
			if(tree->Board->MakeMove(Point {x, y})) {
				TreeBranch* branch = tree->ReserveBranch();
				FollowingMoves[x][y] = branch;
				branch->StaticEvaluate(tree);
				if(branch->StaticEvaluation.StaticEval > StaticEvaluation.StaticEval) {
					BestMove = {x, y};
					BestEval = branch->StaticEvaluation.StaticEval;
				}
			}
			else {
				FollowingMoves[x][y] = NULL;
			}
			tree->Board->UndoMove();
			if(evaluationsUsed >= maxEvaluations) return evaluationsUsed;
		}
	}
	if(evaluationsUsed == 0) {
		tree->Board->MakeMove(Point {-1, -1});
		StalledMove = tree->ReserveBranch();
		StalledMove->StaticEvaluate(tree);
		BestMove = {-1, -1};
		BestEval = StalledMove->StaticEvaluation.StaticEval;
		tree->Board->UndoMove();
	}
	return evaluationsUsed;
}


void SearchTree::Evaluate(int times) {
	Branches[0].Evaluate(this, times);
}
void SearchTree::Clear() {
	CurrentBranchIndex = 1;
	Branches[0].BestMove = {-2, -2};
	Branches[0].StaticEvaluation.SearchImportance = 0;
	Branches[0].StaticEvaluation.StaticEval = 0;
	Branches[0].BestEval = 0;
	Branches[0].StalledMove = NULL;
	for (int i = 0; i < 64; i++) {
		((TreeBranch**)(Branches[0].FollowingMoves))[i] = NULL;
	}
}
inline TreeBranch* SearchTree::ReserveBranch() {
	if(CurrentBranchIndex == MaxBranches) return NULL;
	return Branches + (CurrentBranchIndex++);
}
SearchTree::SearchTree(int hashsize, BoardPosition* board) {
	Board = board;
	MaxBranches = hashsize / sizeof(TreeBranch);
	if (MaxBranches * sizeof(TreeBranch) > hashsize) MaxBranches--;
	Branches = (TreeBranch*)malloc(MaxBranches * sizeof(TreeBranch));
	Clear();
}
SearchTree::~SearchTree() {
	free(Branches);
}