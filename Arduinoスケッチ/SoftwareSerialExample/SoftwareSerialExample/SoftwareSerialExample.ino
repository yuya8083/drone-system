#include "ESP8266.h"
#include "SoftwareSerial.h"

#define SSID "Google"
#define PASSWORD "kanaiatsushil"
//#define HOST_NAME "192.168.xx.xx"
//#define FILE_NAME "test.php"

SoftwareSerial mySerial(12, 13);  //RX, TX
ESP8266 wifi(mySerial);

/**
 * 初期設定
 */
void setup(void)
{
  Serial.begin(9600);
  Serial.print("setup begin\r\n");
  
  if (wifi.setOprToStation()) {
    Serial.println("to station ok\r\n");
  } else {
    Serial.println("to station error\r\n");
  }

  if (wifi.joinAP(SSID, PASSWORD)) {
    Serial.print("connect success\r\n");
    Serial.print("IP: ");
    Serial.println(wifi.getLocalIP().c_str());
  } else {
    Serial.println("connect error\r\n");
  }
}

void loop(void)
{
  //n++;

  // TCPで接続
 //wifi.createTCP(HOST_NAME, 80);
  
  // サーバーへ渡す情報
 //char sendStr[128];
 //sprintf(sendStr, "GET /%s?data=%d HTTP/1.0\r\nHost: %s\r\nUser-Agent: arduino\r\n\r\n", FILE_NAME, n, HOST_NAME);
 //wifi.send((const uint8_t*)sendStr, strlen(sendStr));

  //Serial.println(n);

  //delay(100);
 }
