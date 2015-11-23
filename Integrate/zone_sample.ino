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




void zone4() {
  zone_in = 1;
  Serial.println(start_azimuth);
  motors.setSpeeds(zone4SL, zone4SR);
  switch (state_fsm) {

    case 0: //setup
      Serial.println(state_fsm);
      start_azimuth = averageHeading();

      pinMode(power, OUTPUT);
      digitalWrite(power, HIGH); //電源ON
      static bool skip = true;
      state_fsm = 1;
      break;

    case 1://左に回転
      Serial.println(state_fsm);
      azimuth = averageHeading();
      zone4SL = -200;
      zone4SR = 200;
      if (azimuth <= start_azimuth - 85) {
        state_fsm = 2;
      }
      break;

    case 2://右に回転しながら探索,case3へ
      Serial.println(state_fsm);
      if (countPET == 3)//3本倒したらcase4(zoneToZone)へ
        state_fsm = 5;

      zone4SL = 80;
      zone4SR = -80;
      if (steadyState(200) == 1) {
        state_fsm = 3;
      }
      break;


    case 3://距離測定
      zone4SL = 0;
      zone4SR = 0;
      Serial.println(state_fsm);
      if (skip == true) { //１回目の測定をスキップ
        skip = false;
      }
      else {
        Serial.println(state_fsm);
        if (L == 0) {
          digitalWrite(power, LOW); //電源OFF
          delay(10); //ちょっと待機
          digitalWrite(power, HIGH); //電源ON
          skip = true;
        }
      }
      digitalWrite(trig, HIGH);
      delayMicroseconds(10);
      digitalWrite(trig, LOW);
      interval = pulseIn(echo, HIGH, 5000);//18000μsで306㎝測定できる
      L = C * interval / 2 * 0.0001;//距離
      //L = L - L % 1;

      if (L != 0 && findFlag == false) { //3回同じところで見つかったら距離と方向を保存
        countOnePET++;
        if (countOnePET > 3) {
          countOnePET = 0;
          distanceL = L;
          findFlag = true;
          state_fsm = 4;
        }
      }

      else if (L == 0 && findFlag == true) {
        countOnePET = 0;
        findFlag = false;
      }
      if (steadyState(200) == 1) {
        state_fsm = 2;
      }
      break;


    case 4://倒しに行く
      zone4SL = 100;
      zone4SR = 100;
      digitalWrite(trig, HIGH);
      delayMicroseconds(10);
      digitalWrite(trig, LOW);
      interval = pulseIn(echo, HIGH, 500000);//Echo 信号が HIGH である時間(μs)を計測
      L = C * interval / 2 * 0.0001;//距離
      //L = L - L % 1;

      if (L < 15 ) { //15cmより近づいたら
        countOnePET++;
        if (countOnePET > 3 && approachFlag == false) {
          zone4SR = 250;
          zone4SL = 250;
          approachFlag = true;
        }
      }
      if (approachFlag == true) {
        if (steadyState(500) == 1) {//500ms進んで倒した
          approachFlag = false;
          countPET++;//倒した数
          state_fsm = 1;
        }
      }
      break;

    case 5:
      zoneNumber_G = 8;
      zone_in = 0;
      break;
  }
}

void zone5() {
  zone_in = 1;
  compass.read(); //値を読み込む
  motors.setSpeeds(zone5SL - ratioY, zone5SR + ratioY);
  ratioX = (compass.a.x + 100) / 150;

  switch (zone5Flag) {
    case 0:
      zone5SL = 200;
      zone5SR = 200;
      if (steadyState(200) == 1) {
        if (ratioX >= 20) {
          zone5Flag = 1;
        }
      }
      break;

    case 1:

      zone5SL = 200;
      zone5SR = 200;
      ratioY = (compass.a.y + 800) / 81;
      if (steadyState(500) == 1) {
        if (ratioX < 20) {
          zone5Flag = 2;
        }
      }
      break;

    case 2:

      if (steadyState(100) == 1) {
        zone5SL = 0;
        zone5SR = 0;
        ratioY = 0;
        start_azimuth = averageHeading();
        zone5Flag = 3;
      }
      break;

    case 3:
      zone5SL = -150;
      zone5SR = 150;
      azimuth = averageHeading();
      //Serial.println();
      if (abs(start_azimuth - azimuth) >= 90) {//向く方向を変更する
        zone5Flag = 4;
      }
      break;

    case 4:
      zone5SL = 0;
      zone5SR = 0;
      start_azimuth = averageHeading();
      zone5Flag = 5;
      break;

    case 5:
      zone5SL = 150;
      zone5SR = -150;
      azimuth = averageHeading();
      if (abs(start_azimuth - azimuth) >= 90) {//降りる方向を変更する
        zone5Flag = 6;
      }
      break;

    case 6://zoneToZoneに入らないバグ
      zoneNumber_G = 8;
      zone_in = 0;
      break;
    default:
      break;
  }

}
