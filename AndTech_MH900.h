/*************************************************** 
  This is a library for our makerandhacker.com MH900 GSM-GPRS Shield

  Designed specifically to work with the makerandhacker.com MH900 GSM-GPRS Shield
 
     ****************************************************/

#if (ARDUINO >= 100)
 #include "Arduino.h"
 #include <SoftwareSerial.h>
#else
 #include "WProgram.h"
 #include <NewSoftSerial.h>
#endif

//#define AndTech_MH900_DEBUG

#define MH900_HEADSETAUDIO 0
#define MH900_EXTAUDIO 1

#define MH900_STTONE_DIALTONE 1
#define MH900_STTONE_BUSY 2
#define MH900_STTONE_CONGESTION 3
#define MH900_STTONE_PATHACK 4 
#define MH900_STTONE_DROPPED 5
#define MH900_STTONE_ERROR 6  
#define MH900_STTONE_CALLWAIT 7 
#define MH900_STTONE_RINGING 8
#define MH900_STTONE_BEEP 16
#define MH900_STTONE_POSTONE 17
#define MH900_STTONE_ERRTONE 18
#define MH900_STTONE_INDIANDIALTONE 19
#define MH900_STTONE_USADIALTONE 20

#define MH900_DEFAULT_TIMEOUT_MS 500

class AndTech_MH900 : public Stream {
 public:
#if ARDUINO >= 100
  AndTech_MH900(SoftwareSerial *, int8_t r);
#else
  AndTech_MH900(NewSoftSerial *, int8_t r);
#endif
  boolean begin(uint16_t baud);

  // Stream
  int available(void);
  size_t write(uint8_t x);
  int read(void);
  int peek(void);
  void flush();

  // Battery and ADC
  boolean getADCVoltage(uint16_t *v);
  boolean getBattVoltage(uint16_t *v);

  // SIM query
  uint8_t unlockSIM(char *pin);
  uint8_t getSIMCCID(char *ccid);
  uint8_t getNetworkStatus(void);
  uint8_t getRSSI(void);

  // set Audio output
  boolean setAudio(uint8_t a);
  boolean setVolume(uint8_t i);
  uint8_t getVolume(void);
  boolean playToolkitTone(uint8_t t, uint16_t len);
  boolean setMicVolume(uint8_t a, uint8_t level);

  // FM radio functions.
  boolean tuneFMradio(uint16_t station);
  boolean FMradio(boolean onoff, uint8_t a = MH900_HEADSETAUDIO);
  boolean setFMVolume(uint8_t i);
  int8_t getFMVolume();
  int8_t getFMSignalLevel(uint16_t station);

  // SMS handling
  int8_t getNumSMS(void);
  boolean readSMS(uint8_t i, char *smsbuff, uint16_t max, uint16_t *readsize);
  boolean sendSMS(char *smsaddr, char *smsmsg);
  boolean deleteSMS(uint8_t i);

  // GPRS handling
  boolean enableGPRS(boolean onoff);
  uint8_t GPRSstate(void);
  boolean getGSMLoc(uint16_t *replycode, char *buff, uint16_t maxlen);

  // HTTP 
  boolean HTTP_GET_start(char *url, uint16_t *status, uint16_t *datalen);
  boolean HTTP_GET_end(void);

  // PWM (buzzer)
  boolean PWM(uint16_t period, uint8_t duty = 50);

  // Phone calls
  boolean callPhone(char *phonenum);
  boolean hangUp(void);

 private: 
  int8_t _rstpin;

  char replybuffer[255];

  void flushInput();
  uint16_t readRaw(uint16_t b);
  uint8_t readline(uint16_t timeout = MH900_DEFAULT_TIMEOUT_MS, boolean multiline = false);
  uint8_t getReply(char *send, uint16_t timeout = MH900_DEFAULT_TIMEOUT_MS);
  uint8_t getReply(const __FlashStringHelper *send, uint16_t timeout = MH900_DEFAULT_TIMEOUT_MS);
  uint8_t getReply(const __FlashStringHelper *prefix, char *suffix, uint16_t timeout = MH900_DEFAULT_TIMEOUT_MS);
  uint8_t getReply(const __FlashStringHelper *prefix, int32_t suffix, uint16_t timeout = MH900_DEFAULT_TIMEOUT_MS);

  uint8_t getReply(const __FlashStringHelper *prefix, int32_t suffix1, int32_t suffix2, uint16_t timeout);

  boolean sendCheckReply(char *send, char *reply, uint16_t timeout = MH900_DEFAULT_TIMEOUT_MS);
  boolean sendCheckReply(const __FlashStringHelper *send, const __FlashStringHelper *reply, uint16_t timeout = MH900_DEFAULT_TIMEOUT_MS);
  boolean sendCheckReply(const __FlashStringHelper *prefix, char *suffix, const __FlashStringHelper *reply, uint16_t timeout = MH900_DEFAULT_TIMEOUT_MS);
  boolean sendCheckReply(const __FlashStringHelper *prefix, int32_t suffix, const __FlashStringHelper *reply, uint16_t timeout = MH900_DEFAULT_TIMEOUT_MS);
  boolean sendCheckReply(const __FlashStringHelper *prefix, int32_t suffix, int32_t suffix2, const __FlashStringHelper *reply, uint16_t timeout = MH900_DEFAULT_TIMEOUT_MS);


  boolean parseReply(const __FlashStringHelper *toreply, 
				  uint16_t *v, char divider  = ',', uint8_t index=0);

  boolean sendParseReply(const __FlashStringHelper *tosend, 
			 const __FlashStringHelper *toreply, 
			 uint16_t *v, char divider = ',', uint8_t index=0);

#if ARDUINO >= 100
  SoftwareSerial *mySerial;
#else
  NewSoftSerial *mySerial;
#endif
};
