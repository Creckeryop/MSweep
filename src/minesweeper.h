#pragma once

class minesweeper {
public:
	bool **minField;
	short **numField, **actField; // 0 - closed, 1 - opened, 2 - flaged, 3 - mark, 4 - nominehere
	short width, height, mines, flags, state; // 0 - Game, 1 - Lose, 2 - Win
	
	minesweeper(int _width, int _height, int _mines);
	void Remake();
	void Open(int x, int y);
	void Flag(int x, int y);
	void Mark(int x, int y);
	~minesweeper();
};