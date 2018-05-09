/*
 ZWireDevice

 
 manage 7 bit device adress, and 16 bit data address.

 

 */

#include "Arduino.h"
#include "ZWireDevice.h"
ZWireDevice::ZWireDevice() {
	SerialDebug = 0;
        SlaveDevice_prQ = 0;
}
////////////////////////////////////////////////////////////////////////////////

/**
 * Initializes the ZMCP23017 given its HW selected address, see datasheet for Address selection.
 or use setHardAddress to over write it.

 */
void ZWireDevice::begin(TwoWire &My_i2c, uint8_t addr, uint8_t * ptr) {
	_i2c = &My_i2c;

	_i2caddr = addr;
	//if(_i2c->enabled()) 
	_i2c->begin(_i2caddr);
	SlaveDevice_prQ = 0;
	SlaveDevice_pmem = ptr;
	// _i2c->onReceive(&this->SlaveDevice_receiveEvent);
	//  _i2c->onRequest(&this->SlaveDevice_requestEvent);

}
void ZWireDevice::setWire(TwoWire &MyWire) {
	_i2c = &MyWire;
}
void ZWireDevice::setMem(uint8_t * ptr) {
	SlaveDevice_pmem = ptr;
}
void ZWireDevice::begin(TwoWire &My_i2c, uint8_t addr) {
	begin(My_i2c, addr, (uint8_t *) -1);

}

void ZWireDevice::setSerialDebug(HardwareSerial & mySerialDebug) {
	SerialDebug = &mySerialDebug;
	 if (SerialDebug)
		 SerialDebug->println("SerialDbg(true)");
	/* else
		  SerialDebug->println("SerialDbg(false)");*/
}
#if !defined( BUFFER_LENGTH)
#define BUFFER_LENGTH 32
#endif
void ZWireDevice::requestEvent() {

	if (SerialDebug)
		SerialDebug->println("requestEvent ");
	if (onRead)
		onRead((int) SlaveDevice_prQ);
	for(int i=0;i<BUFFER_LENGTH;i++)
	{
		if (SerialDebug)
		{
	SerialDebug->print("*(");SerialDebug->print((uint16_t)(SlaveDevice_prQ));SerialDebug->print("+");SerialDebug->print((uint32_t)(SlaveDevice_pmem));SerialDebug->print(")= ");
		}
		uint8_t value=(uint8_t) * (SlaveDevice_prQ + SlaveDevice_pmem);
				if (SerialDebug)
		{	SerialDebug->println(value);
	}
	_i2c->write(value); //send mem[prQ]
	
	if (SerialDebug)
{
		SerialDebug->print("SD_prQ= ");
		SerialDebug->println(SlaveDevice_prQ);
		SerialDebug->print("*(SD_prQ+SD_pmem)= ");
		SerialDebug->println(*(SlaveDevice_prQ + SlaveDevice_pmem));
}
SlaveDevice_prQ++;
}
}

void ZWireDevice::SetonRead(void (*function)(int)) {
	onRead = function;
}

void ZWireDevice::SetonWrite(void (*function)(int)) {
	onWrite = function;
}

void onReceive(void (*)(int));
void onRequest(void (*)(void));
/** for futur need*/
void ZWireDevice::loop() 
{
/*
 while (_i2c->available())
 SlaveDevice_receiveEvent(0);*/
}

void ZWireDevice::receiveEvent(int iData) {
	if (SerialDebug)
		SerialDebug->println("recEv");
	SlaveDevice_prQ = _i2c->read();   //update address
	SlaveDevice_prQ = SlaveDevice_prQ << 8 | _i2c->read();   //update address
	if (SerialDebug)
	{
		SerialDebug->print("SD_prQ= ");

		SerialDebug->println(SlaveDevice_prQ);
	}
	while (_i2c->available()) {
		*(SlaveDevice_prQ + SlaveDevice_pmem) = _i2c->read();
		if (SerialDebug)
		{
			SerialDebug->print("*(");SerialDebug->print((uint32_t)(SlaveDevice_prQ+SlaveDevice_pmem));SerialDebug->print(")= ");
	
			SerialDebug->println(*(SlaveDevice_prQ + SlaveDevice_pmem));
		}
		if (onWrite)
			onWrite (SlaveDevice_prQ);
		SlaveDevice_prQ++;
	}   //update mem[prQ]
}



/**
* @brief : Set() allows to read p_len bytes from the p_offset position into arduino's memory map over i2c, and push it into internal buffer.
*
* @return 0 if ok, -1 if not.
*/
int ZWireDevice::get(int device_addr,int p_offset, int p_len) {

if (SerialDebug)
{		
	SerialDebug->print("get data(");         // print the character
	SerialDebug->print(device_addr,HEX);   	SerialDebug->print(",");        
	SerialDebug->print(p_offset,HEX);   	SerialDebug->print(",");        
	SerialDebug->print(p_len,HEX);   	SerialDebug->println(")");        
	
}
if(p_len<1)
	return -1;

	_i2c->beginTransmission(device_addr);
	_i2c->write(p_offset>>8);
	_i2c->write(p_offset);
	int res=_i2c->endTransmission();
	if (SerialDebug)
{	SerialDebug->print("_i2c->endTrans=");    SerialDebug->println(res,HEX);          }
 if(res!=0)
	   return -1;
   res=_i2c->requestFrom(device_addr, p_len);    // request 6 bytes from slave device #8
  
if (SerialDebug)
{	SerialDebug->print("_i2c->req=");    SerialDebug->println(res,HEX);          }
delay(1000);
  while (_i2c->available()) { // slave may send less than requested
    char c = _i2c->read(); // receive a byte as character
	if (SerialDebug)
{	SerialDebug->print("c=");    SerialDebug->println(c,HEX);          }
	(*  (SlaveDevice_pmem + p_offset))=c;
	p_offset++;
 /*   if (SerialDebug)
	{
		SerialDebug->print(c,HEX); 
		SerialDebug->print(",");
	}*/
  }
   if(res!=p_len)
	   return -1;
  return 0;
}
/**
* @brief : Set() allows to write p_len bytes from data[] at the p_offset position into arduino's memory map over i2c.
*
* @return 0 if ok, -1 if not.
*/
int ZWireDevice::set(int device_addr,int p_offset, int p_len, uint8_t data[]) 
{
	if (SerialDebug)
{		
	SerialDebug->print("set data(");         // print the character
	SerialDebug->print(device_addr,HEX);   	SerialDebug->print(",");        
	SerialDebug->print(p_offset,HEX);   	SerialDebug->print(",");        
	SerialDebug->print(p_len,HEX);   	SerialDebug->println(")");        
	
}
    _i2c->beginTransmission(device_addr);
	_i2c->write(p_offset>>8);
	_i2c->write(p_offset);
	for(int i=0;i<p_len;i++)
	_i2c->write(data[i]);
	_i2c->endTransmission();
	
  return 0;
}
