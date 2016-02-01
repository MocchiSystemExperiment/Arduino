const float colorzone3[4][3] = { // identifyZone()用の固定値
  {
    100, 100, 100
  },
  //白
  {
    57.00, 8.00, 2.00
  }
  ,//赤
  {
    13.00, 46.00, 18.00
  }
  ,//緑
  {
    2.00, 8.00, 34.00
  }
  //青(濃い)
};

void sendZone3Data() {

  setData('Q');
  for (int i = 0; i < 5; i++) {
    for (int c = 0; c < 5; c++) {
      setData(masu[i][c]);

    }
  }
  sendingData();
}


int masu[5][5] = {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
int pattern[3] = {0, 0, 0};
int patternNamber = 0;
int retu = 2;
int gyou = 4;
int masutime = 790;
int i = 0;

void zone3() {

  ///タイムアウト処理
  int timeout = countTimeout( 20003 );
  if ( timeout == 1 ) {
    //reset variables
    mode_G = 0;
    zoneNumber_G = 9;
    azimthswitch = 0;
     sendZone3Data();
    return;
  }


  int done;
  int iro;
  int zone3Azimuthup = 112, zone3Azimuthleft = 28, zone3Azimuthrigth = 208, zone3Azimuthdown = 294, zone3nowAzimuth;
  //Serial.println(identifyZone3());
  /*for(retu = 0; retu < 5; retu++){
      for(gyou = 0; gyou < 5; gyou++){
        Serial.println(masu[retu][gyou]);
      }
  }*/

  switch ( mode_G ) {
    case 0: // setupが必要ならここ（必要が無くても形式的に）

      mode_G = 1;
      break;

    case 1: // 始めの3色読み込む
      zone3nowAzimuth = averageHeading();
      if (zone3Azimuthup - zone3nowAzimuth < 0) {
        motorR_G = 70;
        motorL_G = 30;
      }
      else if (zone3Azimuthup - zone3nowAzimuth > 0) {
        motorR_G = 30;
        motorL_G = 70;
      }

      iro = identifyZone3();
      if ( iro != 0 && patternNamber == 0 || pattern[patternNamber - 1] != iro ) {
        if (identifyZone3() == 1 ||
            identifyZone3() == 2 ||
            identifyZone3() == 3) {
          pattern[patternNamber] = iro;
          patternNamber++;
          buzzer.play(">g32>>c32");
        }
        if ( patternNamber == 3 ) { // 3色読み込んだらprocessingに送ってmode2に
          /*Serial.println("pattern");
          Serial.println(pattern[0]);
          Serial.println(pattern[1]);
          Serial.println(pattern[2]);
          */

          setData('P');
          setData(pattern[0]);
          setData(pattern[1]);
          setData(pattern[2]);
          sendingData();

          mode_G = 2;
        }
        else {
        }
      }
      break;

    case 2://白を検知するまで前進
      zone3nowAzimuth = averageHeading();
      if (zone3Azimuthup - zone3nowAzimuth < 0) {
        motorR_G = 100;
        motorL_G = 80;
      }
      else if (zone3Azimuthup - zone3nowAzimuth > 0) {
        motorR_G = 80;
        motorL_G = 100;
      }


      if (red_G > 75 && green_G > 75 && blue_G > 75) {
        mode_G = 3;
      }
      break;

    case 3://白以外の色を検知するまで進む

      iro = identifyZone3();
      if (identifyZone3() == 1 ||
          identifyZone3() == 2 ||
          identifyZone3() == 3) {
        motors.setSpeeds(0, 0);
        delay(500);
        mode_G = 4;
      }
      break;

    case 4://色を送りながら白まで進む
      zone3nowAzimuth = averageHeading();
      if (zone3Azimuthup - zone3nowAzimuth < 0) {
        motorR_G = 100;
        motorL_G = 80;
      }
      else if (zone3Azimuthup - zone3nowAzimuth > 0) {
        motorR_G = 80;
        motorL_G = 100;
      }

      done = steadyState( masutime );

      if (done == 1) {
        iro = identifyZone3();
        if (identifyZone3() == 0) {
          mode_G = 5;
        }
        else if (identifyZone3() != 0 && i < 5) {
          masu[retu][gyou] = identifyZone3();
           sendZone3Data();
          buzzer.play(">g32>>c32");
          i++;
          if (gyou != 0) {
            gyou--;
            //buzzer.play(">g32>>c32");
          }
          else {
            gyou = 0;
            retu = 1;
            i = 0;
          }
          //Serial.println(identifyZone3());
        }
      }

      break;

    case 5://止まる
      motorR_G = 0;
      motorR_G = 0;
      mode_G = 6;
      break;

    case 6://下がる
      done = steadyState(200);
      motorR_G = -100;
      motorL_G = -100;
      if (done == 1) {
        mode_G = 7;
      }
      break;

    case 7://左に向く
      motorR_G = 75;
      motorL_G = -75;
      zone3nowAzimuth = averageHeading();
      if (zone3nowAzimuth > 25 && zone3nowAzimuth < 31) {
        mode_G = 8;
      }
      break;

    case 8://進む
      done = steadyState(1200);
      zone3nowAzimuth = averageHeading();
      if (done == 1) {
        mode_G = 9;
      }
      else if (done == 0 && zone3Azimuthleft - zone3nowAzimuth < 0) {
        motorR_G = 100;
        motorL_G = 80;
      }
      else if (done == 0 && zone3Azimuthleft - zone3nowAzimuth > 0) {
        motorR_G = 80;
        motorL_G = 100;
      }
      break;

    case 9://左に90度回る
      motorR_G = 75;
      motorL_G = -75;
      zone3nowAzimuth = averageHeading();
      if (zone3nowAzimuth > 295 && zone3nowAzimuth < 301) {
        mode_G = 10;
      }
      break;

    case 10://入り口の方向に進む
      zone3nowAzimuth = averageHeading();
      if (zone3Azimuthdown - zone3nowAzimuth < 0) {
        motorR_G = 100;
        motorL_G = 80;
      }
      else if (zone3Azimuthdown - zone3nowAzimuth > 0) {
        motorR_G = 80;
        motorL_G = 100;
      }

      done = steadyState( masutime );

      if (done == 1) {
        iro = identifyZone3();
        if (identifyZone3() == 0) {
          mode_G = 11;
        }
        else if (identifyZone3() != 0 && i < 5) {
          masu[retu][gyou] = identifyZone3();
          buzzer.play(">g32>>c32");
           sendZone3Data();
          i++;
          if (gyou != 4) {
            //buzzer.play(">g32>>c32");
            gyou++;
          }
          else {
            gyou = 4;
            retu = 0;
            i = 0;
          }

          //Serial.println(identifyZone3());
        }
      }
      break;

    case 11://ちょっと下がる
      done = steadyState(200);
      motorR_G = -100;
      motorL_G = -100;
      if (done == 1) {
        mode_G = 12;
      }
      break;

    case 12://右に向く
      motorR_G = -75;
      motorL_G = 75;
      zone3nowAzimuth = averageHeading();
      if (zone3nowAzimuth > 25 && zone3nowAzimuth < 31) {
        mode_G = 13;
      }
      break;

    case 13://進む
      done = steadyState(1200);
      zone3nowAzimuth = averageHeading();
      if (done == 1) {
        mode_G = 14;
      }
      else if (done == 0 && zone3Azimuthleft - zone3nowAzimuth < 0) {
        motorR_G = 100;
        motorL_G = 80;
      }
      else if (done == 0 && zone3Azimuthleft - zone3nowAzimuth > 0) {
        motorR_G = 80;
        motorL_G = 100;
      }
      break;

    case 14://右に向く
      motorR_G = -75;
      motorL_G = 75;
      zone3nowAzimuth = averageHeading();
      if (zone3nowAzimuth > 110 && zone3nowAzimuth < 115) {
        mode_G = 15;
      }
      break;

    case 15://色を送りながら白まで進む
      zone3nowAzimuth = averageHeading();
      if (zone3Azimuthup - zone3nowAzimuth < 0) {
        motorR_G = 100;
        motorL_G = 80;
      }
      else if (zone3Azimuthup - zone3nowAzimuth > 0) {
        motorR_G = 80;
        motorL_G = 100;
      }

      done = steadyState( masutime );

      if (done == 1) {
        iro = identifyZone3();
        if (identifyZone3() == 0) {
          mode_G = 16;
        }
        else if (identifyZone3() != 0 && i < 5) {
          masu[retu][gyou] = identifyZone3();
           sendZone3Data();
          buzzer.play(">g32>>c32");
          i++;
          if (gyou != 0) {
            //buzzer.play(">g32>>c32");
            gyou--;
          }
          else {
            gyou = 0;
            retu = 3;
            i = 0;
          }

          //Serial.println(identifyZone3());
        }
      }
      break;

    case 16://止まって５x５マスのデーターを送る
      motorR_G = 0;
      motorL_G = 0;
      break;


  }
}


// KNNで現在最も近い番号を返す
int identifyZone3()
{
  float minDistance = 999999999;
  static int iro;
  float distance;

  for ( int i = 0; i < 5; ++i ) {
    distance = (red_G - colorzone3[i][0]) * (red_G - colorzone3[i][0])
               + (green_G - colorzone3[i][1]) * (green_G - colorzone3[i][1])
               + (blue_G - colorzone3[i][2]) * (blue_G - colorzone3[i][2]);
    if ( distance < minDistance ) {
      minDistance = distance;
      iro = i;
    }
  }
  return iro;
}

