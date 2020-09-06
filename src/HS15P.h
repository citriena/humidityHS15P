#ifndef HS15P_h
#define HS15P_h

#include <Arduino.h>

#define ANALOG_10US     362     // 1kΩ, 10usレンジのアナログ計測値。極力356に近くなるように充電時間を調整した結果、6usで362となった。
#define CON_F           2.2e-8  // HS15Pのインピーダンス測定用コンデンサの容量（容量の安定したフィルムコンデンサ等を使う）0.022uF
#define AI_THRESHOLD    128     // 湿度アナログ計測下限値　これ以下なら充電時間レンジを上げる。
#define HS15P_FACTOR    1.00    // HS15Pの個体差調整初期値（0.50-1.50くらい）。スケッチ内ではvoid setFactor(float factor);で設定可能。
                                // 実際より湿度が高く出る場合は値を上げる（インピーダンス計算値を大きくし、算出値を低湿度化）。

class HS15P { 
public:
  HS15P(int vfPin, int vrPin, int vaPin); // コンストラクター // vfPin --- HS15P ---- vaPin---- capacitor -----vrPin
  float getRh(float temp);
  float getVPD(float temp, float rh);
  void setFactor(float factor);
  float getFactor();

protected:

private:
  int _vfPin;
  int _vrPin;
  int _vaPin;
  float _hs15pFactor;
  const float _chargeTIME10US  = -log(1.0 - ((float)ANALOG_10US / 1023.0)) * ((float)CON_F * 1000.0); //計算上の充電時間

  void init(void);
  int readSensor(unsigned int chargeTime);

};
#endif
