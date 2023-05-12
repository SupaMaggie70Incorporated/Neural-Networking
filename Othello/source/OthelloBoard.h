#ifndef OthelloBoardHeader
#define OthelloBoardHeader

#include <DeepLearning.h>
#include <stdint.h>

#define NOPIECE 0
#define UNDETERMINED 0

#define PLAYER1 1
#define PLAYER2 -1


using namespace SupaDL;

struct LongLongBitwise {
	uint64_t i1;
	uint64_t i2;

	bool HasAtBit(unsigned char bit);
	void XorAtBit(unsigned char bit);
	void OrAtBit(unsigned char bit);
	void Clear();
};

enum WinnerState : char {
	Incomplete,
	Player1Win,
	Player2Win,
	Player1ForceWin,
	Player2ForceWin,
	Draw,
	ForceBad // For coding purposes, this means that this automatically loses to all other evaluations, for a temporary evaluation
};

struct StaticEvalSet {
	public:
	DataType Evaluation;
	enum WinnerState WinnerState;
};

struct Point {
	signed char x, y;
};

enum MoveState : char {
	Unchecked,
	NoMoves,
	ValidMoves
};


struct BoardPosition {
public:
	char OutputBuffer[130];
	signed char State[8][8];
	LongLongBitwise History[8][8];
	int MoveNumber;
	bool GameEnded;
	bool LastMoveStalled;
	enum MoveState HasValidMoves;

	void Initialize();
	void InitializeOutputBuffer();

	void CopyTo(BoardPosition* dest);
	void PrintToConsole();
	bool CheckForValidMoves();
	bool IsMoveValid(Point spot);
	bool MakeMove(Point spot);
	void UndoMove();
	StaticEvalSet GetWinner();

	
	inline int GetOutputIndex(int x, int y) {
		return (2) + (x * 2) + (y * 16);
	}
	inline char BoardstateToChar(char boardstate) {
		if (boardstate == NOPIECE) return 'O';
		else if (boardstate > 0) return '+';
		else if (boardstate < 0) return '-';
		else return '?';
	}
	inline void SetBoardState(Point square, char value) {
		State[square.x][square.y] = value;
	}
	inline char GetBoardState(Point square) {
		return State[square.x][square.y];
	}
};

#endif