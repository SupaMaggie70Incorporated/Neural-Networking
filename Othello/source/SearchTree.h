#ifndef SearchTreeHeader
#define SearchTreeHeader


#include <DeepLearning.h>
#include "OthelloBoard.h"

using namespace SupaDL;

class SearchTree;


inline bool Compare(StaticEvalSet eval1, StaticEvalSet eval2, char turnNumber);

struct TreeBranch {
public:
	StaticEvalSet BestEval;
	StaticEvalSet StaticEval;
	DataType SearchImportance;
	Point BestMove;
	int FollowingMoves[8][8];
	int StalledMove;
	bool EvaluatedAllFollowing;

	int Evaluate(SearchTree* tree, int maxEvaluations, OptimizedNetwork* network);
	void StaticEvaluate(SearchTree* tree, OptimizedNetwork* network);
	int EvaluateDirectChildren(SearchTree* tree, int maxEvaluations, OptimizedNetwork* network);
};

class SearchTree {
public:
	TreeBranch* Branches;
	int MaxBranches;
	int CurrentBranchIndex;
	unsigned long TotalEvaluations;

	BoardPosition* Board;


	void Evaluate(int times, OptimizedNetwork* network);
	void Clear();
	inline int ReserveBranch();

	SearchTree(int HashSize, BoardPosition* board);
	~SearchTree();
};

#endif