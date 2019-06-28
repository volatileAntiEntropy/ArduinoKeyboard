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

enum ReadSequence:bool {
	ByRows = false,
	ByColums
};

template<byte R, byte C>
class KEYPAD {
private:
	unsigned long refreshInterval;
	unsigned long currentTime;
	Key rawKeyStatus[R][C];
	ReadSequence sequence;

public:
	const byte(&rowPins)[R];
	const byte(&columPins)[C];

	KEYPAD(const byte(&rowpin)[R], const byte(&colpin)[C], unsigned long refresh = 10UL, ReadSequence readSequence = ByRows) :rowPins(rowpin), columPins(colpin) {
		this->refreshInterval = refresh;
		this->currentTime = 0UL;
		this->sequence = readSequence;
		for (byte j = 0U; j < R; j++) {
			for (byte i = 0U; i < C; i++) {
				this->rawKeyStatus[j][i].pressed = false;
				this->rawKeyStatus[j][i].changed = false;
			}
		}
	}

	void setSequence(ReadSequence seq) {
		for (byte j = 0U; j < this->getReaderSize(); j++) {
			pinMode(this->getReaderPinAt(j), INPUT);
		}
		for (byte j = 0U; j < this->getOutputSize(); j++) {
			digitalWrite(this->getOutputPinAt(j), LOW);
			pinMode(this->getOutputPinAt(j), INPUT);
		}
		for (byte j = 0U; j < R; j++) {
			for (byte i = 0U; i < C; i++) {
				this->rawKeyStatus[j][i].pressed = false;
				this->rawKeyStatus[j][i].changed = false;
			}
		}
		this->sequence = seq;
		this->init();
	}

	ReadSequence getReadSequence() const {
		return this->sequence;
	}

	const byte getReaderPinAt(byte index) const {
		return (this->sequence) ? (this->columPins[index]) : (this->rowPins[index]);
	}

	const byte getOutputPinAt(byte index) const {
		return (!this->sequence) ? (this->columPins[index]) : (this->rowPins[index]);
	}

	void init() {
		for (byte j = 0U; j < this->getOutputSize(); j++) {
			pinMode(this->getOutputPinAt(j), OUTPUT);
			digitalWrite(this->getOutputPinAt(j), HIGH);
		}
		for (byte j = 0U; j < this->getReaderSize(); j++) {
			pinMode(this->getReaderPinAt(j), INPUT_PULLUP);
		}
		for (byte j = 0U; j < this->getOutputSize(); j++) {
			digitalWrite(this->getOutputPinAt(j), LOW);
			for (byte i = 0U; i < this->getReaderSize(); i++) {
				bool currentStatus = !digitalRead(this->getReaderPinAt(i));
				if (currentStatus != this->rawKeyStatus[(this->sequence) ? j : i][(this->sequence) ? i : j].pressed) {
					this->rawKeyStatus[(this->sequence) ? j : i][(this->sequence) ? i : j].pressed = currentStatus;
					this->rawKeyStatus[(this->sequence) ? j : i][(this->sequence) ? i : j].changed = true;
				}
				else {
					this->rawKeyStatus[(this->sequence) ? j : i][(this->sequence) ? i : j].changed = false;
				}
			}
			digitalWrite(this->getOutputPinAt(j), HIGH);
		}
		this->currentTime = millis();
	}

	void scanKeys() {
		if (millis() - (this->currentTime) < (this->refreshInterval)) {
			return;
		}
		for (byte j = 0U; j < this->getOutputSize(); j++) {
			digitalWrite(this->getOutputPinAt(j), LOW);
			for (byte i = 0U; i < this->getReaderSize(); i++) {
				bool currentStatus = !digitalRead(this->getReaderPinAt(i));
				if (currentStatus != this->rawKeyStatus[(this->sequence) ? j : i][(this->sequence) ? i : j].pressed) {
					this->rawKeyStatus[(this->sequence) ? j : i][(this->sequence) ? i : j].pressed = currentStatus;
					this->rawKeyStatus[(this->sequence) ? j : i][(this->sequence) ? i : j].changed = true;
				}
				else {
					this->rawKeyStatus[(this->sequence) ? j : i][(this->sequence) ? i : j].changed = false;
				}
			}
			digitalWrite(this->getOutputPinAt(j), HIGH);
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

	byte getReaderSize() const {
		return (this->sequence) ? (C) : (R);
	}

	constexpr byte rowSize() const {
		return R;
	}

	byte getOutputSize() const {
		return (!this->sequence) ? (C) : (R);
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
		for (byte j = 0U; j < this->getReaderSize(); j++) {
			pinMode(this->getReaderPinAt(j), INPUT);
		}
		for (byte j = 0U; j < this->getOutputSize(); j++) {
			digitalWrite(this->getOutputPinAt(j), LOW);
			pinMode(this->getOutputPinAt(j), INPUT);
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