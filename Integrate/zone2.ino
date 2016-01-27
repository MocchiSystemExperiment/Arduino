<<<<<<< HEAD
void zone2() {//棒倒しゾーン

  //タイムアウト処理
  int timeout = countTimeout( 20002 );
  if ( timeout == 1 ) {
    //reset variables
    mode_G = 0;
    zoneNumber_G = 8;
    azimthswitch = 0;
    return;
  }


  
=======

void zone2() {
>>>>>>> refs/remotes/origin/na3alf6
  zone_in = 1;
  //Serial.println(start_azimuth);
  motors.setSpeeds(zone2SL, zone2SR);
  switch (state_fsm) {

    case 0: //setup
      start_azimuth = averageHeading();
      if (start_azimuth < 90) {
        start_azimuth = start_azimuth + 360;
      }
      pinMode(trig, OUTPUT);
      pinMode(echo, INPUT);
      pinMode(power, OUTPUT);
      digitalWrite(power, HIGH); //電源ON
      static bool skip = true;
      state_fsm = 1;
      break;

    case 1://左に回転
      azimuth = averageHeading();
      zone2SL = -200;
      zone2SR = 200;
      if (azimuth <= start_azimuth - 90) {
        state_fsm = 2;
      }
      break;

    case 2://右に回転しながら探索,case3へ
      //Serial.println(state_fsm);
<<<<<<< HEAD
=======
      if (countPET == 3)//3本倒したらcase7(zoneToZone)へ
        state_fsm = 7;

>>>>>>> refs/remotes/origin/na3alf6
      zone2SL = 100;
      zone2SR = -100;
      if (steadyState(100) == 1) {
        state_fsm = 3;
      }
      break;


    case 3://距離測定
      zone2SL = 0;
      zone2SR = 0;
      if (skip == true) { //１回目の測定をスキップ
        skip = false;
      }
      else {
        digitalWrite(trig, HIGH);
        delayMicroseconds(10);
        digitalWrite(trig, LOW);
        interval = pulseIn(echo, HIGH, 3000);//18000μsで306㎝測定できる
        L = C * interval / 10000 / 2; //距離
        Serial.println(L);
        //L = L - L % 1;

        if (L == 0) {
          digitalWrite(power, LOW); //電源OFF
          delay(10); //ちょっと待機
          digitalWrite(power, HIGH); //電源ON
          skip = true;
          state_fsm = 2;
        }
        else if (L != 0) {
          countOnePET++;
          if (countOnePET > 3) {
            countOnePET = 0;
            state_fsm = 4;
          }
          else
            state_fsm = 2;
        }
      }
      break;

    case 4://黒を検知するまで直進
      zone2SR = 200;
      zone2SL = 200;
      if (  identifyColor( 0 ) == 1 || steadyState(1500) == 1) {
        state_fsm = 5;
      }
      break;

    case 5:
      zone2SL = -100;
      zone2SR = -100;
      countPET++;
      if (steadyState(1000) == 1) {//ms進む
        state_fsm = 2;
      }
      break;

    case 6://黒が見つかるまで直進
      zone2SL = 100;
      zone2SR = 100;
      if (  identifyColor( 0 ) == 1 ) {
        state_fsm = 7;
      }
      break;

    case 7:
      zone_in = 0;
      zoneNumber_G = 8;
      break;
  }
}
<<<<<<< HEAD
=======

>>>>>>> refs/remotes/origin/na3alf6
