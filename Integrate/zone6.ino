int zone6BeforeMotion = 0;

void zone6() {

  //タイムアウト処理
  int timeout = countTimeout( 20006 );
  if ( timeout == 1 ) {
    //reset variables
    mode_G = 0;
    zoneNumber_G = 8;
    azimthswitch = 0;
    return;
  }



  int speed = 100;
  int kp = 3;
  int kd = 7;


  azimthswitch = 1;
  int done;

  switch ( mode_G ) {
    case 0: // setupが必要ならここ（必要が無くても形式的に）
      mode_G = 1;
      break;
    case 1: // 青まで前進
      motorR_G = 120;
      motorL_G = 120;
      done = identifyColor2( 0 );

      if ( done == 1 ) {
        mode_G = 2;
      }
      break;


    case 2: // 白を検知するまで右旋回
      motorR_G = -200;
      motorL_G = 200;
      done = identifyColor( 1);
      if ( done == 1 ) {
        mode_G = 3;
        // Serial.write('S');
      }
      break;

    case 3: //ライントレース 赤検知
      setSpeedZone6();
      done = identifyColor2( 1 );
      if ( done == 1 ) {
        mode_G = 4;
      }
      break;



    case 4: //ライントレース　青検知
      setSpeedZone6();
      done = identifyColor2( 0 );
      if ( done == 1 ) {
        mode_G = 5;
      }
      break;


    case 5: //ライントレース　赤検知
      setSpeedZone6();
      done = identifyColor2( 1 );
      if ( done == 1 ) {
        zoneNumber_G = 8;
        mode_G = 0;
        azimthswitch = 0;

      }
      break;



    /*
        case 3: //ライントレース 赤検知
          linetracePID2(speed, kp, kd);
          done = identifyColor2( 1 );
          if ( done == 1 ) {
            mode_G = 4;
          }
          break;



        case 4: //ライントレース　青検知
          linetracePID2(speed, kp, kd);
          done = identifyColor2( 0 );
          if ( done == 1 ) {
            mode_G = 5;
          }
          break;


        case 5: //ライントレース　赤検知
          linetracePID2(speed, kp, kd);
          done = identifyColor2( 1 );
          if ( done == 1 ) {
            zoneNumber_G = 8;
            mode_G = 0;

          }
          break;
    */
    default:
      break;
  }

}


void  setSpeedZone6() {
  if (red_G > 60  && green_G > 60&& blue_G > 60 ) {
    motorR_G = 150;
    motorL_G = -150;
    if (zone6BeforeMotion != 1) {
      zone6BeforeMotion = 1;
      azimuth = averageHeading();

    }
  } else   if ( (  red_G < 25 && green_G < 25 && blue_G > 30 ) || (  red_G > 60 && green_G < 20 && blue_G < 20 ) ) {
   // if      (identifyColor2( 1 ) == 1 || identifyColor2( 0 ) == 1 ) {
    motorR_G = -200;
    motorL_G = 200;
    if (zone6BeforeMotion != 2) {
      zone6BeforeMotion = 2;
      azimuth = averageHeading();
    }
  } else {
    motorR_G = 130;
    motorL_G = 130;

    if (zone6BeforeMotion != 3) {
      zone6BeforeMotion = 3;
      azimuth = averageHeading();
    }
  }
}


int identifyColor2( int color )
{
  static int count = 0;

  if ( color == 0 && red_G < 40  && blue_G > 30 ) // 青検知
    ++count;
  else if ( color == 1 && red_G > 40 && green_G < 40 && blue_G < 40 ) // 赤検知
    ++count;
  else
    count = 0;

  if ( count > 1) {
    count = 0;
    return 1;
  }
  else
    return 0;
}


