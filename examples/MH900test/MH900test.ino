/*************************************************** 
  This is a library for our makerandhacker.com MH900 GSM-GPRS Shield

  Designed specifically to work with the makerandhacker.com MH900 GSM-GPRS Shield
 
     ****************************************************/

#include <SoftwareSerial.h>
#include "AndTech_MH900.h"

#define MH900_RX 2
#define MH900_TX 3
#define MH900_RST 4

// this is a large buffer for replies
char replybuffer[255];

SoftwareSerial mh900SS = SoftwareSerial(MH900_TX, MH900_RX);
AndTech_MH900 mh900 = AndTech_MH900(&mh900SS, MH900_RST);

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);

void setup() {
  Serial.begin(115200);
  Serial.println(F("MH900 basic test"));
  Serial.println(F("Initializing....(May take 3 seconds)"));

  
  // See if the MH900 is responding
  if (! mh900.begin(4800)) {  // make it slow so its easy to read!
    Serial.println(F("Couldn't find MH900"));
    while (1);
  }
  Serial.println(F("MH900 is OK"));

  printMenu();
}

void printMenu(void) {
   Serial.println(F("-------------------------------------"));
   Serial.println(F("[?] Print this menu"));
   Serial.println(F("[a] read the ADC (2.8V max)"));
   Serial.println(F("[b] read the Battery V"));
   Serial.println(F("[C] read the SIM CCID"));
   Serial.println(F("[U] Unlock SIM with PIN code"));
   Serial.println(F("[i] read RSSI"));
   Serial.println(F("[n] get Network status"));
   Serial.println(F("[v] set audio Volume"));
   Serial.println(F("[V] get Volume"));
   Serial.println(F("[H] set Headphone audio"));
   Serial.println(F("[e] set External audio"));
   Serial.println(F("[T] play audio Tone"));
   Serial.println(F("[f] tune FM radio"));
   Serial.println(F("[F] turn off FM"));
   Serial.println(F("[m] set FM volume"));
   Serial.println(F("[M] get FM volume"));
   Serial.println(F("[q] get FM station signal level"));
   Serial.println(F("[P] PWM/Buzzer out"));
   Serial.println(F("[c] make phone Call"));
   Serial.println(F("[h] Hang up phone"));
   Serial.println(F("[N] Number of SMSs"));
   Serial.println(F("[r] Read SMS #"));
   Serial.println(F("[R] Read All SMS"));
   Serial.println(F("[d] Delete SMS #"));
   Serial.println(F("[s] Send SMS"));
   Serial.println(F("[G] Enable GPRS"));
   Serial.println(F("[g] Disable GPRS"));
   Serial.println(F("[l] Query GSMLOC (GPRS)"));
   Serial.println(F("[w] Read webpage (GPRS)"));
   Serial.println(F("[S] create Serial passthru tunnel"));
   Serial.println(F("-------------------------------------"));
   Serial.println(F(""));
  
}
void loop() {
  Serial.print(F("MH900> "));
  while (! Serial.available() );
  
  char command = Serial.read();
  Serial.println(command);
  
  
  switch (command) {
    case '?': {
      printMenu();
      break;
    }
    
    case 'a': {
      // read the ADC
      uint16_t adc;
      if (! mh900.getADCVoltage(&adc)) {
        Serial.println(F("Failed to read ADC"));
      } else {
        Serial.print(F("ADC = ")); Serial.print(adc); Serial.println(F(" mV"));
      }
      break;
    }
    
    case 'b': {
        // read the battery voltage
        uint16_t vbat;
        if (! mh900.getBattVoltage(&vbat)) {
          Serial.println(F("Failed to read Batt"));
        } else {
          Serial.print(F("VBat = ")); Serial.print(vbat); Serial.println(F(" mV"));
        }
        break;
    }

    case 'U': {
        // Unlock the SIM with a PIN code
        char PIN[5];
        flushSerial();
        Serial.println(F("Enter 4-digit PIN"));
        readline(PIN, 3);
        Serial.println(PIN);
        Serial.print(F("Unlocking SIM card: "));
        if (! mh900.unlockSIM(PIN)) {
          Serial.println(F("Failed"));
        } else {
          Serial.println(F("OK!"));
        }        
        break;
    }

    case 'C': {
        // read the CCID
        mh900.getSIMCCID(replybuffer);  // make sure replybuffer is at least 21 bytes!
        Serial.print(F("SIM CCID = ")); Serial.println(replybuffer);
        break;
    }

    case 'i': {
        // read the RSSI
        uint8_t n = mh900.getRSSI();
        int8_t r;
        
        Serial.print(F("RSSI = ")); Serial.print(n); Serial.print(": ");
        if (n == 0) r = -115;
        if (n == 1) r = -111;
        if (n == 31) r = -52;
        if ((n >= 2) && (n <= 30)) {
          r = map(n, 2, 30, -110, -54);
        }
        Serial.print(r); Serial.println(F(" dBm"));
       
        break;
    }
    
    case 'n': {
        // read the network/cellular status
        uint8_t n = mh900.getNetworkStatus();
        Serial.print(F("Network status ")); 
        Serial.print(n);
        Serial.print(F(": "));
        if (n == 0) Serial.println(F("Not registered"));
        if (n == 1) Serial.println(F("Registered (home)"));
        if (n == 2) Serial.println(F("Not registered (searching)"));
        if (n == 3) Serial.println(F("Denied"));
        if (n == 4) Serial.println(F("Unknown"));
        if (n == 5) Serial.println(F("Registered roaming"));
        break;
    }
    
    /*** Audio ***/
    case 'v': {
      // set volume
      flushSerial();
      Serial.print(F("Set Vol %"));
      uint8_t vol = readnumber();
      Serial.println();
      if (! mh900.setVolume(vol)) {
        Serial.println(F("Failed"));
      } else {
        Serial.println(F("OK!"));
      }
      break;
    }

    case 'V': {
      uint8_t v = mh900.getVolume();
      Serial.print(v); Serial.println("%");
    
      break; 
    }
    
    case 'H': {
      // Set Headphone output
      if (! mh900.setAudio(MH900_HEADSETAUDIO)) {
        Serial.println(F("Failed"));
      } else {
        Serial.println(F("OK!"));
      }
      mh900.setMicVolume(MH900_HEADSETAUDIO, 15);
      break;
    }
    case 'e': {
      // Set External output
      if (! mh900.setAudio(MH900_EXTAUDIO)) {
        Serial.println(F("Failed"));
      } else {
        Serial.println(F("OK!"));
      }

      mh900.setMicVolume(MH900_EXTAUDIO, 10);
      break;
    }

    case 'T': {
      // play tone
      flushSerial();
      Serial.print(F("Play tone #"));
      uint8_t kittone = readnumber();
      Serial.println();
      // play for 1 second (1000 ms)
      if (! mh900.playToolkitTone(kittone, 1000)) {
        Serial.println(F("Failed"));
      } else {
        Serial.println(F("OK!"));
      }
      break;
    }
    
    /*** FM Radio ***/
    
    case 'f': {
      // get freq
      flushSerial();
      Serial.print(F("FM Freq (eg 1011 == 101.1 MHz): "));
      uint16_t station = readnumber();
      Serial.println();
      // FM radio ON using headset
      if (mh900.FMradio(true, MH900_HEADSETAUDIO)) {
        Serial.println(F("Opened"));
      }
     if (! mh900.tuneFMradio(station)) {
        Serial.println(F("Failed"));
      } else {
        Serial.println(F("Tuned"));
      }
      break;
    }
    case 'F': {
      // FM radio off
      if (! mh900.FMradio(false)) {
        Serial.println(F("Failed"));
      } else {
        Serial.println(F("OK!"));
      }
      break;
    }
    case 'm': {
      // Set FM volume.
      flushSerial();
      Serial.print(F("Set FM Vol [0-6]:"));
      uint8_t vol = readnumber();
      Serial.println();
      if (!mh900.setFMVolume(vol)) {
        Serial.println(F("Failed"));
      } else {
        Serial.println(F("OK!"));
      }
      break;
    }
    case 'M': {
      // Get FM volume.
      uint8_t fmvol = mh900.getFMVolume();
      if (fmvol < 0) {
        Serial.println(F("Failed"));
      } else {
        Serial.print(F("FM volume: "));
        Serial.println(fmvol, DEC);
      }
      break;
    }
    case 'q': {
      // Get FM station signal level (in decibels).
      flushSerial();
      Serial.print(F("FM Freq (eg 1011 == 101.1 MHz): "));
      uint16_t station = readnumber();
      Serial.println();
      int8_t level = mh900.getFMSignalLevel(station);
      if (level < 0) {
        Serial.println(F("Failed! Make sure FM radio is on (tuned to station)."));
      } else {
        Serial.print(F("Signal level (dB): "));
        Serial.println(level, DEC);
      }
      break;
    }
    
    /*** PWM ***/
    
    case 'P': {
      // PWM Buzzer output @ 2KHz max
      flushSerial();
      Serial.print(F("PWM Freq, 0 = Off, (1-2000): "));
      uint16_t freq= readnumber();
      Serial.println();
      if (! mh900.PWM(freq)) {
        Serial.println(F("Failed"));
      } else {
        Serial.println(F("OK!"));
      }
      break;
    }

    /*** Call ***/
    case 'c': {      
      // call a phone!
      char number[30];
      flushSerial();
      Serial.print(F("Call #"));
      readline(number, 30);
      Serial.println();
      Serial.print(F("Calling ")); Serial.println(number);
      if (!mh900.callPhone(number)) {
        Serial.println(F("Failed"));
      } else {
        Serial.println(F("Sent!"));
      }
      
      break;
    }
    case 'h': {
       // hang up! 
      if (! mh900.hangUp()) {
        Serial.println(F("Failed"));
      } else {
        Serial.println(F("OK!"));
      }
      break;     
    }
    
    /*** SMS ***/
    
    case 'N': {
        // read the number of SMS's!
        int8_t smsnum = mh900.getNumSMS();
        if (smsnum < 0) {
          Serial.println(F("Could not read # SMS"));
        } else {
          Serial.print(smsnum); 
          Serial.println(F(" SMS's on SIM card!"));
        }
        break;
    }
    case 'r': {
      // read an SMS
      flushSerial();
      Serial.print(F("Read #"));
      uint8_t smsn = readnumber();
      
      Serial.print(F("\n\rReading SMS #")); Serial.println(smsn);
      uint16_t smslen;
      if (! mh900.readSMS(smsn, replybuffer, 250, &smslen)) { // pass in buffer and max len!
        Serial.println("Failed!");
        break;
      }
      Serial.print(F("***** SMS #")); Serial.print(smsn); 
      Serial.print(" ("); Serial.print(smslen); Serial.println(F(") bytes *****"));
      Serial.println(replybuffer);
      Serial.println(F("*****"));
      
      break;
    }
    case 'R': {
      // read all SMS
      int8_t smsnum = mh900.getNumSMS();
      uint16_t smslen;
      for (int8_t smsn=1; smsn<=smsnum; smsn++) {
        Serial.print(F("\n\rReading SMS #")); Serial.println(smsn);
        if (!mh900.readSMS(smsn, replybuffer, 250, &smslen)) {  // pass in buffer and max len!
           Serial.println(F("Failed!"));
           break;
        }
        // if the length is zero, its a special case where the index number is higher
        // so increase the max we'll look at!
        if (smslen == 0) {
          Serial.println(F("[empty slot]"));
          smsnum++;
          continue;
        }
        
        Serial.print(F("***** SMS #")); Serial.print(smsn); 
        Serial.print(" ("); Serial.print(smslen); Serial.println(F(") bytes *****"));
        Serial.println(replybuffer);
        Serial.println(F("*****"));
      }
      break;
    }

    case 'd': {
      // delete an SMS
      flushSerial();
      Serial.print(F("Delete #"));
      uint8_t smsn = readnumber();
      
      Serial.print(F("\n\rDeleting SMS #")); Serial.println(smsn);
      if (mh900.deleteSMS(smsn)) {
        Serial.println(F("OK!"));
      } else {
        Serial.println(F("Couldn't delete"));
      }
      break;
    }
    
    case 's': {
      // send an SMS!
      char sendto[21], message[141];
      flushSerial();
      Serial.print(F("Send to #"));
      readline(sendto, 20);
      Serial.println(sendto);
      Serial.print(F("Type out one-line message (140 char): "));
      readline(message, 140);
      Serial.println(message);
      if (!mh900.sendSMS(sendto, message)) {
        Serial.println(F("Failed"));
      } else {
        Serial.println(F("Sent!"));
      }
      
      break;
    }
    /*********************************** GPRS */
    
    case 'g': {
       // turn GPRS off
       if (!mh900.enableGPRS(false))  
         Serial.println(F("Failed to turn off"));
       break;
    }
    case 'G': {
       // turn GPRS on
       if (!mh900.enableGPRS(true))  
         Serial.println(F("Failed to turn on"));
       break;
    }
    case 'l': {
       // check for GSMLOC (requires GPRS)
       uint16_t returncode;
       
       if (!mh900.getGSMLoc(&returncode, replybuffer, 250))
         Serial.println(F("Failed!"));
       if (returncode == 0) {
         Serial.println(replybuffer);
       } else {
         Serial.print(F("Fail code #")); Serial.println(returncode);
       }
       
       break;
    }
    case 'w': {
      // read website URL
      uint16_t statuscode;
      int16_t length;
      char url[80];
      
      flushSerial();
      Serial.println(F("NOTE: in beta! Use small webpages to read!"));
      Serial.println(F("URL to read (e.g. www.adafruit.com/testwifi/index.html):"));
      Serial.print(F("http://")); readline(url, 79);
      Serial.println(url);
      
       Serial.println(F("****"));
       if (!mh900.HTTP_GET_start(url, &statuscode, (uint16_t *)&length)) {
         Serial.println("Failed!");
         break;
       }
       while (length > 0) {
         while (mh900.available()) {
           char c = mh900.read();
           
           // Serial.write is too slow, we'll write directly to Serial register!
           loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
           UDR0 = c;
           
           length--;
           if (! length) break;
         }
       }
       Serial.println(F("\n****"));
       mh900.HTTP_GET_end();
       break;
    }
    
    
    /*****************************************/
      
    case 'S': {
      Serial.println(F("Creating SERIAL TUBE"));
      while (1) {
        while (Serial.available()) {
          mh900.write(Serial.read());
        }
        if (mh900.available()) {
          Serial.write(mh900.read());
        }
      }
      break;
    }
    
    default: {
      Serial.println(F("Unknown command"));
      printMenu();
      break;
    }
  }
  // flush input
  flushSerial();
  while (mh900.available()) {
    Serial.write(mh900.read());
  }

}

void flushSerial() {
    while (Serial.available()) 
    Serial.read();
}

char readBlocking() {
  while (!Serial.available());
  return Serial.read();
}
uint16_t readnumber() {
  uint16_t x = 0;
  char c;
  while (! isdigit(c = readBlocking())) {
    //Serial.print(c);
  }
  Serial.print(c);
  x = c - '0';
  while (isdigit(c = readBlocking())) {
    Serial.print(c);
    x *= 10;
    x += c - '0';
  }
  return x;
}
  
uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout) {
  uint16_t buffidx = 0;
  boolean timeoutvalid = true;
  if (timeout == 0) timeoutvalid = false;
  
  while (true) {
    if (buffidx > maxbuff) {
      //Serial.println(F("SPACE"));
      break;
    }

    while(Serial.available()) {
      char c =  Serial.read();

      //Serial.print(c, HEX); Serial.print("#"); Serial.println(c);

      if (c == '\r') continue;
      if (c == 0xA) {
        if (buffidx == 0)   // the first 0x0A is ignored
          continue;
        
        timeout = 0;         // the second 0x0A is the end of the line
        timeoutvalid = true;
        break;
      }
      buff[buffidx] = c;
      buffidx++;
    }
    
    if (timeoutvalid && timeout == 0) {
      //Serial.println(F("TIMEOUT"));
      break;
    }
    delay(1);
  }
  buff[buffidx] = 0;  // null term
  return buffidx;
}