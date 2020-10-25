#ifndef PLEN5STACK_EX_h
#define PLEN5STACK_EX_h
#include <PLEN5Stack.h>

class PLEN5StackEX : public PLEN5Stack{
	public:
		PLEN5StackEX();
		void motionFlame(uint16_t fileName, uint16_t flameNum);
	  void walk(uint8_t mode);
  private:
    int8_t modeNum;
  
};


#endif
