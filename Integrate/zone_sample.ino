void zone()
{
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
          mode_G = 6;

          //button.waitForButton();
          //zoneNumber_G = 8;
          //
          //mode_G = -2;
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
      mode_G = 0;
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




void zone4() {

  switch (state_fsm) {

    case 0: //左に回転,setup
      if (azimuth <= start_azimuth - 85) {
        motorDrive(LEFT, 100);
      }
      state_fsm = 1;

      break;

    case 1://右に回転しながら探索,180度探索したらcase2へ
      if (countPET == 3)//3本倒したらcase4(zoneToZone)へ
        state_fsm = 4;

      motorDrive(RIGHT, 25);
      if (steadyState(100) == 1) {
        state_fsm = 2;
      }
      break;


    case 2://距離測定
      motors.setSpeeds(0, 0);
      digitalWrite(trig, HIGH);
      delayMicroseconds(10);
      digitalWrite(trig, LOW);
      interval = pulseIn(echo, HIGH, 500000);//Echo 信号が HIGH である時間(μs)を計測
      L = C * interval / 2 * 0.0001;//距離
      //L = L - L % 1;

      if (L != 0 && findFlag == false) { //見つかったら距離と方向を保存
        distanceL = L;
        findFlag = true;
        state_fsm = 3;
      }

      else if (L == 0 && findFlag == true) {
        findFlag = false;
      }

      state_fsm = 1;
      break;


    case 3://倒しに行く
      motors.setSpeeds(100, 100);
      digitalWrite(trig, HIGH);
      delayMicroseconds(10);
      digitalWrite(trig, LOW);
      interval = pulseIn(echo, HIGH, 500000);//Echo 信号が HIGH である時間(μs)を計測
      L = C * interval / 2 * 0.0001;//距離
      //L = L - L % 1;

      if (L < 15 && approachFlag == false) {//15cmより近づいたら
        motors.setSpeeds(250, 250);
        approachFlag = true;
      }
      if (approachFlag == true) {
        if (steadyState(500) == 1) {//500ms進んで倒した
          approachFlag = false;
          countPET++;//倒した数
          state_fsm = 1;
        }
      }
      break;

    case 4:
      zoneNumber_G = 8;
      break;
  }
}

void zone5() {
  compass.read(); //値を読み込む
  //Serial.println(ratioX);
  ratioX = (compass.a.x + 100) / 150;
  switch (zone5Flag) {
    case 0:
      zone5Speed = 300;
      motors.setSpeeds(zone5Speed, zone5Speed);
      if (ratioX > 20) {
        zone5Flag = 1;
      }
      break;

    case 1:
      ratioY = (compass.a.y + 800) / 163;
      motors.setSpeeds(zone5Speed - ratioY, zone5Speed + ratioY);
      if (ratioX < 10)
        zone5Flag = 2;
      break;

    case 2:
      if (steadyState(2000) == 1) {//()ms進んだ
        motors.setSpeeds(0, 0);
      }
      else {}
      break;
    default:
      break;
  }
}
