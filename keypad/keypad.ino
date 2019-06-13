/*
 Name:		keypad.ino
 Created:	2019/5/16 19:57:54
 Author:	qinbi
*/
#include "KeyPad.h"

static constexpr byte rowPins[4U] = { 7U,8U,9U,10U };
static constexpr byte columPins[4U] = { 3U,4U,5U,6U };
KEYPAD<4U, 4U> keyPad(rowPins, columPins);

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600U);
	keyPad.init();//init() automatically checks current key status, using scanKeys() in setup is not a wise choice.
}

// the loop function runs over and over again until power down or reset
void loop() {
	keyPad.scanKeys();
	for (byte i = 0U; i < keyPad.rowSize(); i++) {
		for (byte j = 0U; j < keyPad.columSize(); j++) {
			Serial.print(keyPad.keyAt(i, j).pressed);
			Serial.print(",");
		}
		Serial.println();
	}
	Serial.println();
	//reboot();
}
