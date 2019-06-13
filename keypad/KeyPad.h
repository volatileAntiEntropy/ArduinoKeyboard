#pragma once

#if defined(ARDUINO) && (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

namespace utils {
	template<class T1, class T2>
	class Pair {
	public:
		T1 first;
		T2 second;
		Pair() = default;
		Pair(T1 f, T2 s) {
			first = f;
			second = s;
		}
	};

	template<class T1, class T2>
	Pair<T1, T2> makePair(T1 val1, T2 val2) {
		Pair<T1, T2> temp(val1, val2);
		return temp;
	}

	template<class T, size_t N>
	size_t getArrayLength(T(&arr)[N]) {
		return N;
	}

	void(*reboot)() = NULL;
}

typedef struct __Key__{
	bool pressed;
	bool changed;

	__Key__() {
		pressed = false;
		changed = false;
	}

	~__Key__() {
		pressed = false;
		changed = false;
	}
}Key;

template<byte R, byte C>
class KEYPAD {
private:
	unsigned long refreshInterval;
	unsigned long currentTime;
	Key rawKeyStatus[R][C];

public:
	const byte(&rowPins)[R];
	const byte(&columPins)[C];

	//KEYPAD() = default;

	KEYPAD(const byte(&rowpin)[R], const byte(&colpin)[C], unsigned long refresh = 10UL):rowPins(rowpin),columPins(colpin) {
		this->refreshInterval = refresh;
		this->currentTime = 0UL;
	}

	void init() {
		for (byte j = 0U; j < R; j++) {
			pinMode(this->rowPins[j], OUTPUT);
		}
		for (byte j = 0U; j < C; j++) {
			pinMode(this->columPins[j], INPUT_PULLUP);
		}
		for (byte j = 0U; j < R; j++) {
			digitalWrite(this->rowPins[j], LOW);
			for (byte i = 0U; i < C; i++) {
				bool currentStatus = !digitalRead(this->columPins[i]);
				if (currentStatus != this->rawKeyStatus[j][i].pressed) {
					this->rawKeyStatus[j][i].pressed = currentStatus;
					this->rawKeyStatus[j][i].changed = true;
				}
				else {
					this->rawKeyStatus[j][i].changed = false;
				}
			}
			digitalWrite(this->rowPins[j], HIGH);
		}
		this->currentTime = millis();
	}

	void scanKeys() {
		if (millis() - (this->currentTime) < (this->refreshInterval)) {
			return;
		}
		for (byte j = 0U; j < R; j++) {
			digitalWrite(this->rowPins[j], LOW);
			for (byte i = 0U; i < C; i++) {
				bool currentStatus = !digitalRead(this->columPins[i]);
				if (currentStatus != this->rawKeyStatus[j][i].pressed) {
					this->rawKeyStatus[j][i].pressed = currentStatus;
					this->rawKeyStatus[j][i].changed = true;
				}
				else {
					this->rawKeyStatus[j][i].changed = false;
				}
			}
			digitalWrite(this->rowPins[j], HIGH);
		}
		this->currentTime = millis();
	}

	Key keyAt(byte rownum, byte colnum) const {
		return this->rawKeyStatus[rownum][colnum];
	}

	Key keyAt(uint16_t keynum) const {
		return this->rawKeyStatus[keynum / C][keynum % C];
	}

	constexpr byte columSize() const {
		return C;
	}

	constexpr byte rowSize() const {
		return R;
	}

	constexpr uint16_t keySize() const {
		return R * C;
	}

	void exportTo(Key(&result)[R][C]) const {
		for (byte i = 0U; i < R; i++) {
			for (byte j = 0U; j < C; j++) {
				result[i][j] = this->rawKeyStatus[i][j];
			}
		}
	}

	void detach() {
		this->currentTime = 0UL;
		this->refreshInterval = 0UL;
		for (byte j = 0U; j < C; j++) {
			pinMode(this->columPins[j], OUTPUT);
		}
		for (byte j = 0U; j < R; j++) {
			digitalWrite(this->rowPins[j], LOW);
		}
		for (byte j = 0U; j < R; j++) {
			for (byte i = 0U; i < C; i++) {
				this->rawKeyStatus[j][i].pressed = false;
				this->rawKeyStatus[j][i].changed = false;
			}
		}
	}

	~KEYPAD() {
		this->detach();
	}

	String loadString(const char(&charSet)[R][C]) const {
		String buffer = "";
		for (byte i = 0U; i < R; i++) {
			for (byte j = 0U; j < C; j++) {
				if (this->rawKeyStatus[i][j].pressed) {
					buffer.concat(charSet[i][j]);
				}
			}
		}
		return buffer;
	}
};