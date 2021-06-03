#pragma once
#include "Board.h"
#include <stdlib.h> 

void stopComputer();

int findBestMove(Board board);

void findBestMoveCallback(Board board, void (*callback)(int));