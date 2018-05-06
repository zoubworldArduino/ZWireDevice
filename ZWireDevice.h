/*
 ZwireDevice



 */

#ifndef ZwireDevice_h
#define ZwireDevice_h

#include "Arduino.h"
#include "Wire.h"

/*
 #ifndef ARDUINO_ARCH_AVR
 #define SERIALDEBUG 1
 #endif*/

class ZWireDevice {

public:

	ZWireDevice();
	void requestEvent();
	void receiveEvent(int iData);

	/**
	 * Initializes the ZMCP23017 given its HW selected address, see datasheet for Address selection.
	 or use setHardAddress to over write it.
	 */
	void begin(TwoWire &My_i2c, uint8_t addr, uint8_t * ptr);
	void begin(TwoWire &My_i2c, uint8_t addr);
	/**
	 * Initializes the ZMCP23017 given its HW selected address, see datasheet for Address selection.
	 */
	void begin(uint8_t addr);

	/**
	 * Initializes the default ZMCP23017, with 000 for the configurable part of the address
	 */
	void begin(void);

	void setSerialDebug(HardwareSerial & SerialDebug);

	void setWire(TwoWire &MyWire);
	void setMem(uint8_t * ptr);
	void loop();
	/** callback for read */
	void SetonRead(void (*function)(int));
	/** callback for write */
	void SetonWrite(void (*function)(int));
	
	
	
	

	int get(int device_addr,int p_offset, int p_len);
	int set(int device_addr,int p_offset, int p_len, uint8_t data[]) ;

private:
	// call back function to do a process on read at int address from local memory

	void (*onRead)(int);
// call back function to do a process on read at int address from local memory
	void (*onWrite)(int);
	uint8_t _i2caddr;
	TwoWire *_i2c;
	uint8_t * SlaveDevice_pmem;
	unsigned int SlaveDevice_prQ = 0;
	HardwareSerial * SerialDebug;
};

#endif
