#include "SearchTree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void TreeBranch::Evaluate(int times) {
	if (TimesEvaluated == 0) {
	}
}

SearchTree::SearchTree(int hashsize, char* startPosition) {
	memcpy(StartPosition, startPosition, 64);
	MaxBranches = hashsize / sizeof(TreeBranch);
	if (MaxBranches * sizeof(TreeBranch) > hashsize) MaxBranches--;
	Branches = (TreeBranch*)malloc(MaxBranches * sizeof(TreeBranch));
	Branches[0].BestMove = -1;
	Branches->Move = { -1, -1 };
	Branches[0].StaticEval = 0;
	Branches[0].BestEval = 0;
	for (int i = 0; i < 64; i++) {
		Branches[0].FollowingMoves[i] = NULL;
	}
	Branches[0].Evaluate(1);
}
SearchTree::~SearchTree() {
	free(Branches);
}