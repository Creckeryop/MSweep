#include "minesweeper.h"

minesweeper::minesweeper(int _width, int _height, int _mines) : width(_width), height(_height), mines(_mines), state(0), flags(0) {
	actField = new short*[width];
	numField = new short*[width];
	minField = new bool*[width];
	for (int i = 0; i < width; ++i) {
		actField[i] = new short[height];
		numField[i] = new short[height];
		minField[i] = new bool[height];
	}
	Remake();
}

void minesweeper::Remake() {
	int _mines = mines;
	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < height; ++j) {
			actField[i][j] = 0;
			numField[i][j] = 0;
			minField[i][j] = false;
		}
	}
	while (_mines) {
		int x = rand() % width, y = rand() % height;
		if (!minField[x][y]) {
			minField[x][y] = true;
			for (int k = x - 1; k < x + 2; ++k) {
				for (int l = y - 1; l < y + 2; ++l) {
					if (k >= 0 && l >= 0 && k < width && l < height)
						numField[k][l]++;
				}
			}
			_mines--;
		}
	}
	flags = 0;
	state = 0;
}

void minesweeper::Open(int x, int y) {
	if (state != 0 || x < 0 || y < 0 || x >= width || y >= height)
		return;
	if (actField[x][y] == 1) {
		int count = 0;
		for (int k = x - 1; k < x + 2; ++k)
			for (int l = y - 1; l < y + 2; ++l)
				if (k >= 0 && l >= 0 && k < width && l < height) {
					if (state == 0 && actField[k][l] == 2)
						count++;
				}
		if (count == numField[x][y]) {
			for (int k = x - 1; k < x + 2; ++k)
				for (int l = y - 1; l < y + 2; ++l)
					if (k >= 0 && l >= 0 && k < width && l < height) {
						if (state == 0 && actField[k][l] != 2 && actField[k][l]!= 1)
							Open(k, l);
					}
		}
		return;
	}
	if (actField[x][y] == 2) {
		flags--;
		actField[x][y] = 0;
		return;
	}
	else
		actField[x][y] = 1;
	if (actField[x][y] == 1 && minField[x][y]) {
		state = 1;
		for (int i = 0; i < width; i++)
			for (int j = 0; j < height; j++)
				if (minField[i][j])
					actField[i][j] = 1;
		return;
	}
	if (numField[x][y] == 0) {
		for (int k = x - 1; k < x + 2; ++k)
			for (int l = y - 1; l < y + 2; ++l)
				if (k >= 0 && l >= 0 && k < width && l < height) {
					if (state == 0 && actField[k][l] != 1) {
						if (actField[k][l] == 2) flags--;
						actField[k][l] == 0;
					}
					if (actField[k][l] != 1)
						Open(k, l);
				}
	}
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			if (!minField[i][j] && actField[i][j] != 1)
				return;
	for (int i=0;i< width;i++)
		for (int j=0;j<height;j++)
			if (actField[i][j] != 1 && actField[i][j] != 2) {
				actField[i][j] = 2;
				flags++;
			}
	state = 2;
}

void minesweeper::Flag(int x, int y) {
	if (state != 0 || x < 0 || y < 0 || x >= width || y >= height || actField[x][y] == 1)
		return;
	actField[x][y] = actField[x][y] == 2 ? 0 : 2;
	if (actField[x][y] == 2) flags++;
	else flags--;
}

void minesweeper::Mark(int x, int y) {
	if (state != 0 || x < 0 || y < 0 || x >= width || y >= height || actField[x][y] == 1)
		return;
	actField[x][y] = actField[x][y] == 3 ? 0 : 3;
}
minesweeper::~minesweeper() {
	for (int i = 0; i < width; ++i) {
		delete[] actField[i];
		delete[] numField[i];
		delete[] minField[i];
	}
	delete[] actField;
	delete[] numField;
	delete[] minField;
}