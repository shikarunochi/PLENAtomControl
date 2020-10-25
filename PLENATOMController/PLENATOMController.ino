//PLEN ATOM Controller for M5Stack Core2
#include <M5Core2.h>
#include <esp_now.h>
#include <WiFi.h>

const uint8_t peer_addr[6]
  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //PLENM5ATOM MacAddress
esp_now_peer_info_t plenm5atom;
uint8_t data;

int mode = 0; //0:HOME 1:motion
boolean lightOn = false;
int homeTouchPos[][6] = { /* motionId, x, y, width, height */
  {255, 142, 128, 30, 30, 1000}, //リセット
  //{70, 130, 72, 60, 40, 2500}, //前に進む
  {70, 130, 72, 60, 40, 1100}, //前に進む
  {73, 130, 185, 60, 40, 2500}, //後ろに進む
  {71, 75, 113, 40, 60, 2500}, //左に回る
  {72, 200, 113, 40, 60, 2500}, //右に回る
  {24, 130, 0, 60, 40, 2000}, //ダッシュ
  {0, 35, 113, 40, 60, 1000}, //左ステップ
  {2, 241, 113, 40, 60, 1000}, //右ステップ

  {101, 15, 15, 70, 60, 1000}, //（特殊）ライトON/OFF
};

boolean isSending = false;
int waitMillSecond = 0;
unsigned long sendTime;

void OnDataSent(const uint8_t* mac, esp_now_send_status_t status) {
  isSending = false;
}

void setup() {
  M5.begin(true, true, true, true);

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.setTextSize(2);
  memset(&plenm5atom, 0, sizeof(plenm5atom));
  memcpy(plenm5atom.peer_addr,  peer_addr, 6);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_now_init();
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer((const esp_now_peer_info_t*)&plenm5atom);

  M5.Lcd.drawJpgFile(SD, "/plenController.jpg", 0, 0);
  sendTime = 0;
  waitMillSecond = 0;

 //起動時に目を一度点灯 
  delay(500);
  data = 101;
  esp_now_send((const uint8_t*)plenm5atom.peer_addr, &data, sizeof(data));
  delay(1000);
    data = 102;
  esp_now_send((const uint8_t*)plenm5atom.peer_addr, &data, sizeof(data));
}

void loop() {
  TouchPoint_t pos = M5.Touch.getPressPoint();
  //Serial.printf("x:%d, y:%d\n", pos.x, pos.y);
  if (millis() - sendTime > waitMillSecond) {
    if (pos.x > -1) {
      boolean sendFlag = false;

      for (int index = 0; index < sizeof(homeTouchPos) / sizeof(*homeTouchPos); index++) {
        //Serial.printf("x:%d, y:%d , X1:%d, Y1:%d, X2:%d, Y2:%d \n",pos.x,pos.y, homeTouchPos[index][1],homeTouchPos[index][2]
        //,homeTouchPos[index][1] + homeTouchPos[index][3] , homeTouchPos[index][2] + homeTouchPos[index][4]);
        if (pos.x >= homeTouchPos[index][1]
            && pos.x <= homeTouchPos[index][1] + homeTouchPos[index][3]
            && pos.y >= homeTouchPos[index][2]
            && pos.y <= homeTouchPos[index][2] + homeTouchPos[index][4]) {
          if (homeTouchPos[index][0] == 101) {
            if (lightOn == false) {
              data = 101;
              M5.Lcd.fillCircle(31, 45, 5, GREEN);
              M5.Lcd.fillCircle(66, 45, 5, GREEN);
            } else {
              data = 102;
              M5.Lcd.fillCircle(31, 45, 5, BLACK);
              M5.Lcd.fillCircle(66, 45, 5, BLACK);
            }
            lightOn = !lightOn;
          } else {
            data =  homeTouchPos[index][0];
          }
          sendFlag = true;
          waitMillSecond = homeTouchPos[index][5];
          sendTime = millis();
          break;
        }
      }

      if (sendFlag == true && isSending == false) {
        isSending = true;
        esp_now_send((const uint8_t*)plenm5atom.peer_addr, &data, sizeof(data));
        Serial.printf("Send:%d\n", data);
        delay(500);
      }
    }
  }
  delay(10);

}
