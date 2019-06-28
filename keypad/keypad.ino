/*
 Name:		keypad.ino
 Created:	2019/5/16 19:57:54
 Author:	qinbi
*/
#include "KeyPad.h"

static constexpr byte rowPins[12] = { 13,12,11,10,9,8,7,6,5,4,3,2 };
static constexpr byte columPins[6] = { A0,A1,A2,A3,A4,A5 };
KEYPAD<6,12> keyPad(columPins, rowPins);

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
