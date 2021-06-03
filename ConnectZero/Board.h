#pragma once
#include <iostream>
#include <assert.h>

#define U64 unsigned long long

#define ROW(row, bb) (0x7FULL & ((bb) >> ((row) * 8)))

#define RED_WON 1
#define BLUE_WON 2
#define DRAW 3

class Board
{
public:

	U64 red = 0, blue = 0;
	bool redsTurn = true;

	Board() {}

	Board(bool redsTurn)
	{
		this->redsTurn = redsTurn;
	}

	Board(U64 red, U64 blue, bool redsTurn)
	{
		this->red = red;
		this->blue = blue;
		this->redsTurn = redsTurn;
	}

	char PossibleMoves() const
	{
		U64 pieces = red | blue;
		char lastRow = ROW(5, pieces);
		return (lastRow ^ 0x7f); // flip
	}

	Board MoveCopy(int file) const
	{
		assert(file >= 0 && file < 8 && "File not in range");

		Board board = (*this); // copy
		U64 pieces = red | blue;

		for (int i = 0; i < 6; i++)
		{
			U64 bit = 1ULL << (file + i * 8);

			if (!(pieces & bit))
			{
				if (redsTurn)
				{
					board.red |= bit;
				}
				else
				{
					board.blue |= bit;
				}

				board.redsTurn ^= 1;

				return board;
			}
		}

		assert(!"MoveCopy() was called but piece could not be placed");
	}

	void Move(int file)
	{
		assert(file >= 0 && file < 8 && "File not in range");

		U64 pieces = red | blue;

		for (int i = 0; i < 6; i++)
		{
			U64 bit = 1ULL << (file + i * 8);

			if (!(pieces & bit))
			{
				if (redsTurn)
				{
					red |= bit;
				}
				else
				{
					blue |= bit;
				}

				redsTurn ^= 1;

				return;
			}
		}

		assert(!"MoveCopy() was called but piece could not be placed");
	}

	int GetStatus() const
	{
		if (redsTurn)
		{
			if (HasConnect4(blue))
			{
				return BLUE_WON;
			}
		}
		else
		{
			if (HasConnect4(red))
			{
				return RED_WON;
			}
		}

		if (PossibleMoves())
		{
			return 0; // moves possible
		}
		else
		{
			return DRAW;
		}

		/*if (HasConnect4(red))
		{
			return RED_WON;
		}
		else if (HasConnect4(blue))
		{
			return BLUE_WON;
		}
		else if (PossibleMoves())
		{
			return 0;
		}
		else
		{
			return DRAW;
		}*/
	}

	bool HasConnect4(U64 bb) const
	{
		assert(!(bb & 0xFFFF808080808080));

		// red
		U64 horizontal = bb;
		U64 vertical = bb;
		U64 diagonal = bb;
		U64 antiDiagonal = bb;

		for (int i = 0; i < 3; i++)
		{
			horizontal &= (horizontal >> 1);
			vertical &= (vertical >> 8);
			diagonal &= (diagonal >> 9);
			antiDiagonal &= (antiDiagonal >> 7);
		}

		return horizontal | vertical | diagonal | antiDiagonal;
	}

	void Print() const
	{

		std::cout << std::endl;

		for (int i = 5; i >= 0; i--)
		{
			char rowRed = ROW(i, red);
			char rowBlue = ROW(i, blue);

			std::cout << " | ";

			for (int j = 0; j < 7; j++)
			{
				if (rowRed & (1 << j))
				{
					std::cout << "O" << " | ";
				}
				else if (rowBlue & (1 << j))
				{
					std::cout << "X" << " | ";
				}
				else
				{
					std::cout << "  | ";
				}
			}

			std::cout << std::endl;

			std::cout << " |---|---|---|---|---|---|---|\n";
		}

		if (redsTurn)
		{
			std::cout << "\nRed to play\n";
		}
		else
		{
			std::cout << "Blue to play\n";
		}
	}
};
