#ifndef OthelloBoardHeader
#define OthelloBoardHeader

#include <DeepLearning.h>

#define NOPIECE 0
#define PLAYER1 1
#define PLAYER2 2
#define DRAW 3


struct Point {
	char x, y;
};


class OthelloBoard
{
	inline int GetOutputIndex(int x, int y) {
		return (2) + (x * 2) + (y * 16);
	}
	inline char BoardstateToChar(char boardstate) {
		if (boardstate == NOPIECE) return 'O';
		else if (boardstate == PLAYER1) return '+';
		else if (boardstate == PLAYER2) return '-';
		else return '?';
	}
	inline void SetBoardState(Point square, char value) {
		BoardState[square.x][square.y] = value;
	}
	inline char GetBoardState(Point square) {
		return BoardState[square.x][square.y];
	}
public:
	char Winner = 0;
	int MoveNumber;
	char PlayerTurn;
	char BoardState[8][8];
	char TemporaryBoardState[8][8];
	char OutputBuffer[130];
	~OthelloBoard();
	OthelloBoard();
	void PrintToConsole();
	void Move(Point square);
	/// <summary>
	/// Sets the temporary board state to 
	/// </summary>
	/// <param name="square"></param>
	/// <returns></returns>
	bool TryGetAfterMove(Point square);
	char GetWinner();
};

#endif