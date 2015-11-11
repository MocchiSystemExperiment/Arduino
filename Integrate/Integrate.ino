#include <Wire.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <ZumoBuzzer.h>
#include <LSM303.h>

ZumoMotors motors;
ZumoBuzzer buzzer;
Pushbutton button(ZUMO_BUTTON);
LSM303 compass;

#define SPEED    120 // default motor speed 
#define LEFT    1//左回り
#define RIGHT    0//右回り

float red_G, green_G, blue_G; //  RGB values
int zoneNumber_G; // zone number
int mode_G; // mode in each zone
unsigned long timeInit_G, timeNow_G; // start time, current time,
int motorR_G, motorL_G;  // input values to the motors

int startedDirection_G;//the direction


//zone4(棒倒し)で使用する変数
boolean findFlag = false;
boolean approachFlag = false;
int state_fsm;//switch文で使用
float azimuth = 0;
float start_azimuth;
float L,distanceL;//距離
float C = 340;//音速
int countPET = 0;//ペットボトルを倒した数
const int trig = 2;//Trig ピンをデジタル 2 番に接続
const int echo = 3; //Echo ピンをデジタル 3 番に接続
unsigned long interval;



void setup()
{
  Serial.begin(9600);
  Wire.begin();

  button.waitForButton();

  setupColorSensor(); // カラーセンサーのsetup
  CalibrationColorSensor(); // カラーセンサーのキャリブレーション
  setupCompass(); // 地磁気センサーのsetup
  CalibrationCompass(); // 地磁気センサーのキャリブレーション
  buzzer.play(">g32>>c32");

  zoneNumber_G = 0;
  mode_G = 0;
  timeInit_G = millis();

  button.waitForButton();
}

void loop()
{
  readRGB();//read color sensor value
  clearInterrupt();
  timeNow_G = millis() - timeInit_G;// calculate current time
  motors.setSpeeds(motorL_G, motorR_G);//set motor speeds
  sendData();// send data to PC

  switch ( zoneNumber_G ) {
    case 0:
      startToZone(); // start to zone
      break;
    case 1:
      zone(); // zone 1
      break;
    case 2:
      zone(); // zone 2
      break;
    case 3:
      zone(); // zone 3
      break;
    case 4:
      zone(); // zone 4
      break;
    case 5:
      zone(); // zone 5
      break;
    case 6:
      zone(); // zone 6
      break;
    case 7:
      zone(); // finish zone
      break;
    case 8:
      zoneToZone(); // zone to zone
      break;
    default:
      break;
  }

  // calculate timeout
  int timeout = steadyState( 30000 );
  if ( timeout == 1 ) {
    //reset variables
    mode_G = 0;
    zoneNumber_G = 8;
  }
}


int countTimeout( unsigned long period )
{
  static int flagStart = 0;
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


void sendData()
{
  static unsigned long timePrev = 0;

  if ( timeNow_G - timePrev > 50 ) { // 50msごとにデータ送信
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




    timePrev = timeNow_G;
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


