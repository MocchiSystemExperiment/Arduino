const float colorzone4[9][3] = { // identifyZone()用の固定値
  {84.00, 80.00, 25.00},//黄色
  {26.00, 59.00, 68.00},//水色
  {57.00, 8.00, 2.00},//赤
  {13.00, 46.00, 18.00},//緑
  {2.00, 8.00, 34.00},//青(濃い)
  {10.00, 16.00, 47.00},//ソ(うすい)
  {100.00, 100.00, 100.00},//白
  {0.00, 0.00, 0.00},//黒
  {52.00, 19.00, 36.00}//ピンク
};


void zone4() {
  ///タイムアウト処理
  int timeout = countTimeout( 20004 );
  if ( timeout == 1 ) {
    //reset variables
    mode_G = 0;
    zoneNumber_G = 8;
    azimthswitch = 0;
    return;
  }

  int done;
  int iro;
  int zone4Azimuth = 209, zone4nowAzimuth;

  switch ( mode_G ) {
    case 0: // setupが必要ならここ（必要が無くても形式的に）
      mode_G = 1;
      break;
    /* case 1: // ピンクまで前進
       zone4nowAzimuth = averageHeading();
       if(zone4Azimuth - zone4nowAzimuth < 0){
       motorR_G = 60;
       motorL_G = 40;
     }
     else if(zone4Azimuth - zone4nowAzimuth > 0){
       motorR_G = 40;
       motorL_G = 60;
     }

       if ( red_G > 75 && green_G > 75 && blue_G > 75 )  // 黒を感知
       {
         motorR_G = 0;
         motorL_G = 0;
         mode_G = 2;
       }
       break;*/


    case 1: // ピンクを検知するまでライントレース
      linetracePID2(100, 1.2, 1.2);
      iro = identifyZone4();
      /*if(iro == 8){
        motorR_G = 0;
        motorL_G = 0;
        buzzer.play(">g32>>c32");
        mode_G = 2;
      }*/
      break;

      /*case 2://色を読み取りながら黒線まで進む
        iro = identifyZone4();
        Serial.println(iro);
        zone4nowAzimuth = averageHeading();
        if(zone4Azimuth - zone4nowAzimuth <= 0){
        motorR_G = 100;
        motorL_G = 80;
      }
      else if(zone4Azimuth - zone4nowAzimuth > 0){
        motorR_G = 80;
        motorL_G = 100;
      }
      if ( iro == 3 ){  // を感知
        mode_G = 3;
        motorR_G = 0;
        motorL_G = 0;
        buzzer.play(">g32>>c32");
      }*/
  }
}


// KNNで現在最も近い番号を返す
int identifyZone4()
{

  float minDistance = 999999999;
  static int iro;
  float distance;

  for ( int i = 0; i < 10; ++i ) {
    distance = (red_G - colorzone4[i][0]) * (red_G - colorzone4[i][0])
               + (green_G - colorzone4[i][1]) * (green_G - colorzone4[i][1])
               + (blue_G - colorzone4[i][2]) * (blue_G - colorzone4[i][2]);
    if ( distance < minDistance ) {
      minDistance = distance;
      iro = i;
    }
  }
  return iro;
}
