/*
 * Copyright (c) 2010 Jason Lynch <jason@calindora.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef TRIPLETRIAD_H
#define TRIPLETRIAD_H

#include <memory>

#include "SDL.h"

class GameBoard;

class TripleTriad
{
	public:
		~TripleTriad();

		static std::shared_ptr<TripleTriad> get_instance(const std::string & filename);

		// Method to run the game.
		void run();

		// Method to check SDL events.
		bool checkEvent(bool moveHuman);

	private:
		TripleTriad(const std::string & filename);

		static std::shared_ptr<TripleTriad> _instance;

		// SDL Surface
		SDL_Surface *_surface;

		// Game board.
		GameBoard *_gameBoard;
		
		// Input data
		int _cardChosen;
		std::vector<const Card *> cards;
};

#endif
