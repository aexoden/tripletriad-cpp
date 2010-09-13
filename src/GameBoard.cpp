#include <assert.h>

#include "GameBoard.h"

GameBoard::GameBoard(Piece startPiece, std::list<Card*> blueCards, std::list<Card*> redCards, bool isSame, bool isPlus, bool isSameWall, bool isElemental)
{
	// Set up the game rules.
	this->_isSame = isSame;
	this->_isPlus = isPlus;
	this->_isSameWall = isSameWall;
	this->_isElemental = isElemental;
	
	// Store each player's card list.
	this->_blueCards = blueCards;
	this->_redCards = redCards;
	
	// Initialize the zobrist key database.
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			this->_zobristGrid[i][PIECE_BLUE][j] = this->rand64();
			this->_zobristGrid[i][PIECE_RED][j] = this->rand64();
		}
	}
	this->_zobristRed = this->rand64();

	this->_zobristKey = 0;
	
	// Generate the possible move list.
	for (int row = 0; row < 3; row++)
	{
		for (int col = 0; col < 3; col++)
		{
			std::list<Card*>::iterator iter = this->_blueCards.begin();
			while (iter != this->_blueCards.end())
			{
				this->_moveMap[row][col][(*iter)] = new Move(row, col, (*iter));
				
				iter++;
			}
			
			iter = this->_redCards.begin();
			while (iter != this->_redCards.end())
			{
				this->_moveMap[row][col][(*iter)] = new Move(row, col, (*iter));
				
				iter++;
			}
		}
	}

	// Initialize the initial game board.
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		this->_gameBoard[i].element = ELEMENT_NONE;
		if (SQUARE_TO_ROW(i) >= 0 && SQUARE_TO_ROW(i) < 3 && SQUARE_TO_COL(i) >= 0 && SQUARE_TO_COL(i) < 3)
		{

			this->_gameBoard[i].owner = PIECE_NONE;
			this->_gameBoard[i].row = SQUARE_TO_ROW(i);
			this->_gameBoard[i].col = SQUARE_TO_COL(i);
		}
		else
		{
			this->_gameBoard[i].owner = PIECE_INVALID;
		}
	}

	// Initialize the initial piece to be placed.
	this->_currentPiece = startPiece;

	this->_lastMove = NULL;
}

void GameBoard::move(Move *move)
{
	// Assert that this is a valid move.
	if (this->isValidMove(move))
	{
		// Save the current board state.
		Square *squareArray = new Square[BOARD_SIZE];
		memcpy(squareArray, this->_gameBoard, sizeof(Square) * BOARD_SIZE);
		this->_gameBoardStack.push(squareArray);
		this->_pieceStack.push(this->_currentPiece);
		this->_moveStack.push(this->_lastMove);
		this->_cardStack.push(move->card);
		this->_zobristStack.push(this->_zobristKey);

		// Add the actual card to the board.
		this->_gameBoard[ROWCOL_TO_SQUARE(move->row, move->col)].owner = this->_currentPiece;
		this->_gameBoard[ROWCOL_TO_SQUARE(move->row, move->col)].setCard(move->card);
		this->_zobristKey = this->_zobristKey ^ this->_zobristGrid[ROWCOL_TO_SQUARE(move->row, move->col)][this->_currentPiece][move->card->ID];
		
		// Declare a list of squares to check for combos.
		std::list<Square*> comboSquares;

		// Establish some basic variables.
		Square baseSquare = this->_gameBoard[ROWCOL_TO_SQUARE(move->row, move->col)];
		Card *baseCard = baseSquare.getCard();
		bool flipNorth = false;
		bool flipSouth = false;
		bool flipWest = false;
		bool flipEast = false;
		
		// Prepare various variables to make square and card access easier.
		Square *northSquare = this->_gameBoard + ROWCOL_TO_SQUARE(move->row, move->col) + N;
		Square *southSquare = this->_gameBoard + ROWCOL_TO_SQUARE(move->row, move->col) + S;
		Square *westSquare = this->_gameBoard + ROWCOL_TO_SQUARE(move->row, move->col) + W;
		Square *eastSquare = this->_gameBoard + ROWCOL_TO_SQUARE(move->row, move->col) + E;
		Card *northCard = northSquare->getCard();
		Card *southCard = southSquare->getCard();
		Card *eastCard = eastSquare->getCard();
		Card *westCard = westSquare->getCard();		
		
		// Execute the same rule, if enabled.
		if (this->_isSame)
		{
			// Check the north and east set.
			if (move->col < 2 && move->row > 0 && northCard != NULL && eastCard != NULL && baseCard->top == northCard->bottom && baseCard->right == eastCard->left)
			{
				flipNorth = true;
				flipEast = true;
			}
			else if (this->_isSameWall && move->col == 2 && move->row > 0 && northCard != NULL && baseCard->right == 10 && baseCard->top == northCard->bottom)
			{
				flipNorth = true;
			}
			else if (this->_isSameWall && move->col < 2 && move->row == 0 && eastCard != NULL && baseCard->top == 10 && baseCard->right == eastCard->left)
			{
				flipEast = true;
			}
			
			// Check the north and west set.
			if (move->col > 0 && move->row > 0 && northCard != NULL && westCard != NULL && baseCard->top == northCard->bottom && baseCard->left == westCard->right)
			{
				flipNorth = true;
				flipWest = true;
			}
			else if (this->_isSameWall && move->col == 0 && move->row > 0 && northCard != NULL && baseCard->left == 10 && baseCard->top == northCard->bottom)
			{
				flipNorth = true;
			}
			else if (this->_isSameWall && move->col > 0 && move->row == 0 && westCard != NULL && baseCard->top == 10 && baseCard->left == westCard->right)
			{
				flipWest = true;
			}
			
			// Check the south and east set.
			if (move->col < 2 && move->row < 2 && southCard != NULL && eastCard != NULL && baseCard->bottom == southCard->top && baseCard->right == eastCard->left)
			{
				flipSouth = true;
				flipEast = true;
			}
			else if (this->_isSameWall && move->col == 2 && move->row < 2 && southCard != NULL && baseCard->right == 10 && baseCard->bottom == southCard->top)
			{
				flipSouth = true;
			}
			else if (this->_isSameWall && move->col < 2 && move->row == 2 && eastCard != NULL && baseCard->bottom == 10 && baseCard->right == eastCard->left)
			{
				flipEast = true;
			}
			
			// Check the south and west set.
			if (move->col > 0 && move->row < 2 && southCard != NULL && westCard != NULL && baseCard->bottom == southCard->top && baseCard->left == westCard->right)
			{
				flipSouth = true;
				flipWest = true;
			}
			else if (this->_isSameWall && move->col == 0 && move->row < 2 && southCard != NULL && baseCard->left == 10 && baseCard->bottom == southCard->top)
			{
				flipSouth = true;
			}
			else if (this->_isSameWall && move->col > 0 && move->row == 2 && westCard != NULL && baseCard->bottom == 10 && baseCard->left == westCard->right)
			{
				flipWest = true;
			}
			
			// Check the north and south set.
			if (move->row == 1 && southCard != NULL && northCard != NULL && baseCard->bottom == southCard->top && baseCard->top == northCard->bottom)
			{
				flipNorth = true;
				flipSouth = true;
			}
			else if (this->_isSameWall && move->row == 2 && northCard != NULL && baseCard->bottom == 10 && baseCard->top == northCard->bottom)
			{
				flipNorth = true;
			}
			else if (this->_isSameWall && move->row == 0 && southCard != NULL && baseCard->top == 10 && baseCard->bottom == southCard->top)
			{
				flipSouth = true;
			}
			
			// Check the east and west set.
			if (move->col == 1 && eastCard != NULL && westCard != NULL && baseCard->right == eastCard->left && baseCard->left == westCard->right)
			{
				flipEast = true;
				flipWest = true;
			}
			else if (this->_isSameWall && move->col == 2 && westCard != NULL && baseCard->right == 10 && baseCard->left == westCard->right)
			{
				flipWest = true;
			}
			else if (this->_isSameWall && move->col == 0 && eastCard != NULL && baseCard->left == 10 && baseCard->right == eastCard->left)
			{
				flipEast = true;
			}
		}
		
		// Execute the plus rule, if enabled.
		if (this->_isPlus)
		{
			// Check the north and east set.
			if (move->col < 2 && move->row > 0 && northCard != NULL && eastCard != NULL && (baseCard->top + northCard->bottom == baseCard->right + eastCard->left))
			{
				flipNorth = true;
				flipEast = true;
			}
			
			// Check the north and west set.
			if (move->col > 0 && move->row > 0 && northCard != NULL && westCard != NULL && (baseCard->top + northCard->bottom == baseCard->left + westCard->right))
			{
				flipNorth = true;
				flipWest = true;
			}
			
			// Check the south and east set.
			if (move->col < 2 && move->row < 2 && southCard != NULL && eastCard != NULL && (baseCard->bottom + southCard->top == baseCard->right + eastCard->left))
			{
				flipSouth = true;
				flipEast = true;
			}
			
			// Check the south and west set.
			if (move->col > 0 && move->row < 2 && southCard != NULL && westCard != NULL && (baseCard->bottom + southCard->top == baseCard->left + westCard->right))
			{
				flipSouth = true;
				flipWest = true;
			}
			
			// Check the north and south set.
			if (move->row == 1 && southCard != NULL && northCard != NULL && (baseCard->bottom + southCard->top == baseCard->top + northCard->bottom))
			{
				flipNorth = true;
				flipSouth = true;
			}
			
			// Check the east and west set.
			if (move->col == 1 && eastCard != NULL && westCard != NULL && (baseCard->right + eastCard->left == baseCard->left + westCard->right))
			{
				flipEast = true;
				flipWest = true;
			}			
		}

		// Check each of the four cards to see if it can be flipped.
		if (flipNorth == true)
		{
			if (northSquare->owner == this->getOppositePiece(this->_currentPiece))
			{
				northSquare->owner = this->_currentPiece;
				comboSquares.push_back(northSquare);
				this->_zobristKey = this->_zobristKey ^ this->_zobristGrid[ROWCOL_TO_SQUARE(move->row, move->col) + N][this->getOppositePiece(this->_currentPiece)][northCard->ID];
				this->_zobristKey = this->_zobristKey ^ this->_zobristGrid[ROWCOL_TO_SQUARE(move->row, move->col) + N][this->_currentPiece][northCard->ID];					
			}
		}
		
		if (flipSouth == true)
		{
			if (southSquare->owner == this->getOppositePiece(this->_currentPiece))
			{
				southSquare->owner = this->_currentPiece;
				comboSquares.push_back(southSquare);
				this->_zobristKey = this->_zobristKey ^ this->_zobristGrid[ROWCOL_TO_SQUARE(move->row, move->col) + S][this->getOppositePiece(this->_currentPiece)][southCard->ID];
				this->_zobristKey = this->_zobristKey ^ this->_zobristGrid[ROWCOL_TO_SQUARE(move->row, move->col) + S][this->_currentPiece][southCard->ID];
			}
		}
		
		if (flipEast == true)
		{
			if (eastSquare->owner == this->getOppositePiece(this->_currentPiece))
			{
				eastSquare->owner = this->_currentPiece;
				comboSquares.push_back(eastSquare);
				this->_zobristKey = this->_zobristKey ^ this->_zobristGrid[ROWCOL_TO_SQUARE(move->row, move->col) + E][this->getOppositePiece(this->_currentPiece)][eastCard->ID];
				this->_zobristKey = this->_zobristKey ^ this->_zobristGrid[ROWCOL_TO_SQUARE(move->row, move->col) + E][this->_currentPiece][eastCard->ID];
			}
		}
		
		if (flipWest == true)
		{
			if (westSquare->owner == this->getOppositePiece(this->_currentPiece))
			{
				westSquare->owner = this->_currentPiece;
				comboSquares.push_back(westSquare);
				this->_zobristKey = this->_zobristKey ^ this->_zobristGrid[ROWCOL_TO_SQUARE(move->row, move->col) + W][this->getOppositePiece(this->_currentPiece)][westCard->ID];
				this->_zobristKey = this->_zobristKey ^ this->_zobristGrid[ROWCOL_TO_SQUARE(move->row, move->col) + W][this->_currentPiece][westCard->ID];
			}
		}
		
		// Execute any applicable combo rules.
		while (comboSquares.size() > 0)
		{
			Square *baseSquare = comboSquares.front();
			Card *baseCard = baseSquare->getCard();
			comboSquares.pop_front();
			
			if (baseSquare->col > 0)
			{
				Square *attackSquare = this->_gameBoard + ROWCOL_TO_SQUARE(baseSquare->row, baseSquare->col) + W;
				Card *attackCard = attackSquare->getCard();
				if (attackSquare->owner == this->getOppositePiece(this->_currentPiece) && baseCard->left > attackCard->right)
				{
					attackSquare->owner = this->_currentPiece;
					comboSquares.push_back(attackSquare);
					this->_zobristKey = this->_zobristKey ^ this->_zobristGrid[ROWCOL_TO_SQUARE(baseSquare->row, baseSquare->col) + W][this->getOppositePiece(this->_currentPiece)][attackCard->ID];
					this->_zobristKey = this->_zobristKey ^ this->_zobristGrid[ROWCOL_TO_SQUARE(baseSquare->row, baseSquare->col) + W][this->_currentPiece][attackCard->ID];
				}
			}
			if (baseSquare->col > 0)
			{
				Square *attackSquare = this->_gameBoard + ROWCOL_TO_SQUARE(baseSquare->row, baseSquare->col) + N;
				Card *attackCard = attackSquare->getCard();
				if (attackSquare->owner == this->getOppositePiece(this->_currentPiece) && baseCard->top > attackCard->bottom)
				{
					attackSquare->owner = this->_currentPiece;
					comboSquares.push_back(attackSquare);
					this->_zobristKey = this->_zobristKey ^ this->_zobristGrid[ROWCOL_TO_SQUARE(baseSquare->row, baseSquare->col) + N][this->getOppositePiece(this->_currentPiece)][attackCard->ID];
					this->_zobristKey = this->_zobristKey ^ this->_zobristGrid[ROWCOL_TO_SQUARE(baseSquare->row, baseSquare->col) + N][this->_currentPiece][attackCard->ID];
				}
			}
			if (baseSquare->col < 2)
			{
				Square *attackSquare = this->_gameBoard + ROWCOL_TO_SQUARE(baseSquare->row, baseSquare->col) + E;
				Card *attackCard = attackSquare->getCard();
				if (attackSquare->owner == this->getOppositePiece(this->_currentPiece) && baseCard->right > attackCard->left)
				{
					attackSquare->owner = this->_currentPiece;
					comboSquares.push_back(attackSquare);
					this->_zobristKey = this->_zobristKey ^ this->_zobristGrid[ROWCOL_TO_SQUARE(baseSquare->row, baseSquare->col) + E][this->getOppositePiece(this->_currentPiece)][attackCard->ID];
					this->_zobristKey = this->_zobristKey ^ this->_zobristGrid[ROWCOL_TO_SQUARE(baseSquare->row, baseSquare->col) + E][this->_currentPiece][attackCard->ID];
				}
			}
			if (baseSquare->row < 2)
			{
				Square *attackSquare = this->_gameBoard + ROWCOL_TO_SQUARE(baseSquare->row, baseSquare->col) + S;
				Card *attackCard = attackSquare->getCard();
				if (attackSquare->owner == this->getOppositePiece(this->_currentPiece) && baseCard->bottom > attackCard->top)
				{
					attackSquare->owner = this->_currentPiece;
					comboSquares.push_back(attackSquare);
					this->_zobristKey = this->_zobristKey ^ this->_zobristGrid[ROWCOL_TO_SQUARE(baseSquare->row, baseSquare->col) + S][this->getOppositePiece(this->_currentPiece)][attackCard->ID];
					this->_zobristKey = this->_zobristKey ^ this->_zobristGrid[ROWCOL_TO_SQUARE(baseSquare->row, baseSquare->col) + S][this->_currentPiece][attackCard->ID];
				}
			}
		}
		
		// Execute the basic flip rule for each of the four directions.
		if (move->col > 0)
		{
			Square *attackSquare = this->_gameBoard + ROWCOL_TO_SQUARE(move->row, move->col) + W;
			Card *attackCard = attackSquare->getCard();
			if (attackSquare->owner == this->getOppositePiece(this->_currentPiece) && (baseCard->left + baseSquare.elementalBonus) > (attackCard->right + attackSquare->elementalBonus))
			{
				attackSquare->owner = this->_currentPiece;
				this->_zobristKey = this->_zobristKey ^ this->_zobristGrid[ROWCOL_TO_SQUARE(move->row, move->col) + W][this->getOppositePiece(this->_currentPiece)][attackCard->ID];
				this->_zobristKey = this->_zobristKey ^ this->_zobristGrid[ROWCOL_TO_SQUARE(move->row, move->col) + W][this->_currentPiece][attackCard->ID];
			}
		}
		if (move->row > 0)
		{
			Square *attackSquare = this->_gameBoard + ROWCOL_TO_SQUARE(move->row, move->col) + N;
			Card *attackCard = attackSquare->getCard();
			if (attackSquare->owner == this->getOppositePiece(this->_currentPiece) && (baseCard->top + baseSquare.elementalBonus) > (attackCard->bottom + attackSquare->elementalBonus))
			{
				attackSquare->owner = this->_currentPiece;
				this->_zobristKey = this->_zobristKey ^ this->_zobristGrid[ROWCOL_TO_SQUARE(move->row, move->col) + N][this->getOppositePiece(this->_currentPiece)][attackCard->ID];
				this->_zobristKey = this->_zobristKey ^ this->_zobristGrid[ROWCOL_TO_SQUARE(move->row, move->col) + N][this->_currentPiece][attackCard->ID];
			}
		}
		if (move->col < 2)
		{
			Square *attackSquare = this->_gameBoard + ROWCOL_TO_SQUARE(move->row, move->col) + E;
			Card *attackCard = attackSquare->getCard();
			if (attackSquare->owner == this->getOppositePiece(this->_currentPiece) && (baseCard->right + baseSquare.elementalBonus) > (attackCard->left + attackSquare->elementalBonus))
			{
				attackSquare->owner = this->_currentPiece;
				this->_zobristKey = this->_zobristKey ^ this->_zobristGrid[ROWCOL_TO_SQUARE(move->row, move->col) + E][this->getOppositePiece(this->_currentPiece)][attackCard->ID];
				this->_zobristKey = this->_zobristKey ^ this->_zobristGrid[ROWCOL_TO_SQUARE(move->row, move->col) + E][this->_currentPiece][attackCard->ID];
			}
		}
		if (move->row < 2)
		{
			Square *attackSquare = this->_gameBoard + ROWCOL_TO_SQUARE(move->row, move->col) + S;
			Card *attackCard = attackSquare->getCard();
			if (attackSquare->owner == this->getOppositePiece(this->_currentPiece) && (baseCard->bottom + baseSquare.elementalBonus) > (attackCard->top + attackSquare->elementalBonus))
			{
				attackSquare->owner = this->_currentPiece;
				this->_zobristKey = this->_zobristKey ^ this->_zobristGrid[ROWCOL_TO_SQUARE(move->row, move->col) + S][this->getOppositePiece(this->_currentPiece)][attackCard->ID];
				this->_zobristKey = this->_zobristKey ^ this->_zobristGrid[ROWCOL_TO_SQUARE(move->row, move->col) + S][this->_currentPiece][attackCard->ID];
			}
		}
		
		// Remove the current card from the player's list.
		if (this->_currentPiece == PIECE_BLUE)
		{
			int oldSize = this->_blueCards.size();
			this->_blueCards.remove(move->card);
			if (this->_blueCards.size() == oldSize)
			{
				std::cout << "Failed to remove blue card!" << std::endl;
			}
		}
		else
		{
			int oldSize = this->_redCards.size();
			this->_redCards.remove(move->card);
			if (this->_redCards.size() == oldSize)
			{
				std::cout << "Failed to remove red card from " << (*move) << std::endl;
				this->printBoard();
			}
		}
		
		// Change the current piece to be played.
		this->_currentPiece = (this->_currentPiece == PIECE_BLUE) ? PIECE_RED : PIECE_BLUE;
		this->_zobristKey = this->_zobristKey ^ this->_zobristRed;
		
		this->_lastMove = move;
	}
	else
	{
		// Save the current board state.
		Square *squareArray = new Square[BOARD_SIZE];
		memcpy(squareArray, this->_gameBoard, sizeof(Square) * BOARD_SIZE);
		this->_gameBoardStack.push(squareArray);
		this->_pieceStack.push(this->_currentPiece);
		this->_moveStack.push(this->_lastMove);
		this->_cardStack.push(move->card);
		this->_zobristStack.push(this->_zobristKey);
		std::cout << "Attempted to make invalid move " << (*move) << " with current Zobrist key " << this->_zobristKey << std::endl;
	}
}

void GameBoard::unmove()
{
	// Restore the game state.
	Square* squareArray = (Square*)this->_gameBoardStack.top();
	memcpy(this->_gameBoard, squareArray, sizeof(Square) * BOARD_SIZE);
	delete[] squareArray;
	this->_currentPiece = (Piece)this->_pieceStack.top();
	this->_lastMove = (Move*)this->_moveStack.top();
	this->_zobristKey = this->_zobristStack.top();
	
	// Replace the removed card.
	if (this->_currentPiece == PIECE_BLUE)
	{
		this->_blueCards.push_back(this->_cardStack.top());
	}
	else
	{
		this->_redCards.push_back(this->_cardStack.top());
	}	
	
	this->_gameBoardStack.pop();
	this->_pieceStack.pop();
	this->_moveStack.pop();
	this->_cardStack.pop();
	this->_zobristStack.pop();
}

// Initialize the square zone array.
int GameBoard::_squareZone[BOARD_SIZE] = { 0, 0, 0, 0,
                                           0, 1, 2, 3,
                                           0, 4, 5, 6,
                                           0, 7, 8, 9,
                                           0, 0, 0, 0, 0 };

bool GameBoard::isValidMove(Move *move)
{
	// A valid move must take place in an empty square.
	if (this->_gameBoard[ROWCOL_TO_SQUARE(move->row, move->col)].owner == PIECE_NONE)
	{
		// The current player must possess the card.
		std::list<Card*>::iterator iter = (this->_currentPiece == PIECE_BLUE) ? this->_blueCards.begin() : this->_redCards.begin();
		std::list<Card*>::iterator end = (this->_currentPiece == PIECE_BLUE) ? this->_blueCards.end() : this->_redCards.end();
		bool hasCard = false;
		while (iter != end)
		{
			if (move->card == (*iter))
			{
				hasCard = true;
			}
			iter++;
		}
		return hasCard;
	}
	else
	{
		return false;
	}
}

void GameBoard::getMoveList(std::list<Move*> &moveList, Piece piece)
{
	// Clear the given list, to make sure no stray entries remain.
	moveList.clear();
	
	// Iterate through our available cards, producing moves for each position.
	std::list<Card*>::iterator iter;
	std::list<Card*>::iterator end;
	switch (piece)
	{
		case PIECE_BLUE: iter = this->_blueCards.begin(); end = this->_blueCards.end(); break;
		case PIECE_RED:  iter = this->_redCards.begin();  end = this->_redCards.end();  break;
		default: return;
	}
	
	while (iter != end)
	{
		for (int row = 0; row < 3; row++)
		{
			for (int col = 0; col < 3; col++)
			{
				// Create the move.
				Move *move = this->_moveMap[row][col][(*iter)];
				
				if (this->isValidMove(move) == true)
				{
					moveList.push_back(move);
				}
			}
		}
		
		iter++;
	}
}

Move* GameBoard::getMove(int row, int col, int index)
{
	std::list<Card*>::iterator iter = (this->_currentPiece == PIECE_BLUE) ? this->_blueCards.begin() : this->_redCards.begin();
	std::list<Card*>::iterator end = (this->_currentPiece == PIECE_BLUE) ? this->_blueCards.end() : this->_redCards.end();
	int i = 0;
	Card *card = NULL;
	while (iter != end)
	{
		if (index == i)
		{
			card = (*iter);
		}
		i++;
		iter++;
	}
	
	Move* move = this->_moveMap[row][col][card];
	if (this->isValidMove(move))
	{
		return move;
	}
	else
	{
		return NULL;
	}
}

bool GameBoard::gameComplete()
{
	// If neither player has available moves, the game is over.
	std::list<Move*> moveList;
	this->getMoveList(moveList, PIECE_BLUE);
	bool blueMoves = ((int)moveList.size() > 0) ? true : false;
	this->getMoveList(moveList, PIECE_RED);
	bool redMoves = ((int)moveList.size() > 0) ? true : false;
	
	
	
	return (blueMoves == false && redMoves == false);
}

int GameBoard::getNumPieces(Piece piece)
{
	int number = 0;
	
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		if (this->_gameBoard[i].owner == piece)
		{
			number++;
		}
	}
	
	if (piece == PIECE_BLUE)
	{
		number += this->_blueCards.size();
	}
	else
	{
		number += this->_redCards.size();
	}
	

	return number;
}

void GameBoard::printBoard()
{
	for (int row = 0; row < 3; row++)
	{
		for (int col = 0; col < 3; col++)
		{
			if (this->_gameBoard[ROWCOL_TO_SQUARE(row, col)].owner == PIECE_BLUE)
			{
				std::cout << "B";
			}
			else if (this->_gameBoard[ROWCOL_TO_SQUARE(row, col)].owner == PIECE_RED)
			{
				std::cout << "R";
			}
			else
			{
				std::cout << "X";
			}
		}
		std::cout << std::endl;
	}

	std::list<Card*>::iterator iter = this->_redCards.begin();
	while (iter != this->_redCards.end())
	{
		std::cout << "Red Card: " << (*(*iter)) << std::endl;
		iter++;
	}
	iter = this->_blueCards.begin();
	while (iter != this->_blueCards.end())
	{
		std::cout << "Blue Card: " << (*(*iter)) << std::endl;
		iter++;
	}
	
	/*
	for (int row = 0; row < 8; row++)
	{
		std::cout << "+-+-+-+-+-+-+-+-+" << std::endl;
		for (int col = 0; col < 8; col++)
		{
			if (this->_gameBoard[ROWCOL_TO_SQUARE(row, col)] == PIECE_LIGHT)
			{
				std::cout << "|*";
			}
			else if (this->_gameBoard[ROWCOL_TO_SQUARE(row, col)] == PIECE_DARK)
			{
				std::cout << "|O";
			}
			else
			{
				std::cout << "| ";
			}
		}
		std::cout << "|" << std::endl;;
	}
	std::cout << "+-+-+-+-+-+-+-+-+" << std::endl;
	*/
}

void GameBoard::drawBoard(SDL_Surface *surface)
{
	// Paint the overall background.
	boxRGBA(surface, 0, 0, 523, 434, 128, 64, 0, 255);
	
	// Paint each player's cards.
	int index = 0;
	std::list<Card*>::iterator iter = this->_redCards.begin();
	while (iter != this->_redCards.end())
	{
		// Draw the card's border and background color.
		boxRGBA(surface, 10, index * 85 + 10, 99, index * 85 + 84, 0, 0, 0, 255);
		boxRGBA(surface, 11, index * 85 + 11, 98, index * 85 + 83, 128, 0, 0, 255);
		
		// Draw the actual card.
		(*iter)->draw(surface, 11, index * 85 + 11);
		
		// Increment the indices.
		index++;
		iter++;
	}
	
	index = 0;
	iter = this->_blueCards.begin();
	while (iter != this->_blueCards.end())
	{
		// Draw the card's border and background color.
		boxRGBA(surface, 414 + 10, index * 85 + 10, 414 + 99, index * 85 + 84, 0, 0, 0, 255);
		boxRGBA(surface, 414 + 11, index * 85 + 11, 414 + 98, index * 85 + 83, 0, 0, 128, 255);
		
		// Draw the actual card.
		(*iter)->draw(surface, 414 + 11, index * 85 + 11);
		
		// Increment the indices.
		index++;
		iter++;
	}

	// Determine the base offsets for the game board.
	int colOffset = 110;
	int rowOffset = 121;
	
	// Paint the game board's underlying border.
	boxRGBA(surface, colOffset, rowOffset, colOffset + 303, rowOffset + 303, 0, 0, 0, 255);
	
	// Paint the game board itself.
	for (int row = 0; row < 3; row++)
	{
		for (int col = 0; col < 3; col++)
		{
			// Calculate offsets.
			int colOffset2 = colOffset + (col + 1) + col * 100;
			int rowOffset2 = rowOffset + (row + 1) + row * 100;
			
			// Draw the underlying square.
			if (this->_lastMove != NULL && this->_lastMove->row == row && this->_lastMove->col == col)
			{
				boxRGBA(surface, colOffset2, rowOffset2, colOffset2 + 99, rowOffset2 + 99, 128, 128, 128, 255);
			}
			else
			{
				boxRGBA(surface, colOffset2, rowOffset2, colOffset2 + 99, rowOffset2 + 99, 64, 32, 0, 255);
			}
			
			// Draw the square's element.
			switch (this->_gameBoard[ROWCOL_TO_SQUARE(row, col)].element)
			{
				case ELEMENT_NONE: break;
				case ELEMENT_FIRE: stringRGBA(surface, colOffset2 + 5, rowOffset2 + 6, "Fire", 255, 255, 255, 255); break;
				default: break;
			}
			
			// Draw the card's color, if there is a card.
			switch (this->_gameBoard[ROWCOL_TO_SQUARE(row, col)].owner)
			{
				case PIECE_BLUE:
					boxRGBA(surface, colOffset2 + 5, rowOffset2 + 20, colOffset2 + 5 + 89, rowOffset2 + 20 + 74, 0, 0, 0, 255);
					boxRGBA(surface, colOffset2 + 6, rowOffset2 + 21, colOffset2 + 6 + 87, rowOffset2 + 21 + 72, 0, 0, 128, 255);
					this->_gameBoard[ROWCOL_TO_SQUARE(row, col)].getCard()->draw(surface, colOffset2 + 6, rowOffset2 + 21);
					break;
					
				case PIECE_RED:
					boxRGBA(surface, colOffset2 + 5, rowOffset2 + 20, colOffset2 + 5 + 89, rowOffset2 + 20 + 74, 0, 0, 0, 255);
					boxRGBA(surface, colOffset2 + 6, rowOffset2 + 21, colOffset2 + 6 + 87, rowOffset2 + 21 + 72, 128, 0, 0, 255);
					this->_gameBoard[ROWCOL_TO_SQUARE(row, col)].getCard()->draw(surface, colOffset2 + 6, rowOffset2 + 21);
					break;
					
				default:
					break;
			}
			
			// Draw the elemental bonus, if applicable.
			switch (this->_gameBoard[ROWCOL_TO_SQUARE(row, col)].elementalBonus)
			{
				case 1:  stringRGBA(surface, colOffset2 + 40, rowOffset2 + 75, "+1", 255, 255, 255, 255); break;
				case -1: stringRGBA(surface, colOffset2 + 40, rowOffset2 + 75, "-1", 255, 255, 255, 255); break;
				default: break;
			}
		}
	}
}

Square* const GameBoard::getBoard()
{
	return this->_gameBoard;
}

int GameBoard::getEmptySquares()
{
	int number = 0;
	
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		if (this->_gameBoard[i].owner == PIECE_NONE)
		{
			number++;
		}
	}
	
	return number;
}

unsigned long long GameBoard::rand64()
{
	return rand() ^ ((unsigned long long)rand() << 15) ^ ((unsigned long long)rand() << 30) ^ ((unsigned long long)rand() << 45) ^ ((unsigned long long)rand() << 60);
}
