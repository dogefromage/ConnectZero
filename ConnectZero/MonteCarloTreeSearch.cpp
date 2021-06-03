#include "MonteCarloTreeSearch.h"

struct TreeNode
{
	Board board;

	TreeNode** children;

	bool isLeaf, isEndNode = false;

	float t; // total score
	int n; // number of visits

	TreeNode(const Board& board)
	{
		this->board = board;

		isLeaf = true;
		n = t = 0;

		children = new TreeNode * [7];

		int status = board.GetStatus();
		if (status != 0)
		{
			isEndNode = true;

			if (status == RED_WON)
			{
				t = 1;
			}
			else if (status == BLUE_WON)
			{
				t = -1;
			}
		}
	}

	void Expand()
	{
		assert(isLeaf); // make sure is leaf
		isLeaf = false;

		char possibleMoves = board.PossibleMoves();

		assert(possibleMoves); // this shouldn't be zero because it was checked in constructor

		for (int i = 0; i < 7; i++)
		{
			if (possibleMoves & (1 << i))
			{
				children[i] = new TreeNode(board.MoveCopy(i));
			}
			else
			{
				children[i] = nullptr;
			}
		}
	}
};

float RollOut(const Board& board)
{
	int iterations = 100;

	float sum = 0;

	for (int n = 0; n < iterations; n++)
	{
		Board rollBoard(board); // copy

		int numberMoves = 0;

		while (true)
		{
			char possibleMoves = rollBoard.PossibleMoves();

			assert(possibleMoves);

			int move = 0;
			do
			{
				move = std::rand() % 7;
			} while (!(possibleMoves & (1 << move)));

			rollBoard.Move(move);
			numberMoves++;

			int status = rollBoard.GetStatus();

			if (status != 0)
			{
				if (status == RED_WON)
				{
					sum += 1.0f / numberMoves;
				}
				else if (status == BLUE_WON)
				{
					sum -= 1.0f / numberMoves;
				}

				break;
			}
		}
	}

	return sum / iterations;
}

int monteCarloRecurse(TreeNode* node, bool redMaximizing)
{
	assert(node != nullptr);

	float score = 0;

	if (node->isEndNode)
	{
		score = node->t;
	}
	else if (node->isLeaf)
	{
		if (node->n == 0)
		{
			score = RollOut(node->board);
		}
		else
		{
			node->Expand();

			for (int i = 0; i < 7; i++)
			{
				TreeNode* child = node->children[i];

				if (child != nullptr)
				{
					score = monteCarloRecurse(child, redMaximizing);
					break;
				}
			}
		}
	}
	else
	{
		// not leaf node:
		// search child which maximises ucb and recurse

		float highestUCB = -INFINITY;
		int highestNode = 0;

		for (int i = 0; i < 7; i++)
		{
			TreeNode* child = node->children[i];

			if (child != nullptr)
			{
				float ucb = INFINITY;

				if (child->n > 0)
				{
					float exploitation = child->t / (float)child->n;

					if (!redMaximizing)
					{
						exploitation *= -1.0;
					}

					float exploration = sqrt(log(node->n) / child->n);

					ucb = exploitation + 30 * exploration;
				}

				if (ucb > highestUCB)
				{
					highestUCB = ucb;
					highestNode = i;

					if (ucb == INFINITY)
					{
						break; // not worth looking further
					}
				}
			}
		}

		TreeNode* next = node->children[highestNode];

		score = monteCarloRecurse(next, redMaximizing);
	}

	node->t += score;
	node->n++;

	return score;
}

bool thinking = false;

void stopComputer()
{
	thinking = false;
}

int findBestMove(Board rootBoard)
{
	thinking = true;

	TreeNode root(rootBoard);

	if (root.isEndNode)
	{
		return -1;
	}

	root.Expand();

	while (thinking)
	{
		monteCarloRecurse(&root, rootBoard.redsTurn);
	}

	int bestMove = -1;
	float bestScore = -INFINITY;

	std::cout << "n: score,     iterations.\n";

	for (int i = 0; i < 7; i++)
	{
		TreeNode* child = root.children[i];

		if (child != nullptr)
		{
			if (child->n > 0)
			{
				float childScore = child->t / (float)child->n;

				if (!root.board.redsTurn)
				{
					childScore *= -1;
				}

				if (childScore > bestScore)
				{
					bestScore = childScore;
					bestMove = i;
				}

				std::cout << i << ": " << child->t / (float)child->n << ",   " << child->n << std::endl;
			}
			else
			{
				std::cout << i << ": " << 0 << ",   " << child->n << std::endl;
			}

		}
		else
		{
			std::cout << i << ": -" << std::endl;
		}
	}

	std::cout << "\nTotal iterations: " << root.n << std::endl << std::endl;

	return bestMove;
}

void findBestMoveCallback(Board rootBoard, void (*callback)(int))
{
	int bestMove = findBestMove(rootBoard);

	callback(bestMove);
}