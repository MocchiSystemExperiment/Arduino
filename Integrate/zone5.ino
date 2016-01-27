
void zone5() {
//タイムアウト処理
  int timeout = countTimeout( 20005 );
  if ( timeout == 1 ) {
    //reset variables
    mode_G = 0;
    zoneNumber_G = 8;
    azimthswitch = 0;
    return;
  }
  
  zone_in = 1;
  compass.read(); //値を読み込む
  motors.setSpeeds(zone5SL - ratioY, zone5SR + ratioY);
  ratioX = (compass.a.x + 100) / 150;

  switch (zone5Flag) {
    case 0://直進
      zone5SL = 250;
      zone5SR = 270;
      if (steadyState(200) == 1) {
        if (ratioX >= 20) {
          zone5Flag = 1;
        }
      }
      break;

    case 1://登る
      zone5SL = 150;
      zone5SR = 150;
      ratioY = (compass.a.y + 800) / 81;
      if (steadyState(500) == 1) {
        if (ratioX < 20) {//頂上に着いたら
          zone5Flag = 2;
        }
      }
      break;

    case 2://真ん中でストップ
      // zone5SL = 100;
      //zone5SR = 100;
      //if (steadyState(30) == 1) {
      zone5SL = 0;
      zone5SR = 0;
      ratioY = 0;
      zone5Flag = 3;
      // }
      break;

    case 3:
      zone5SL = -150;
      zone5SR = 150;
      azimuth = averageHeading();
      if (80 < azimuth && azimuth < 100) { //向く方向を変更する
        buzzer.play(">g32>>c32");
        zone5Flag = 4;
      }
      break;

    case 4:
      zone5SL = 0;
      zone5SR = 0;
      buzzer.play(">g32>>c32");
      zone5Flag = 5;
      break;

    case 5:
      zone5SL = 150;
      zone5SR = -150;
      azimuth = averageHeading();
      if (260 < azimuth && azimuth < 280) {//降りる方向を変更する
        zone5Flag = 6;
      }
      break;

    case 6://降りる
      zone5SL = 100;
      zone5SR = 110;
      if (steadyState(500) == 1) {
        zone5Flag = 7;
      }
      break;
    case 7:
      if (ratioX > 0) { //下に着いたら
        zone5SL = 100;
        zone5SR = 100;
        zone5Flag = 8;
      }
      break;
    case 8:
      if (  identifyColor( 0 ) == 1 ) {
         buzzer.play(">g32>>c32");
        zone_in = 0;
        zoneNumber_G = 8;
      }
      break;

    default:
      break;
  }

}
