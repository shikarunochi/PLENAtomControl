#include "PLEN5StackEX.h"

#define Walk_Forward 70

PLEN5StackEX::PLEN5StackEX() {
  modeNum = 0;
}

void PLEN5StackEX::walk(uint8_t mode) {
  if (mode == 1) {
    if (modeNum == 0) {
      modeNum = 0;
    } else if (modeNum == 100) {
      modeNum = 0;
    }
  } else {
    if (modeNum == 1) {
      modeNum = 2;
    } else {
      modeNum = 100;
    }
  }
  switch (modeNum) {
    case 0:
      motionFlame(Walk_Forward, 0);
      motionFlame(Walk_Forward, 1);
      modeNum = 1;
    //break;
    case 1:
      motionFlame(Walk_Forward, 2);
      motionFlame(Walk_Forward, 3);
      motionFlame(Walk_Forward, 4);
      motionFlame(Walk_Forward, 5);
      motionFlame(Walk_Forward, 6);
      motionFlame(Walk_Forward, 7);
      break;
    case 2:
      motionFlame(Walk_Forward, 8);
      motionFlame(Walk_Forward, 9);
      modeNum = 0;
      break;
    default:
      break;
  }
}

void PLEN5StackEX::motionFlame(uint16_t fileName, uint16_t flameNum) {
  int16_t angle[SERVO_NUM_USED] = {};
  const uint8_t listLen = 43;

  String str = "";

  bool loopBool = false;
  if (flameNum == 0xFF) {
    flameNum = 0;
    loopBool = true;
  }
  uint16_t readAdr = 0x32 + 860 * fileName + flameNum * listLen;;

  while (1)
  {
    String mFile = reep(readAdr, listLen);//"";
    readAdr += mFile.length();//listLen;

    //Serial.print(",mf = "); Serial.println(mFile);
    uint16_t listNum = 0;
    str = mFile.substring(0, 3);
    if (! str.equals(">MF")) {
      break;
    }
    listNum += 3; // >MF
    str = mFile.substring(listNum, listNum + 2);
    if (fileName != strtol(str.c_str(), NULL, 16)) {
      break;
    }
    listNum += 4;// slot,flame
    str = mFile.substring(listNum, listNum + 4);
    uint16_t time = strtol(str.c_str(), NULL, 16);
    listNum += 4;// time
    //Serial.print(",time="); Serial.println(time);
    for (uint8_t val = 0; val < SERVO_NUM_USED; val++)
    { //plen2 max servo = 24, plenbit = 7
      str = mFile.substring(listNum, listNum + 4);
      int16_t numHex = strtol(str.c_str(), NULL, 16);
      if (numHex >= 0x7fff)
      {
        numHex = numHex - 0x10000;
      }
      else
      {
        numHex = numHex & 0xffff;
      }
      angle[val] = numHex;
      //Serial.print(",val="); Serial.print(numHex);
      listNum += 4;
    }//for
    //Serial.println("");
    setAngle(angle, time);
    if (!loopBool)break;
  }


}
