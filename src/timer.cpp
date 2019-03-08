#include "timer.h"

timer::timer() {
	_tick = GetUTime();
	_isPlaying = true;
}

const long long timer::GetUTime() {
	using namespace std::chrono;
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

long long timer::GetTime() {
	return _isPlaying ? GetUTime() - _tick : _tick;
}

void timer::Pause() {
	if (_isPlaying) {
		_isPlaying = false;
		_tick = GetUTime() - _tick;
	}
}

void timer::Resume() {
	if (!_isPlaying) {
		_isPlaying = true;
		_tick = GetUTime() - _tick;
	}
}

void timer::Reset() {
	Set(0);
}

void timer::Set(int milliseconds) {
	_tick = _isPlaying ? GetUTime() - milliseconds : milliseconds;
}

bool timer::isPlaying() {
	return _isPlaying;
}
