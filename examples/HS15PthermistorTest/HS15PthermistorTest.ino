#include <HS15P.h>
#include <SHthermistor.h>  // https://github.com/citriena/SHthermistor


// HS15P接続　　D11 --- HS15P ---- A1---- 0.022uF -----D12

// サーミスタ接続　Vdd---10kΩ---A0---NTC thermistor---GND


HS15P hs15p(11, 12, 1);

SHthermistor thermistor(0, 25, 50, 27280, 10000, 4160, 10000, 0, NTC_GND);  // SEMITEC株式会社103AT-11の場合

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}


void loop() {
  // put your main code here, to run repeatedly:
  float temp;
  float rh;

  temp = thermistor.readTemp();
  rh = hs15p.getRh(temp);

  Serial.print("temp = ");
  Serial.print(temp); 
  Serial.print("C, ");
  Serial.print(rh); 
  Serial.print("humidity = ");
  Serial.println("%");
  delay(1000);
}
