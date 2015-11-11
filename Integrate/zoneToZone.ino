const float color[7][3] = { // identifyZone()用の固定値
  {93.00, 88.00, 35.00},
  {53.00, 20.00, 38.00},
  {29.00, 61.00, 68.00},
  {57.00, 9.00, 5.00},
  {17.00, 45.00, 16.00},
  {6.00, 10.00, 37.00},
  {14.00, 20.00, 51.00}
};

float minDistance; // identifyZone()用のグローバル変数

// startからzoneへの移動（zone番号の認識もする）
void startToZone()
{
  int zoneNumber;
  int done;

  switch ( mode_G ) {
    case 0: // setupが必要ならここ（必要が無くても形式的に）
      mode_G = 1;
      break;
    case 1: // ライントレース（黄と黒の混合色を検知するまで）
      linetracePID();
      done = identifyColor( 2 );
      if ( done == 1 ) {
        mode_G = 2;
      }
      break;
    case 2: // 黒を検知するまで直進
      motorR_G = 50;
      motorL_G = 50;
      done = identifyColor( 0 );
      if ( done == 1 )

        mode_G = 3;
      break;
    case 3: // identifyZone()のsetup
      minDistance = 9999999;//色差
      mode_G = 4;

      break;
    case 4: // 白を検知するまで直進（その間ゾーン番号を検知）
      motorR_G = 50;
      motorL_G = 50;
      zoneNumber = identifyZone();
      done = identifyColor( 1 );
      if ( done == 1 ) {

        zoneNumber_G = zoneNumber;

        timeInit_G = millis();
        mode_G = 0;
        startedDirection_G = averageHeading();//calculate the direction

      }
      break;
    case -1:
      mode_G = 0;
      break;
    default:
      break;
  }
}

// zoneからzoneへの移動（zone番号の認識もする）
void zoneToZone()
{
  int zoneNumber;
  int done;

  switch ( mode_G ) {
    case 0: // setupが必要ならここ（必要が無くても形式的に）
      motorL_G = 100;
      motorR_G = -100;
      int dif;
      dif =   startedDirection_G - averageHeading();
      if (abs(dif) < 5)mode_G = 1;

      break;

    //escape the zone
    case 1:  // 黒を検知するまで直進
      goStraight();
      done = identifyColor( 0 );
      if ( done == 1 ) {
        mode_G = 2;

      }
      break;
    case 2://白を検知するまで旋回
      motorL_G = 200;
      motorR_G = -150;
      done = identifyColor( 1 );
      if ( done == 1 ) {
        mode_G = 3;

      }
      break;
    case 3:// 黒と黄色の混合色を検知するまでライントレース
      linetracePID2(180, 1.5, 2.6);
      done = identifyColor( 2 );
      if ( done == 1 ) {
        mode_G = 4;
      }
      break;
    case 4://白色を検知するまでゆっくり前進
      motorL_G = 50;
      motorR_G = 50;
      done = identifyColor( 1 );
      if ( done == 1 ) {
        mode_G = 5;

      }
      break;

    //from the previous zone to the next zone
    case 5: // ライントレース（黄と黒の混合色を検知するまで）
      linetracePID2(200, 1.5, 2.5);
      done = identifyColor( 2 );
      if ( done == 1 ) {
        mode_G = 6;

      }
      break;
    case 6: // 黒を検知するまで直進
      goStraight();
      done = identifyColor( 0 );
      if ( done == 1 )
        mode_G = 7;

      break;
    case 7: // identifyZone()のsetup
      minDistance = 9999999;//色差設定
      mode_G = 8;

      break;
    case 8: // 白を検知するまで直進（その間ゾーン番号を検知）
      goStraight();
      zoneNumber = identifyZone();
      done = identifyColor( 1 );
      if ( done == 1 ) {
        zoneNumber_G = zoneNumber;
        mode_G = 0;

        startedDirection_G = averageHeading();//calculate the direction and store it

        timeInit_G = millis();//Zoneに入った時間を記録

      }
      break;

    //restart  ZonetoZone program
    case -1:
      mode_G = 0;

    default:
      break;

  }
}

// 直進する
void goStraight()
{
  motorL_G = SPEED;
  motorR_G = SPEED;
}

// 黒白の境界に沿ってライントレース
void linetracePID()
{
  static unsigned long timePrev = 0;
  static float lightPrevPD = 0.0;

  float lightNowPD;
  float error, errorSP;
  float diff, diffSP;
  float speedDiff;

  float target = 50;
  float Kp = 1.5;
  float Kd = 1.8;//0.5;

  lightNowPD =  ( red_G + green_G + blue_G ) / 3.0;
  error = lightNowPD - target;
  errorSP = map(error, -target, target, -SPEED, SPEED );
  diff = (lightNowPD - lightPrevPD) / (timeNow_G - timePrev );
  diffSP  = map(diff, -100.0, 100.0, -SPEED, SPEED );

  speedDiff = Kp * errorSP + Kd * diffSP;

  motorL_G = SPEED - speedDiff;
  motorR_G = SPEED + speedDiff;

  timePrev = timeNow_G;
  lightPrevPD = lightNowPD;
}



void linetracePID2(int speed, float Kp, float Kd)
{
  static unsigned long timePrev = 0;
  static float lightPrevPD = 0.0;

  float lightNowPD;
  float error, errorSP;
  float diff, diffSP;
  float speedDiff;

  float target = 50;
  // float Kp = 1.5;
  // float Kd = 1.8;//0.5;

  lightNowPD =  ( red_G + green_G + blue_G ) / 3.0;
  error = lightNowPD - target;
  errorSP = map(error, -target, target, -speed, speed );
  diff = (lightNowPD - lightPrevPD) / (timeNow_G - timePrev );
  diffSP  = map(diff, -100.0, 100.0, -speed, speed );

  speedDiff = Kp * errorSP + Kd * diffSP;

  motorL_G = speed - speedDiff;
  motorR_G = speed + speedDiff;

  timePrev = timeNow_G;
  lightPrevPD = lightNowPD;
}

// 指定の色を連続４回認識したら1を返す（それ以外0)
int identifyColor( int color )
{
  static int count = 0; // この関数が初めて呼ばれた時にのみ初期化される

  if ( color == 0 && red_G < 25 && green_G < 25 && blue_G < 25 )  // 黒を感知
    ++count;
  else if ( color == 1 && red_G > 75 && green_G > 75 && blue_G > 75 ) // 白を感知
    ++count;
  else if ( color == 2 && ( red_G + green_G ) / 2.0 * 0.7  > blue_G && ( red_G + green_G ) * 0.5 > 30 )  // 黄黒の混合色を感知
    ++count;
  else
    count = 0;

  if ( count > 3 ) { // パラメーター
    count = 0; // 次に呼ばれる時に備えて値を初期値に戻す
    return 1;
  }
  else
    return 0;
}

// KNNで現在最も近い番号を返す
int identifyZone()
{
  // float minDistance; グローバル変数で定義
  static int zoneNumber;
  float distance;

  for ( int i = 0; i < 7; ++i ) {
    distance = (red_G - color[i][0]) * (red_G - color[i][0])
               + (green_G - color[i][1]) * (green_G - color[i][1])
               + (blue_G - color[i][2]) * (blue_G - color[i][2]);
    if ( distance < minDistance ) {
      minDistance = distance;
      zoneNumber = i;
    }
  }

  return zoneNumber + 1; // zone番号は1-7なので+1
}



