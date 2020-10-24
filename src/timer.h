#pragma once
#include <string>

class timer
{
private:
	bool _isPlaying;
	long long _tick;
public:
	timer();
	const long long GetUTime();
	long long GetTime();
	void Pause();
	void Resume();
	void Reset();
	void Set(int milliseconds);
	bool isPlaying();
};