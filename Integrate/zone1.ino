
//linecolar 60,10,6
/*
pointstart
27,55,62
end
56,56,57
1
6,13,6
2
63,78,62
3
70,23,24
4
13,18,51
5
2,5,22
branch
18,4,2
turn right
13,31,60
*/



int colorChartZone1[][3] = {
  {60, 10, 6},    //0  linecolor
  {27, 55, 62},   //1  pointstart
  {56, 56, 57},   //2  endzone
  {6, 13, 6},     //3  1
  {63, 78, 62},   //4  2
  {70, 23, 24},   //5  3
  {10, 14, 51},   //6  4
  {2, 5, 22},     //7  5
  {18, 4, 2},     //8  branch
  {13, 31, 60},   //9  turn right
  {0, 0, 0},      //10 outerline
  {100, 100, 100} //11 field
};

//0->left 1->right
int task_zone1[] = {1, 0};
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
      c = identifyZone1();

      if ( c == 0 ) {
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
      c = identifyZone1();
      if ( c == 11 ) {//白検知
        mode_G = 3;
        currentTask++;
      }
      break;

    case 3: //ライントレース pointstart(水色)検知
      linetracePID2(100, 5, 5);

      c = identifyZone1();
      if ( c == 1 ) {//pointstart検知
        mode_G = 4;
      }
      break;



    case 4: //ポイント処理
      motorR_G = 50;
      motorL_G = 50;

      c = identifyZone1();
      if (c >= 3 && c <= 7) { //ポイント送信
        Serial.print("Z");
        int point = c - 2;
        Serial.print(point);
      } else if (c == 2) { //escape
        //motorR_G = 0;
        //motorL_G = 0;
        //escapeFlag=1;
        if (colorChartZone1[2][0]>50&& colorChartZone1[2][0]<60) {
          mode_G = 0;
          zoneNumber_G = 8;
          azimthswitch = 0;
        }
        //return;
      } else if (c == 8) { //分岐検出
        if (task_zone1[currentTask] == 0) {
          pointTask = 1; //turn left
        } else {
          pointTask = 2; //turn right
        }
      } else if (c == 9) {
        pointTask = 3;
      } else if (c == 0 || c == 11) {
        mode_G = 5;
      }
      break;


    case 5: //ポイント後のタスク処理
      if (pointTask == 1 || pointTask == 0) {
        //motorR_G = 250;
        //motorL_G = 100;
        linetracePID2(100, 5, 5);
        c = identifyZone1();
        if ( c == 0 ) {
          if (pointTask == 1)currentTask++;
          pointTask = 0;
          mode_G = 3;
        }
      } else if (pointTask == 2 || pointTask == 3) {
        motorR_G = -100;
        motorL_G = 300;
        c = identifyZone1();
        if ( c == 0 ) {
          pointTask = 0;
          currentTask++;
          mode_G = 6;
        }
      }
      break;

    case 6:
      motorR_G = 100;
      motorL_G = 100;

      c = identifyZone1();
      if ( c == 11 ) {
        mode_G = 3;
      }

      break;

    default:
      break;
  }

}

int identifyZone1()
{
  static int count = 0; // この関数が初めて呼ばれた時にのみ初期化される
  static int  beforeColor = 0;
  float minDistance = 3000;
  int currentStatus = -1;
  float distance;

  for ( int i = 0; i < 12; ++i ) {
    distance = (red_G - colorChartZone1[i][0]) * (red_G - colorChartZone1[i][0])
               + (green_G - colorChartZone1[i][1]) * (green_G - colorChartZone1[i][1])
               + (blue_G - colorChartZone1[i][2]) * (blue_G - colorChartZone1[i][2]);
    if ( distance < minDistance ) {
      minDistance = distance;
      currentStatus = i;
    }
  }
  if (beforeColor == currentStatus)count++;
  else count=0;
  
  
  beforeColor=currentStatus;
  if (count > 5) {
    
    return currentStatus;
  }
  return -1;

}


