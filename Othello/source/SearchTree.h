#ifndef SearchTreeHeader
#define SearchTreeHeader


#include <DeepLearning.h>
#include "OthelloBoard.h"

using namespace SupaDL;

class SearchTree;

struct EvalSet {
	DataType StaticEval;
	DataType SearchImportance;
};

struct TreeBranch {
public:
	DataType BestEval;
	EvalSet StaticEvaluation;
	Point BestMove;
	TreeBranch* FollowingMoves[8][8];
	TreeBranch* StalledMove;

	int Evaluate(SearchTree* tree, int maxEvaluations);
	void StaticEvaluate(SearchTree* tree);
	int EvaluateDirectChildren(SearchTree* tree, int maxEvaluations);
};

class SearchTree {
public:
	TreeBranch* Branches;
	int MaxBranches;
	int CurrentBranchIndex;

	BoardPosition* Board;


	void Evaluate(int times);
	void Clear();
	inline TreeBranch* ReserveBranch();

	SearchTree(int HashSize, BoardPosition* board);
	~SearchTree();
};

#endif