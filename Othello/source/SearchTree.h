#ifndef SearchTreeHeader
#define SearchTreeHeader


#include <DeepLearning.h>
#include "OthelloBoard.h"

using namespace SupaDL;

struct TreeBranch {
public:
	Point Move;
	DataType BestEval;
	DataType StaticEval;
	int BestMove;
	TreeBranch* FollowingMoves[60]; // The middle 4 squares can never be moved to
	int TimesEvaluated;

	void Evaluate(int evaluations);
};

class SearchTree {
public:
	TreeBranch* Branches;
	int MaxBranches;
	int CurrentBranchIndex;

	char StartPosition[64];

	SearchTree(int HashSize, char* startPosition);
	~SearchTree();
};

#endif