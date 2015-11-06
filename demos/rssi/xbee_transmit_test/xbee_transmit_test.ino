/**
 * Copyright (c) 2009 Andrew Rapp. All rights reserved.
 *
 * This file is part of XBee-Arduino.
 *
 * XBee-Arduino is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * XBee-Arduino is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XBee-Arduino.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <XBee.h>
#include <SoftwareSerial.h>

/*
This example is for Series 2 XBee
 Sends a ZB TX request with the value of analogRead(pin5) and checks the status response for success
*/
SoftwareSerial xbeeSerial(2,3);
// create the XBee object
XBee xbee = XBee();

uint8_t payload[] = { 0, 0 };

// SH + SL Address of receiving XBee
XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x0000FFFF);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

int pin5 = 0;

int statusLed = 13;
int errorLed = 13;

void flashLed(int pin, int times, int wait) {

  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(wait);
    digitalWrite(pin, LOW);

    if (i + 1 < times) {
      delay(wait);
    }
  }
}

void setup() {
  pinMode(statusLed, OUTPUT);
  pinMode(errorLed, OUTPUT);

  Serial.begin(9600);
  xbeeSerial.begin(9600);
  xbee.setSerial(xbeeSerial);
}

void sendTx(ZBTxRequest zbTx){
  xbee.send(zbTx);

   if (xbee.readPacket(500)) {
    Serial.println("Got a response!");
    // got a response!

    // should be a znet tx status              
    if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
      xbee.getResponse().getZBTxStatusResponse(txStatus);

      // get the delivery status, the fifth byte
      if (txStatus.getDeliveryStatus() == SUCCESS) {
        // success.  time to celebrate
        Serial.println("SUCCESS!");
        flashLed(statusLed, 5, 50);
      } else {
        Serial.println("FAILURE!");
        // the remote XBee did not receive our packet. is it powered on?
      }
    }
  } else if (xbee.getResponse().isError()) {
    Serial.print("Error reading packet.  Error code: ");  
    Serial.println(xbee.getResponse().getErrorCode());
  } else {
    // local XBee did not provide a timely TX Status Response -- should not happen
    Serial.println("This should never happen...");
  }
}

void loop() {   
  // break down 10-bit reading into two bytes and place in payload
  pin5 = analogRead(5);
  payload[0] = pin5 >> 8 & 0xff;
  payload[1] = pin5 & 0xff;

  sendTx(zbTx);

  delay(1000);
}

