// �Ƃ肠��������m�F�p
// ���x��25���Ƃ��ď���

// �ڑ��@�@D11 --- HS15P ---- A1---- 0.022uF -----D12

#include <HS15P.h>

HS15P hs15p(11, 12, 1);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}


void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(hs15p.getRh(25), 1); 
  delay(1000);
}}