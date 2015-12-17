int colorChartZone1[][3] = {
  {0, 0, 0},      //0 outerline
  {100, 100, 100}, //1 field
  {60, 10, 6},    //2  linecolor
  {27, 55, 62},   //3  pointstart
  {56, 56, 57},   //4  endzone
  {6, 13, 6},     //5  1
  {63, 78, 62},   //6  2
  {70, 23, 24},   //7  3
  {10, 14, 51},   //8  4
  {2, 5, 22},     //9  5
  {18, 4, 2},     //10  branch
  {13, 31, 60},   //11  turn right
  {100, 100, 100}, //12 field
  {60, 10, 6},    //13  linecolor
};

//0->left 1->right
int task_zone1[] = {0, 0, 1, 0};
int task_num = 4;
int currentTask = 0;

int pointTask = 0;
int escapeFlag = 0;


void zone1() {

  //タイムアウト処理
  /*int timeout = countTimeout( 20000 );
  if ( timeout == 1 ) {
    //reset variables
    mode_G = 0;
    zoneNumber_G = 8;
    azimthswitch = 0;
    return;
  }*/

  int c;

  //escape
  /*if(escapeFlag==1 && red_G>95&&green_G>95&& blue_G>95){
    mode_G = 0;
    zoneNumber_G = 8;
    azimthswitch = 0;
  }else{
    escapeFlag=0;
  }
  */

  switch ( mode_G ) {
    case 0: // setupが必要ならここ（必要が無くても形式的に）
      mode_G = 1;
      azimthswitch = 1;//方角測定OFF
      break;
    case 1: // 赤まで前進
      motorR_G = 80;
      motorL_G = 80;
      c = identifyZone1(0, 2);

      if ( c == 2 ) {
        mode_G = 2;
      }
      break;


    case 2: // 白を検知するまで旋回
      if (task_zone1[currentTask] == 0) {//左旋回
        motorR_G = 200;
        motorL_G = 0;

      } else {//右旋回
        motorR_G = -200;
        motorL_G = 200;
      }
      c = identifyZone1(0, 2);
      if ( c == 1 ) {//白検知
        mode_G = 3;
        currentTask++;
      }
      break;

    case 3: //ライントレース pointstart(水色)検知
      linetracePID2(80, 5, 3);

      c = identifyZone1(0, 3);
      if ( c == 3 ) {//pointstart検知
        mode_G = 4;
      }
      break;



    case 4: //ポイント処理
      motorR_G = 50;
      motorL_G = 75;

      c = identifyZone1(3, 9);
      if (c >= 5 && c <= 9) { //ポイント送信
        int point = c - 4;
        for(int i=0;i<10;i++){
          Serial.write('Z');    
          Serial.print(point);
        }
        mode_G = 5;
      } else if (c == 4) { //escape
      motors.setSpeeds(-50,-50);
      delay(500);
        mode_G = 0;
        zoneNumber_G = 8;
        azimthswitch = 0;
        return ;
      }
      break;
      
    case 5: //ポイント後のタスク処理
      c = identifyZone1(5, 13);
      if (c == 10) { //分岐検出
        if (task_zone1[currentTask] == 0) {
          pointTask = 1; //turn left
          mode_G = 6;
          
        } else {
          pointTask = 2; //turn right
          mode_G = 6;
        }
      } else if (c == 11) {
        pointTask = 2;
        mode_G = 6;
      }
      else if(c==12||c==13){
        mode_G=6;
      }
      break;
      
      case 6://ポイントからの脱出
      if (pointTask == 1 || pointTask == 0) {
        motorR_G = 250;
        motorL_G = 50;
        //linetracePID2(100, 3, 3);
        c = identifyZone1(9,13);
        if ( c == 12||c==13 ) {
          if (pointTask == 1)currentTask++;
          pointTask = 0;
          mode_G = 3;
        }
      } else  {
        motorR_G = 50;
        motorL_G = 250;
        c = identifyZone1(9,13);
        if ( c==13 ) {
          if(pointTask==2)currentTask++;
          pointTask = 0;
          mode_G = 7;
        }
      }
      break;

    case 7:
      motorR_G = 100;
      motorL_G = 100;
      c = identifyZone1(0,2);
      if ( c ==  1) {
        mode_G = 8;
      }
      break;
      
    case 8:
      motorR_G = 100;
      motorL_G = -100;
      c = identifyZone1(0,2);
      if ( c ==  2) {
        mode_G = 3;
      }
      break;

    default:
      break;
  }

}

int identifyZone1(int begin, int end)
{
  static int count = 0; // この関数が初めて呼ばれた時にのみ初期化される
  static int  beforeColor = 0;
  float minDistance = 3000;
  int currentStatus = -1;
  float distance;

  for ( int i = begin; i <= end; ++i ) {
    distance = (red_G - colorChartZone1[i][0]) * (red_G - colorChartZone1[i][0])
               + (green_G - colorChartZone1[i][1]) * (green_G - colorChartZone1[i][1])
               + (blue_G - colorChartZone1[i][2]) * (blue_G - colorChartZone1[i][2]);
    if ( distance < minDistance ) {
      minDistance = distance;
      currentStatus = i;
    }
  }
  if (beforeColor == currentStatus)count++;
  else count = 0;


  beforeColor = currentStatus;
  if (count > 5) {

    return currentStatus;
  }
  return -1;

}


