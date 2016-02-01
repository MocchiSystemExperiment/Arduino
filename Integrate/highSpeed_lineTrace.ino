extern float minDistance; // identifyZone()用のグローバル変数


void highSpeed_LineTrace()
{
  int zoneNumber;
  int done;

  switch ( mode_G ) {
    case 0: // setupが必要ならここ（必要が無くても形式的に）
      motorL_G = 120;
      motorR_G = -120;
      int dif;
      dif =   startedDirection_G - averageHeading();
      if (abs(dif) < 10)mode_G = 1;

      break;

    //escape the zone
    case 1:  // 黒を検知するまで直進
      goStraight();
      done = identifyColor( 0 );
      if ( done == 1 ) {
        mode_G = 2;

        buzzer.play(">g32>>c32");
      }
      break;
    case 2://白を検知するまで旋回
      motorL_G = 200;
      motorR_G = -150;
      done = identifyColor( 1 );
      if ( done == 1 ) {
        mode_G = 3;

        buzzer.play(">g32>>c32");

      }
      break;
    case 3:// 黒と黄色の混合色を検知するまでライントレース
      linetracePID2(100, 1, 0.5);
      done = identifyColor( 2 );
      if ( done == 1 ) {
        mode_G = 4;

        buzzer.play(">g32>>c32");
      }
      break;
    case 4://白色を検知するまでゆっくり前進
      motorL_G = 50;
      motorR_G = 50;
      done = identifyColor( 1 );
      if ( done == 1 ) {
        mode_G = 5;
        buzzer.play(">g32>>c32");
      }
      break;

    case 5: // ライントレース（3000ms）
      linetracePID2(100, 1.0, 0.5);


      done = steadyState( 3000 );
      if ( done == 1 ) {
        mode_G = 6;
        buzzer.play(">g32>>c32");

      }
      break;




    case 6: // 直線（行き）
      linetracePID2(400, 0.3, 0 );

      done = steadyState( 5000 );
      if ( done == 1 ) {
        mode_G = 7;
        buzzer.play(">g32>>c32");

      }
      break;

    case 7: // ライントレース（５ｓ）
      linetracePID2(100, 1.0, 0.5);


      done = steadyState( 5000 );
      if ( done == 1 ) {
        mode_G = 8;
        buzzer.play(">g32>>c32");

      }
      break;

    case 8: //直線（帰り）
      linetracePID2(400, 0.3, 0);


      done = steadyState( 4000 );
      if ( done == 1 ) {
        mode_G = 9;
        buzzer.play(">g32>>c32");

      }
      break;






    //from the previous zone to the next zone
    case 9: // ライントレース（黄と黒の混合色を検知するまで）
      linetracePID2(120, 1.0, 1.5);


      done = identifyColor( 2 );
      if ( done == 1 ) {
        mode_G = 10;

      }
      break;
    case 10: // 黒を検知するまで直進
      motorR_G = 50;
      motorL_G = 50;
      done = identifyColor( 0 );
      if ( done == 1 )
        mode_G = 11;

      break;
    case 11: // identifyZone()のsetup
      minDistance = 9999999;//色差設定
      mode_G = 12;

      break;
    case 12: // 白を検知するまで直進（その間ゾーン番号を検知）
      motorR_G = 50;
      motorL_G = 50;
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

