#ifndef OthelloBoardHeader
#define OthelloBoardHeader

#include <DeepLearning.h>
#include <stdint.h>

#define NOPIECE 0
#define UNDETERMINED 0

#define PLAYER1 1
#define PLAYER2 -1


struct Point {
	signed char x, y;
};

enum MoveState : char {
	Unchecked,
	NoMoves,
	ValidMoves
};
enum GameState : char {
	Undetermined,
	Player1,
	Player2,
	Draw
};


struct BoardPosition {
public:
	char OutputBuffer[130];
	signed char State[8][8];
	uint64_t History[8][8];
	int MoveNumber;
	bool GameEnded;
	bool LastMoveStalled;
	enum MoveState HasValidMoves;

	void Initialize();


	void PrintToConsole();
	bool CheckForValidMoves();
	bool IsMoveValid(Point spot);
	bool MakeMove(Point spot);
	void UndoMove();
	enum GameState GetWinner();

	
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