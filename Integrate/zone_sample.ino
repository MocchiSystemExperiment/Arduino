void zone()
{
  //タイムアウト処理  
    int timeout = countTimeout( 20000 );
    if ( timeout == 1 ) {
      //reset variables
      mode_G = 0;
      zoneNumber_G = 8;
      return;
    }

  static int count = 0;
  int done;

  switch ( mode_G ) {
    case 0: // setupが必要ならここ（必要が無くても形式的に）
      mode_G = 1;
      break;
    case 1: // stop for 500 ms
      motorL_G = 0;
      motorR_G = 0;

      done = steadyState( 500 );
      if ( done == 1 ) {
        mode_G = 2;
      }
      break;
    case 2: // go straight for 500ms
      motorL_G = SPEED;
      motorR_G = SPEED;

      done = steadyState( 500 );
      if ( done == 1 ) {
        mode_G = 3;
      }
      break;
    case 3: // stop for 500 ms
      motorL_G = 0;
      motorR_G = 0;

      done = steadyState( 500 );
      if ( done == 1 ) {
        mode_G = 4;
      }
      break;
    // loop mode_G(4,5) (zoneNumber_G) times
    case 4: // turn left for 200 ms
      motorL_G = -SPEED;
      motorR_G = SPEED;

      done = steadyState( 200 );
      if ( done == 1 ) {
        mode_G = 5;
      }
      break;

    case 5: // stop for 200ms
      motorL_G = 0;
      motorR_G = 0;

      done = steadyState( 200 );
      if ( done == 1 ) {
        mode_G = 4;
        count++; //ループ回数カウント
        if (count >= zoneNumber_G) {
          //reset variables
          count = 0;
          //mode_G = 6;

          //button.waitForButton();
          //zoneNumber_G = 8;
          //
          mode_G = -2;
        }
      }
      break;


    case 6: // escape sequence(turn right in the direction when zumo started)
      int d;
      d = abs(startedDirection_G - (int)averageHeading());

      if (d > 90) { //差が大きければ素早く動く
        motorL_G = 200;
        motorR_G = -200;
      } else if (d > 15) { //差が15~90度ならゆっくり旋回
        motorL_G = 50;
        motorR_G = -50;
      } else {//誤差が15度以内なら脱出
        //reset variables
        motorL_G = 0;
        motorR_G = 0;
        count = 0;
        mode_G = 0;
        zoneNumber_G = 8;
      }

      break;
    //to next zone
    case -1:
      count = 0;
      zoneNumber_G = 8;
      break;

    default:
      //stop
      motorL_G = 0;
      motorR_G = 0;

      break;
  }
  //
  //button.waitForButton();
}


int steadyState( unsigned long period )
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
<<<<<<< HEAD
=======




>>>>>>> refs/remotes/origin/na3alf6
