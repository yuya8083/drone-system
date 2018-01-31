#include <Servo.h>                                
#include <SoftwareSerial.h>

//#define SSID "HoseiMirai1"
//#define PASSWORD "isehara2200"
#define SSID "dronewifi"
#define PASSWORD "kanail5533"
//#define SSID "dronewifi"
//#define PASSWORD "kanail5533"
#define PASSWORDESP "13741374"
#define SERIAL_WAIT_TIME 5000
//#define IP_ADDRESS "192.168.11.2"
#define IP_ADDRESS "192.168.2.101"
#define PORT "8234"


Servo myservo;                         // サーボを制御するサーボオブジェクトを作成 
Servo myservo2;
Servo myservo3;
Servo myservo4;
SoftwareSerial esp8266Serial(12,13); // ESP8266用のRX, TX
 
int pos=0;                              // サーボの位置を保存する変数
//超音波センサ①ピンの設定
int URPWM1 = 2;                                     
int URTRIG1= 3;                        
//超音波センサ②ピンの設定
int URPWM2 = 4;                                     
int URTRIG2= 5;
//超音波センサ③ピンの設定
int URPWM3 = 6;                                     
int URTRIG3= 7;
//超音波センサ④ピンの設定
int URPWM4 = 8;                                     
int URTRIG4= 9;

boolean up = true;                         // プール変数を作成
unsigned long time;                        // 時間変数を作成
unsigned long urmTimer = 0;                // センサ読み取りのフレッシュ速度を管理するタイマー
 
unsigned int Distance1 = 0;                //センサの距離変数の初期化
unsigned int Distance2 = 0;
unsigned int Distance3 = 0;
unsigned int Distance4 = 0;
uint8_t EnPwmCmd[4] = {0x44,0x22,0xbb,0x01};        // 距離測定コマンド



//初めの1回のみ実行される
void setup(){
                                        
  //シリアルの設定
  Serial.begin(9600);                              // 通信速度を9600にする
  esp8266Serial.begin(9600);
  esp8266Serial.setTimeout(SERIAL_WAIT_TIME);       
  Serial.println("--Setup Start--");
  esp8266Serial.println("--Setup Start--");

  //Wi-Fi設定
  bool setupFinish = false;
  bool setupPCFinish = false;


  //AP+server
  sendATCommand("AT+CWMODE=1", SERIAL_WAIT_TIME);
  
  do{
    Serial.println("Try Connect Wi-Fi");
    setupFinish = setupWiFi();
    if(setupFinish){
      Serial.println("Connected");
    }
    else{
      Serial.println("Not Connected");
    }
  } while(setupFinish == 0); 

 do{
    Serial.println("Try Connect PC");
    setupPCFinish = setupWiFitoPC();
    if(setupPCFinish){
        Serial.println("Connected PC");
    }
    else{
        Serial.println("Not Connected PC");
    }
  } while(setupPCFinish == 0); 
  
  //AP+server
  sendATCommand("AT+CIPMODE=1", SERIAL_WAIT_TIME);
  //setupWiFitoESP();
  sendATCommand("AT+CIPSEND", SERIAL_WAIT_TIME);
  
  //超音波センサ設定
  PWM_Mode_Setup();
  PWM_Mode_Setup2();
  PWM_Mode_Setup3();
  PWM_Mode_Setup4();
}

//この範囲が繰り返し実行される
void loop(){   
   if(esp8266Serial.available())
    Serial.write(esp8266Serial.read());
   if (Serial.available())
    esp8266Serial.write(Serial.read());
   
   if(millis()-time>=20){                     // interval 0.02 seconds
     time = millis();                          // get the current time of programme
     if(up){                                   // judge the condition
      if(pos>=0 && pos<=179){                   
        pos=pos+1;                             // in steps of 1 degree 
        myservo.write(pos);                    // tell servo to go to position in variable 'pos' 
      }
      if(pos>179)  up= false;                  // assign the variable again
    }
     else {                                       
      if(pos>=1 && pos<=180){    
        pos=pos-1;
        myservo.write(pos);
      }
      if(pos<1)  up=true;
    }
   }
    
   if(millis()-urmTimer>50){
     urmTimer = millis();
     delay(500);     //ここで遅延をつくれる？？ 500ms安定する？ 100msだとちょい不安定？
     PWM_Mode();
     delay(500);    //ここで遅延をつくれる？？
     PWM_Mode2();
     delay(500);    //ここで遅延をつくれる？？
     PWM_Mode3();
     delay(500);    //ここで遅延をつくれる？？
     PWM_Mode4();
   }
}


//ピンの入出力設定①
void PWM_Mode_Setup(){ 
   pinMode(URTRIG1,OUTPUT);                // COMP/TRIG（UTRIG1R）ピンを出力に
   digitalWrite(URTRIG1,HIGH);             // COMP/TRIG（UTRIG1R）ピンをHIGH(5Vor3.3V)に
   pinMode(URPWM1, INPUT);                 // PWMモードピンを入力に（有効に送信する）
   for(int i=0;i<4;i++){
       //Serial.write(EnPwmCmd[i]);
       esp8266Serial.write(EnPwmCmd[i]);
   } 
}

//ピンの入出力設定②
void PWM_Mode_Setup2(){ 
   pinMode(URTRIG2,OUTPUT);                            
   digitalWrite(URTRIG2,HIGH);                          
   pinMode(URPWM2, INPUT);                             
   for(int i=0;i<4;i++){
       //Serial.write(EnPwmCmd[i]);
       esp8266Serial.write(EnPwmCmd[i]);
   } 
}
  
//ピンの入出力設定③
void PWM_Mode_Setup3(){ 
   pinMode(URTRIG3,OUTPUT);                            
   digitalWrite(URTRIG3,HIGH);                         
   pinMode(URPWM3, INPUT);                             
   for(int i=0;i<4;i++){
      //Serial.write(EnPwmCmd[i]);
      esp8266Serial.write(EnPwmCmd[i]);
   } 
}

//ピンの入出力設定④
void PWM_Mode_Setup4(){ 
   pinMode(URTRIG4,OUTPUT);                            
   digitalWrite(URTRIG4,HIGH);                         
   pinMode(URPWM4, INPUT);                             
   for(int i=0;i<4;i++){
      //Serial.write(EnPwmCmd[i]);
      esp8266Serial.write(EnPwmCmd[i]);
   } 
}

  
//センサ①情報の抽出
void PWM_Mode(){                                     
    digitalWrite(URTRIG1, LOW);
    digitalWrite(URTRIG1, HIGH);                     
    unsigned long DistanceMeasured1 = pulseIn(URPWM1,LOW);  //実際にかかった時間（パルスの検出）
    //距離測定
    if(DistanceMeasured1==50000){                     // 読み取りが無効の場合
      Serial.print("Invalid");    
    }
    else{
      Distance1 = DistanceMeasured1 / 50;              // 実際にかかった時間を距離に変換
    }
     //距離の書き出し
     Serial.print("1=");
     Serial.print(Distance1);
     Serial.print("\n");
     esp8266Serial.print("1=");
     esp8266Serial.print(Distance1);
     esp8266Serial.print("\n");
     //sendATCommand("AT+CIPSEND=3", 0);
  }

//センサ②情報の抽出
void PWM_Mode2(){                                     
    digitalWrite(URTRIG2, LOW);                       
    digitalWrite(URTRIG2, HIGH);
    unsigned long DistanceMeasured2 = pulseIn(URPWM2,LOW);
    //距離測定
    if(DistanceMeasured2==50000){                    
      Serial.print("Invalid");    
    }
    else{
      Distance2 = DistanceMeasured2 / 50;              
    }
    //距離の書き出し
     Serial.print("2=");
     Serial.print(Distance2);
     Serial.print("\n");
     esp8266Serial.print("2=");
     esp8266Serial.print(Distance2);
     esp8266Serial.print("\n");
     //sendATCommand("AT+CIPSEND=3", 0);
}

//センサ③情報の抽出
void PWM_Mode3(){                                    
    digitalWrite(URTRIG3, LOW);                       
    digitalWrite(URTRIG3, HIGH);
    unsigned long DistanceMeasured3 = pulseIn(URPWM3,LOW);
    //距離測定
    if(DistanceMeasured3==50000){                    
      Serial.print("Invalid");    
    }
    else{
      Distance3 = DistanceMeasured3 / 50;              
    }
    //距離の書き出し
     Serial.print("3=");
     Serial.print(Distance3);
     Serial.print("\n");
     esp8266Serial.print("3=");
     esp8266Serial.print(Distance3);
     esp8266Serial.print("\n");
     //sendATCommand("AT+CIPSEND=3",0);
}

//センサ④情報の抽出
void PWM_Mode4(){                                    
    digitalWrite(URTRIG4, LOW);                       
    digitalWrite(URTRIG4, HIGH);
    unsigned long DistanceMeasured4 = pulseIn(URPWM4,LOW);
    //距離測定
    if(DistanceMeasured4==50000){                    
      Serial.print("Invalid");    
    }
    else{
      Distance4 = DistanceMeasured4 / 50;              
    }
    //距離の書き出し
     Serial.print("4=");
     Serial.print(Distance4);
     Serial.print("\n");
     esp8266Serial.print("4=");
     esp8266Serial.print(Distance4);
     esp8266Serial.print("\n");
     //sendATCommand("AT+CIPSEND=3", 0);
}



//APとのWi-Fiの設定(準備)
bool setupWiFi() {
  if(sendATCommand("AT+RST", SERIAL_WAIT_TIME)) {        //AT+RST > Reset
    String wifiConnectCmd = "AT+CWJAP=\"";               //AT+CWJAP="SSID","PASSWORD" > WiFi-Connect
    wifiConnectCmd += SSID;
    wifiConnectCmd += "\",\"";
    wifiConnectCmd += PASSWORD;
    wifiConnectCmd += "\"";
    return sendATCommand(wifiConnectCmd, SERIAL_WAIT_TIME);
  } 
  else {
    return false;
  }
 }
 
//Wi-Fi設定（送信）
bool sendATCommand(String cmd, int waitTime) {
  //シリアルコマンドの送信
  esp8266Serial.print(cmd);
  esp8266Serial.print("\r\n");
  
  //コマンドのログ
  Serial.print("AT Command > ");
  Serial.println(cmd);

  delay(waitTime);

  //シリアルバッファからの応答を確認
  if(esp8266Serial.find("OK")) {
    Serial.println("OK");
    return true;
  } else {
    Serial.println("NG");
    return false;
  }
 }

bool setupWiFitoPC(){
    Serial.println("next to PC");
    String PCwifiConnectCmd = "AT+CIPSTART=\"TCP";      //AT+CIPSTART="TCP","IP_ADRESS","PORT" > WiFi-Connect
    PCwifiConnectCmd += "\",\"";
    PCwifiConnectCmd += IP_ADDRESS;
    PCwifiConnectCmd += "\",";
    PCwifiConnectCmd += PORT;
    //PCwifiConnectCmd += "\"";
    return sendATCommand(PCwifiConnectCmd, SERIAL_WAIT_TIME);
}


