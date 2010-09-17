#ifndef TRIPLETRIAD_PLAYER_HH
#define TRIPLETRIAD_PLAYER_HH

#include "game_board.hh"
#include "transposition_table.hh"

class TripleTriad;

class Player
{
	public:
		// Constructor for objects of type Player.
		Player(TripleTriad *game, GameBoard *gameBoard, Piece myPiece, Piece opponentPiece);
		~Player();

		// Makes a move.
		void move(unsigned int moveTime);
		
		// Sets the weights to use for evaluation.
		void setWeights(int pieceWeight, int mobilityWeight, int pMobilityWeight, int cornerWeight, int winWeight);

	private:
		// Various search methods.
		int minimaxSearch(int ply);
		int alphaBetaSearch(int ply, int alpha, int beta);
		int alphaBetaMemorySearch(int ply, int alpha, int beta);
		int MTDSearch(int ply, int firstGuess);

		// Evaluates the board.
		int evaluateBoard();

		// Variable to hold the current board.
		GameBoard *_gameBoard;

		// The true game board that we should move on.
		GameBoard *_realGameBoard;

		// The pieces of me and my opponent.
		Piece _myPiece;
		Piece _opponentPiece;

		// Transposition table for the Player.
		TranspositionTable _transpositionTable;

		// The time when searches should be aborted.
		unsigned int _moveTime;

		// Signals whether or not a ply was aborted.
		bool _abortedPly;

		// Statistic-related variables.
		int _positions;
		int _currentPly;
		int _maxPly;
		int _totalPly;
		int _searchCount;
		
		// Coefficients for the evaluation function.
		int _pieceWeight;
		int _mobilityWeight;
		int _pMobilityWeight;
		int _cornerWeight;
		int _winWeight;

		// Game
		TripleTriad *_game;
};

#endif
