#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <list>
#include <map>
#include <stack>

#include "SDL.h"
#include "SDL_gfxPrimitives.h"

#include "Card.h"
#include "enum.h"
#include "Move.h"
#include "Square.h"

class GameBoard
{
	public:
		// Constructor for the gameboard. We require lots of information.
		// TODO: This will need to be revised once non-Open games are
		//       allowed. It only needs the cards to generate moves, so
		//       it can use dummy cards or something.
		GameBoard(Piece startPiece, std::list<Card*> blueCards, std::list<Card*> redCards, bool isSame, bool isPlus, bool isSameWall, bool isElemental);
		
		// Two methods to handle both moving, and unmoving.
		void move(Move *move);
		void unmove();

		// Verifies that a given move is valid.
		bool isValidMove(Move *move);

		// In order to traverse the various directions on this board, we need a few constants.
		enum Direction
		{
			NW = -5,
			N  = -4,
			NE = -3,
			W  = -1,
			E  =  1,
			SW =  3,
			S  =  4,
			SE =  5
		};

		// Returns the valid move list for a given piece.
		void getMoveList(std::list<Move*> &moveList, Piece piece);
		Move* getMove(int row, int col, int index);

		// Returns whether or not the game is complete.
		bool gameComplete();

		// Returns the number of pieces the given piece has on the board.
		int getNumPieces(Piece piece);

		// Display the board.
		void printBoard();
		void drawBoard(SDL_Surface *surface);

		// Returns a pointer to the board for evaluation functions to use.
		Square* const getBoard();

		// Returns the number of empty squares.
		int getEmptySquares();

		// Returns the current zobrist key of the board.
		unsigned long long getZobristKey() { return this->_zobristKey; }

		// Returns the current piece to move.
		Piece getCurrentPiece() { return this->_currentPiece; }

		// Returns the opposite of the given piece.
		Piece getOppositePiece(Piece piece) { return (piece == PIECE_BLUE) ? PIECE_RED : PIECE_BLUE; }

	private:
		// Generates 64 bits of random data. 
		unsigned long long rand64();

		// Constant specifying the size of the board.
		// 3 rows of 3 squares, a sentry square for each row, two rows of sentry squares,
		// and one additional sentry square at the end.
		static const int BOARD_SIZE = 21;

		// Array that serves as the actual game board.
		// We use a single-dimensional array with sentry squares, for a total size of 21 squares.
		Square _gameBoard[BOARD_SIZE];

		// Array of zones, for determining which flip directions are possible.
		static int _squareZone[BOARD_SIZE];
		
		// The piece whose turn it currently is.
		Piece _currentPiece;

		// Series of stacks for saving the board state.
		std::stack<Square*> _gameBoardStack;
		std::stack<Piece> _pieceStack;
		std::stack<Move*> _moveStack;
		std::stack<unsigned long long> _zobristStack;
		std::stack<Card*> _cardStack;
		
		// The rules of the game.
		bool _isSame;
		bool _isPlus;
		bool _isSameWall;
		bool _isElemental;
		
		// The list of cards for each player.
		std::list<Card*> _blueCards;
		std::list<Card*> _redCards;
		
		// The available moves.
		std::map<Card*, Move*> _moveMap[3][3];

		// The current zobrist key of the board.
		unsigned long long _zobristKey;

		// Zobrist key elements. We use the same square number as the real grid, which
		// means there are some useless entries, but that's okay.
		unsigned long long _zobristGrid[BOARD_SIZE][4][10];
		unsigned long long _zobristRed;

		// The last move that was made.
		Move *_lastMove;
};

// Macros for fast conversion of a square number to row and column, and vice-versa.
// Converts to a 0-indexed system, and *ignores* sentry squares.
#define SQUARE_TO_ROW(x) (((x) / 4) - 1)
#define SQUARE_TO_COL(x) (((x) - 1) % 4)
#define ROWCOL_TO_SQUARE(x, y) ((((x) + 1) * 4) + ((y) + 1))

#endif
