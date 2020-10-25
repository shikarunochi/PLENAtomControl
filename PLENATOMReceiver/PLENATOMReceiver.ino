#include "M5Atom.h"
#include <esp_now.h>
#include <WiFi.h>
#include "PLEN5StackEX.h"
#include <Adafruit_NeoPixel.h> 


PLEN5StackEX plen5stack;
const int LED_PIN = 22;
#define SCL 21
#define SDA 25

void OnDataRecv(const uint8_t* mac, const uint8_t* data, int data_len){
  if(data_len == 0){
    return;
  }
  int moveData = *data;
  if(moveData == 70){//連続前進
    M5.dis.drawpix(12,0x500000);
    plen5stack.walk(1);
    delay(100);
    M5.dis.drawpix(12,0x000000);
    return;
  }
  if(moveData < 100){ //100以下なら、そのままモーションとして再生する。
      M5.dis.drawpix(12,0x500000);
      plen5stack.motion(moveData);
      delay(100);
      M5.dis.drawpix(12,0x000000);
      return;
  }

  //101 ライト点灯 102 ライト消灯
  if(moveData == 101){
      digitalWrite( LED_PIN, LOW );
      return;
  }
  if(moveData == 102){
      digitalWrite( LED_PIN, HIGH );
      return;
  }

  //255 モーションリセット
  if(moveData == 255){
      digitalWrite( LED_PIN, LOW );
      plen5stack.servoInitialSet();
      delay(500);
      digitalWrite( LED_PIN, HIGH );
      return;
  }
  
}

void setup(){
  M5.begin(true, false, true);
  delay(10);
  Wire.begin(SDA, SCL);
  uint8_t stationMac[6];
  esp_read_mac(stationMac, ESP_MAC_WIFI_STA);
  Serial.printf("[Wi-Fi Station] Mac Address = {0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X}\r\n", stationMac[0], stationMac[1], stationMac[2], stationMac[3], stationMac[4], stationMac[5]);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_now_init();
  esp_now_register_recv_cb(OnDataRecv);
  
  pinMode(LED_PIN,OUTPUT);
  digitalWrite( LED_PIN, HIGH );
  plen5stack.servoInitialSet();
}

void loop(){
  taskYIELD();
}
