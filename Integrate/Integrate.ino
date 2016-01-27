#include <Wire.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <ZumoBuzzer.h>
#include <LSM303.h>

ZumoMotors motors;
ZumoBuzzer buzzer;
Pushbutton button(ZUMO_BUTTON);
LSM303 compass;

#define MAX_SENDING_BUFFER 48

#define SPEED    120 // default motor speed 
#define LEFT    1//左回り
#define RIGHT    0//右回り

float red_G, green_G, blue_G; //  RGB values
int zoneNumber_G; // zone number
int mode_G; // mode in each zone
unsigned long timeInit_G, timeNow_G; // start time, current time,
int motorR_G, motorL_G;  // input values to the motors

int startedDirection_G;//the direction

byte sendBuffer[MAX_SENDING_BUFFER];
int  cur=0;
int zone_in = 0; //ゾーンに入ったらloop最初のmotors.setSpeed()に入らないようにする   0->入る 1->入らない

//0==on 1=off
int azimthswitch = 0;

//zone2(棒倒し)で使用する変数
boolean findFlag = false;
boolean approachFlag = false;
int state_fsm;//switch文で使用
float azimuth = 0;
float start_azimuth;
unsigned long L, distanceL; //距離
float C = 340;//音速
int zone2SL = 0, zone2SR = 0;
int countPET = 0;//ペットボトルを倒した数
int countOnePET = 0;
const int trig = 2;//Trig ピンをデジタル 2 番に接続
const int echo = 4; //Echo ピンをデジタル 3 番に接続
const int power = 13;
unsigned long interval;


//zone5
int zone5Flag = 0;//zone5のswitch文Flag
int ratioX = 0; //傾きの比率
int ratioY = 0; //傾きの比率
int zone5SL = 0, zone5SR = 0;
void setup()
{
  Serial.begin(9600);
  Wire.begin();

  button.waitForButton();

  setupColorSensor(); // カラーセンサーのsetup
  //CalibrationColorSensor(); // カラーセンサーのキャリブレーション

  
  setupCompass(); // 地磁気センサーのsetup
  //CalibrationCompass(); // 地磁気センサーのキャリブレーション
  compass.m_max.x = 4087;
  compass.m_max.y = 3398;
  compass.m_min.x = -3792;
  compass.m_min.y = -3792;
  buzzer.play(">g32>>c32");

  zoneNumber_G = 0;
  mode_G = 0;
  timeInit_G = millis();

  button.waitForButton();
  for(int i=0;i<MAX_SENDING_BUFFER;i++){
    sendBuffer[i]='\0';
  }
  cur=0;
}

void loop()
{
  readRGB();//read color sensor value
  clearInterrupt();
  timeNow_G = millis() - timeInit_G;// calculate current time
  if (zone_in == 0) { //zoneの処理をしているときは入らない
    motors.setSpeeds(motorL_G, motorR_G);//set motor speeds
  }
  azimuth = averageHeading();
  if (azimthswitch == 0)azimuth = averageHeading();
  sendData();// send data to PC

  switch ( zoneNumber_G ) {
    case 0:
<<<<<<< HEAD
      startToZone(); // start to zone
      //zone1();
=======
      startToZone();
>>>>>>> 009acf58cc6c8474ff0b8f81756e7d46e0ba35b0
      break;
    case 1:
      zone1(); // zone 1
      break;
    case 2:
      zone2(); // zone 2
      break;
    case 3:
      zone3(); // zone 3
      break;
    case 4:
<<<<<<< HEAD
      zone4(); // zone 4
=======
      zone2(); // zone 4
>>>>>>> 009acf58cc6c8474ff0b8f81756e7d46e0ba35b0
      break;
    case 5:
      zone5(); // zone 5
      break;
    case 6:
      zone_in = 0;
      zone6(); // zone 6
      break;
    case 7:
      zone(); // finish zone
      break;
    case 8:
      zoneToZone(); // zone to zone
      break;
    case 9:
      zone_in = 0;
      highSpeed_LineTrace();
    default:
      break;
  }
}


int countTimeout( unsigned long period )
{
  static int flagStart = 0;
  static int beforeZone = zoneNumber_G;
  if (beforeZone != zoneNumber_G) {
    flagStart = 0;
  }
  static  unsigned long startTime = 0;

  if ( flagStart == 0 ) {
    startTime = timeNow_G;
    flagStart = 1;
  }

  if ( timeNow_G - startTime > period ) {
    flagStart = 0;
    startTime = 0; // 必要ないがstatic変数は形式的に初期化
    return 1;
  }
  else
    return 0;
}

void setData(byte  data){
  if(cur<MAX_SENDING_BUFFER)sendBuffer[cur++]=data;
}
void sendingData(){
  Serial.write(sendBuffer,cur);
  cur=0;
}


void sendData()
{
  static unsigned long timePrev = 0;

  if ( timeNow_G - timePrev > 100 ) { // 100msごとにデータ送信
  /*
    Serial.write('H');
    Serial.write(zoneNumber_G);
    Serial.write(mode_G);
    Serial.write((int)red_G);
    Serial.write((int)green_G);
    Serial.write((int)blue_G);

    //send max/min values of acc and  geomagnetic sensor
    Serial.write(compass.m_max.x >> 8);
    Serial.write(compass.m_max.x & 255);
    Serial.write(compass.m_max.y >> 8);
    Serial.write(compass.m_max.y & 255);
    Serial.write(compass.m_min.x >> 8);
    Serial.write(compass.m_min.x & 255);
    Serial.write(compass.m_min.y >> 8);
    Serial.write(compass.m_min.y & 255);
    //send the sensor values of the geomagnetic sensor
    Serial.write(compass.m.x >> 8);
    Serial.write(compass.m.x & 255);
    Serial.write(compass.m.y >> 8);
    Serial.write(compass.m.y & 255);

    Serial.write(compass.a.x >> 8);
    Serial.write(compass.a.x & 255);
    Serial.write(compass.a.y >> 8);
    Serial.write(compass.a.y & 255);
    Serial.write(compass.a.z >> 8);
    Serial.write(compass.a.z & 255);
    //send the direction
    Serial.write((int)(azimuth) >> 8);
    Serial.write((int)(azimuth) & 255);


    interval =    timeNow_G -  timePrev;
    Serial.write(interval >> 24);
    Serial.write(interval >> 16);
    Serial.write(interval >> 8);
    Serial.write(interval & 255);

    Serial.write(motorR_G >> 8);
    Serial.write(motorR_G & 255);


    Serial.write(motorL_G >> 8);
    Serial.write(motorL_G & 255);
    timePrev = timeNow_G;
    
    */
    setData('H');
    setData(zoneNumber_G);
    setData(mode_G);
    setData((int)red_G);
    setData((int)green_G);
    setData((int)blue_G);
    interval =    timeNow_G -  timePrev;
    timePrev = timeNow_G;
    setData(interval >> 24);
    setData(interval >> 16);
    setData(interval >> 8);
    setData(interval & 255);
    setData(motorR_G >> 8);
    setData(motorR_G & 255);
    setData(motorL_G >> 8);
    setData(motorL_G & 255);
    setData((int)(azimuth) >> 8);
    setData((int)(azimuth) & 255);
    sendingData();
    
    
    setData('D');
    //send max/min values of acc and  geomagnetic sensor
    setData(compass.m_max.x >> 8);
    setData(compass.m_max.x & 255);
    setData(compass.m_max.y >> 8);
    setData(compass.m_max.y & 255);
    setData(compass.m_min.x >> 8);
    setData(compass.m_min.x & 255);
    setData(compass.m_min.y >> 8);
    setData(compass.m_min.y & 255);
    sendingData();
    
    
    
    
    //send the sensor values of the geomagnetic sensor
    setData('M');
    setData(compass.m.x >> 8);
    setData(compass.m.x & 255);
    setData(compass.m.y >> 8);
    setData(compass.m.y & 255);
    sendingData();


    setData('A');
    setData(compass.a.x >> 8);
    setData(compass.a.x & 255);
    setData(compass.a.y >> 8);
    setData(compass.a.y & 255);
    setData(compass.a.z >> 8);
    setData(compass.a.z & 255);
    sendingData();


        
  }
}


//方向とスピードを引数にすることで、方向転換を楽にする
void motorDrive(boolean direction, int rotateSpeed) { //第一引数->方向、第二引数->回るスピード
  //direction=1; 左回り
  //direction=0; 右回り

  int mappedRotateSpeed = map(rotateSpeed, 0, 100, 0, 400);//100段階でスピード変更
  if (direction)
    motors.setSpeeds(-100 * mappedRotateSpeed, 100 * mappedRotateSpeed);

  else
    motors.setSpeeds(100 * mappedRotateSpeed, -100 * mappedRotateSpeed);

  return;
}



