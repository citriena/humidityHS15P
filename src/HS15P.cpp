/*********************************************************************

 HS15Pを使った湿度測定用Arduinoライブラリ
 
 本ライブラリはライセンスフリーです。

                               2021 citriena
                               https://github.com/citriena/humidityHS15P

再配布については下記の著作者の承諾を得る必要があります。
（本ソフトウェアの湿度センサHS15P測定ルーチンは下記の検討結果を基にしています）

    HS15P 湿度センサーの簡易動作実験
    渡波 郁
    http://siesta.la.coocan.jp/zk/hs15/hs15.html
	

参考資料

本ライブラリ開発のために下記の資料を参考にしました。

・HS15P 湿度センサーの簡易動作実験
  http://siesta.la.coocan.jp/zk/hs15/hs15.html

・HS15P 湿度センサーの簡易動作実験 Arduino版
  http://siesta.la.coocan.jp/zk/hs15/hs15_arduino.html

・Arduino温湿度測定ロガー・湿度センサーの比較
  https://bokunimo.net/arduino/humidity.html

*********************************************************************/

#include <HS15P.h>

#define CONVERSION_3    // 温度、HS15Pの抵抗値から湿度を算出する計算式；４種類から選択

///////////////////////////////////////////////
//  コンストラクタ
///////////////////////////////////////////////

HS15P::HS15P(int16_t vfPin, int16_t vrPin, int16_t vaPin, int16_t vdPin) :
  _vfPin(vfPin), 
  _vrPin(vrPin),
  _vaPin(vaPin),
  _vdPin(vdPin)
{
  _hs15pFactor = HS15P_FACTOR;
}


///////////////////////////////////////////////
//  個体補正用factor値の設定
///////////////////////////////////////////////

void HS15P::setFactor(float factor) {
  _hs15pFactor = factor;
}


///////////////////////////////////////////////
//  個体補正用factor値の取得
///////////////////////////////////////////////

float HS15P::getFactor() {
  return _hs15pFactor;
}


///////////////////////////////////////////////
//  HS15Pの初期化
///////////////////////////////////////////////

void HS15P::init(void) {
  pinMode(_vfPin, OUTPUT);
  digitalWrite(_vfPin, LOW);
  pinMode(_vrPin, OUTPUT);
  digitalWrite(_vrPin, LOW);
  pinMode(_vaPin, INPUT);
}


////////////////////////////////////////////////////////////////////////////////////////
//  HS15Pの抵抗値読出 chargeTime (us) コンデンサ充電後 vaPinの読み出し値(0-1023)を返す。
////////////////////////////////////////////////////////////////////////////////////////

int HS15P::readSensor(const unsigned int chargeTime) {
  int16_t i, j;
  int16_t remainCharge;

  init();
  remainCharge = analogRead(_vaPin);
  /* 測定前に完全に放電する。*/
  for (i = 0; i < 20; i++) {
    pinMode(_vdPin, OUTPUT);
    digitalWrite(_vdPin, LOW); // 荷電解消対策で計測用ピンを強制的にLOWにする。
    delay( 10 );
    pinMode(_vaPin, INPUT);
    remainCharge = analogRead(_vaPin);
#ifdef DEBUG
    Serial.print(remainCharge, 1);
    Serial.print (", ");
#endif
    if (remainCharge < 1) break;
  }
  if ( remainCharge > 4 ) {
    return (9999);
  }

  /* 充電時間を指定し、インピーダンス測定 */
  noInterrupts();
  if ( chargeTime == 10 ) {
    digitalWrite(_vfPin, HIGH);
    delayMicroseconds( 6 );				  // 充電 9.4us だが、ダミー抵抗を使ってdelay時間を調整
    pinMode(_vfPin, INPUT);
    j = analogRead(_vaPin);
    digitalWrite(_vfPin, LOW);
    digitalWrite(_vrPin, HIGH);
    delayMicroseconds( 6 );         // 荷電解消対策で反転充電し、疑似交流化
    digitalWrite(_vrPin, LOW);
  } else if ( chargeTime == 100 ) {
    digitalWrite(_vfPin, HIGH);
    delayMicroseconds( 94 );        // 充電 94us だが、ダミー抵抗を使ってdelay時間を調整
    pinMode(_vfPin, INPUT);
    j = analogRead(_vaPin);
    digitalWrite(_vfPin, LOW);
    digitalWrite(_vrPin, HIGH);
    delayMicroseconds( 94 );        // 荷電解消対策で反転充電し、疑似交流化
    digitalWrite(_vrPin, LOW);
  } else if ( chargeTime == 1000 ) {
    digitalWrite(_vfPin, HIGH);
    delayMicroseconds( 990 );       // 充電 940us だが、ダミー抵抗を使ってdelay時間を調整
    pinMode(_vfPin, INPUT);
    j = analogRead(_vaPin);
    digitalWrite(_vfPin, LOW);
    digitalWrite(_vrPin, HIGH);
    delayMicroseconds( 990 );       // 荷電解消対策で反転充電し、疑似交流化
    digitalWrite(_vrPin, LOW);
  } else if ( chargeTime == 10000 ) {
    digitalWrite(_vfPin, HIGH);
    delayMicroseconds( 10100 );     // 充電 9.4ms だが、ダミー抵抗を使ってdelay時間を調整
    pinMode(_vfPin, INPUT);
    j = analogRead(_vaPin);
    digitalWrite(_vfPin, LOW);
    digitalWrite(_vrPin, HIGH);
    delayMicroseconds( 10100 );	    // 荷電解消対策で反転充電し、疑似交流化
    digitalWrite(_vrPin, LOW);
  } else {
    digitalWrite(_vfPin, HIGH);
    delay( 101 );                   // 充電 100ms　はたしてこのdelayはちゃんと動作するのか
    pinMode(_vfPin, INPUT);
    j = analogRead(_vaPin);
    digitalWrite(_vfPin, LOW);
    digitalWrite(_vrPin, HIGH);
    delay( 101 );                   // 荷電解消対策で反転充電し、疑似交流化
    digitalWrite(_vrPin, LOW);
  }
  interrupts();
  init();                           //HS15Pのピン初期化。なぜかこれが必要。
  return j;
}


////////////////////////////////////////////////////
//  HS15Pの抵抗値を測定し、変換式で湿度算出して返す
////////////////////////////////////////////////////

float HS15P::getRh(const float temp) {
  int16_t i, j, t;
  float Rs;                 // HS15Pの抵抗値（10us基準）
  float Rl;                 // HS15Pの抵抗値（対数）
  float rh;                 // 算出した湿度

  t = 10;                   // 最初は10uSec充電での測定から開始する
  for (i = 0; i < 5; i++) {
    j = readSensor(t);
#ifdef DEBUG
    Serial.print("t=");
    Serial.print(t);
    Serial.print(", ");
    Serial.print("j=");
    Serial.print(j);
    Serial.print(", ");
#endif
    if ((j == 9999) || isnan(j) || isinf(j)) {       //コンデンサの放電にトラブル時
      return (0);
    }
//  計測値から湿度への変換
    Rs = -(_chargeTIME10US) / CON_F / log(1.0 - (float)j / 1024.0); // 充電された電圧から算出したHS15P抵抗値（10uSレンジ基準）
    Rs *= _hs15pFactor;                                             // HS15Pの個体差調整
    Rl = log10(Rs) + (float)i;                                      // 充電時間レンジ補正したHS15P抵抗対数値
#ifdef DEBUG
    Serial.print("Rl=");
    Serial.print(Rl, 2);
    Serial.print(", ");
#endif
    if ( j > AI_THRESHOLD) {  //充電後の電圧が閾値以上なら測定終了
      break;
    }
    t *= 10;                  // 充電後の電圧が閾値以下の場合は、充電時間（レンジ）を10倍して再測定
  }


#ifdef CONVERSION_1

//  データシートの対数値からのリニアリティ補正および温度補正を浮動小数点、３次関数まで使う力業版。湿度20％以下は使えない。
  if ((i < 4) or ((-0.06 * temp + 9.04) > Rl)) {            // 湿度20％以下は補正近似式が対応していない。
    float Pa = 0.00001373 * pow(temp, 3) - 0.001204 * pow(temp, 2) + 0.06165 * temp + 2.0327;
    float Pb = 0.0001802 * pow(temp, 3) - 0.01709 * pow(temp, 2) + 0.6645 * temp + 40.97;
    float Pc = 0.0005950 * pow(temp, 3) - 0.05359 * pow(temp, 2) + 0.8408 * temp + 221.8;
    rh = Pa * pow(Rl, 2) - Pb * Rl + Pc;
  } else {
     rh = -1;
  }
#endif

#ifdef CONVERSION_2

// 飽和塩法によって5，15, 25, 35℃で実測した結果から作成した変換式 (5個のセンサの出力対数値の平均値を使った)
  float Pa = -0.0025 * pow(temp, 2) + 0.097 * temp + 1.122;
  float Pb = -0.0179 * pow(temp, 2) + 0.7607 * temp + 35.147;
  float Pc = -0.0304 * pow(temp, 2) + 0.722 * temp + 204;
  rh = Pa * pow(Rl, 2) - Pb * Rl + Pc;
#endif

#ifdef CONVERSION_3

// 飽和塩法によって5，15, 25, 30℃で実測した結果から作成した変換式 (特性の近い3個のセンサの出力対数値の平均値を使った)
  float Pa = -0.0007 * pow(temp, 2) + 0.0556 * temp + 1.2651;
  float Pb = -0.0028 * pow(temp, 2) + 0.3813 * temp + 37.031;
  float Pc = -0.0036 * pow(temp, 2) + 0.0457 * temp + 206.91;
  rh = Pa * pow(Rl, 2) - Pb * Rl + Pc;
#endif

#ifdef CONVERSION_4

//  「HS15P 湿度センサーの簡易動作実験 Arduino版」(http://siesta.la.coocan.jp/zk/hs15/hs15_arduino.html)
//  での変換式（本プログラムに合わせるため、一部変数名を変更）

  float R;  // HS15の抵抗の測定値

  R= Rl + temp*(0.008363*Rl + 0.007695);       //温度補正　       ※補正値はデータシートのグラフより目測にて推定
  rh= 1.998 * R*R -40.74 *R +222.13;           //リニアリティ補正 ※補正値はデータシートのグラフより目測にて推定
#endif


  if (rh > 100.0) rh = 100.0;
  if ((rh < 0.0) || isnan(rh) || isinf(rh)) {
    rh = 0;
  }


#ifdef DEBUG
  Serial.print("RH=");
  Serial.print( rh , 1 );
  Serial.print("%, TEMP=");
  Serial.print( temp , 1 );
  Serial.print("C, VPD=");
  Serial.print( getVPD(temp, rh), 1);
  Serial.println("hPa");
#endif

  return rh;
}


////////////////////////////////////////////////////
//  飽差（vapor pressure deficit）を算出して返す
////////////////////////////////////////////////////

float HS15P::getVPD(float temp, float rh) { // hPa
  float vpd;   // Vapour-pressure deficit
  float vSVP;  // saturation vapor pressure
//  vSVP = 0.6112 * exp((17.67 * temp) / (243.5 + temp));  // hPa by Bolton (1980)
  vSVP = 6.1078 * pow(10, (7.5 * temp) / (237.3 + temp)); // hPa by Tetens (1930)
  vpd = vSVP * (1.0 - (rh/100.0));
  if (vpd < 0) vpd = 0;
  return vpd;
}
