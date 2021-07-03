// とりあえず動作確認用
// 温度は25℃として処理

// 接続　　D11 --- HS15P ---- A1(D15)---- 0.022uF -----D12

#include <HS15P.h>  // https://github.com/citriena/humidityHS15P

HS15P hs15p(11, 12, A1, 15);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}


void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(hs15p.getRh(25), 1); 
  delay(1000);
}