/*********************************************************************

 HS15P���g�������x����pArduino���C�u����
 
 �{���C�u�����̓��C�Z���X�t���[�ł��B

                               (c) 2020 citriena
                               https://github.com/citriena/hs15p

�Ĕz�z�ɂ��Ă͉��L�̒���҂̏����𓾂�K�v������܂��B
�i�{�\�t�g�E�F�A�̎��x�Z���THS15P���胋�[�`���͉��L�̌������ʂ���ɂ��Ă��܂��j

    HS15P ���x�Z���T�[�̊ȈՓ������
    �n�g ��
    http://siesta.la.coocan.jp/zk/hs15/hs15.html
	

�Q�l����

�{���C�u�����J���̂��߂ɉ��L�̎������Q�l�ɂ��܂����B

�EHS15P ���x�Z���T�[�̊ȈՓ������
  http://siesta.la.coocan.jp/zk/hs15/hs15.html

�EHS15P ���x�Z���T�[�̊ȈՓ������ Arduino��
  http://siesta.la.coocan.jp/zk/hs15/hs15_arduino.html

�EArduino�����x���胍�K�[�E���x�Z���T�[�̔�r
  https://bokunimo.net/arduino/humidity.html

*********************************************************************/

#include <HS15P.h>

#define CONVERSION_3


///////////////////////////////////////////////
//  �R���X�g���N�^
///////////////////////////////////////////////

HS15P::HS15P(int vfPin, int vrPin, int vaPin) :
  _vfPin(vfPin), 
  _vrPin(vrPin),
  _vaPin(vaPin)
{
  _hs15pFactor = HS15P_FACTOR;
}


///////////////////////////////////////////////
//  �̕␳�pfactor�l�̐ݒ�
///////////////////////////////////////////////

void HS15P::setFactor(float factor) {
  _hs15pFactor = factor;
}


///////////////////////////////////////////////
//  �̕␳�pfactor�l�̎擾
///////////////////////////////////////////////

float HS15P::getFactor() {
  return _hs15pFactor;
}


///////////////////////////////////////////////
//  HS15P�̏�����
///////////////////////////////////////////////

void HS15P::init(void) {
  pinMode(_vfPin, OUTPUT);
  digitalWrite(_vfPin, LOW);
  pinMode(_vrPin, OUTPUT);
  digitalWrite(_vrPin, LOW);
  pinMode(_vaPin, INPUT);
}


////////////////////////////////////////////////////////////////////////////////////////
//  HS15P�̒�R�l�Ǐo chargeTime (us) �R���f���T�[�d�� vaPin�̓ǂݏo���l(0-1023)��Ԃ��B
////////////////////////////////////////////////////////////////////////////////////////

int HS15P::readSensor(const unsigned int chargeTime) {
  int i, j;
  int remainCharge;

  init();
  remainCharge = analogRead(_vaPin);
  /* ����O�Ɋ��S�ɕ��d����B*/
  for (i = 0; i < 20; i++) {
    pinMode(_vaPin, OUTPUT);
    digitalWrite(_vaPin, LOW); // �דd�����΍�Ōv���p�s���������I��LOW�ɂ���B
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

  /* �[�d���Ԃ��w�肵�A�C���s�[�_���X���� */
  noInterrupts();
  if ( chargeTime == 10 ) {
    digitalWrite(_vfPin, HIGH);
    delayMicroseconds( 6 );				  // �[�d 9.4us �����A�_�~�[��R���g����delay���Ԃ𒲐�
    pinMode(_vfPin, INPUT);
    j = analogRead(_vaPin);
    digitalWrite(_vfPin, LOW);
    digitalWrite(_vrPin, HIGH);
    delayMicroseconds( 6 );         // �דd�����΍�Ŕ��]�[�d���A�^���𗬉�
    digitalWrite(_vrPin, LOW);
  } else if ( chargeTime == 100 ) {
    digitalWrite(_vfPin, HIGH);
    delayMicroseconds( 94 );        // �[�d 94us �����A�_�~�[��R���g����delay���Ԃ𒲐�
    pinMode(_vfPin, INPUT);
    j = analogRead(_vaPin);
    digitalWrite(_vfPin, LOW);
    digitalWrite(_vrPin, HIGH);
    delayMicroseconds( 94 );        // �דd�����΍�Ŕ��]�[�d���A�^���𗬉�
    digitalWrite(_vrPin, LOW);
  } else if ( chargeTime == 1000 ) {
    digitalWrite(_vfPin, HIGH);
    delayMicroseconds( 990 );       // �[�d 940us �����A�_�~�[��R���g����delay���Ԃ𒲐�
    pinMode(_vfPin, INPUT);
    j = analogRead(_vaPin);
    digitalWrite(_vfPin, LOW);
    digitalWrite(_vrPin, HIGH);
    delayMicroseconds( 990 );       // �דd�����΍�Ŕ��]�[�d���A�^���𗬉�
    digitalWrite(_vrPin, LOW);
  } else if ( chargeTime == 10000 ) {
    digitalWrite(_vfPin, HIGH);
    delayMicroseconds( 10100 );     // �[�d 9.4ms �����A�_�~�[��R���g����delay���Ԃ𒲐�
    pinMode(_vfPin, INPUT);
    j = analogRead(_vaPin);
    digitalWrite(_vfPin, LOW);
    digitalWrite(_vrPin, HIGH);
    delayMicroseconds( 10100 );	    // �דd�����΍�Ŕ��]�[�d���A�^���𗬉�
    digitalWrite(_vrPin, LOW);
  } else {
    digitalWrite(_vfPin, HIGH);
    delay( 101 );                   // �[�d 100ms�@�͂����Ă���delay�͂����Ɠ��삷��̂�
    pinMode(_vfPin, INPUT);
    j = analogRead(_vaPin);
    digitalWrite(_vfPin, LOW);
    digitalWrite(_vrPin, HIGH);
    delay( 101 );                   // �דd�����΍�Ŕ��]�[�d���A�^���𗬉�
    digitalWrite(_vrPin, LOW);
  }
  interrupts();
  init();                           //HS15P�̃s���������B�Ȃ������ꂪ�K�v�B
  return j;
}


////////////////////////////////////////////////////
//  HS15P�̒�R�l�𑪒肵�A�ϊ����Ŏ��x�Z�o���ĕԂ�
////////////////////////////////////////////////////

float HS15P::getRh(const float temp) {
  int i, j, t;
  float Rs;                 // HS15P�̒�R�l�i10us��j
  float Rl;                 // HS15P�̒�R�l�i�ΐ��j
  float rh;                 // �Z�o�������x

  t = 10;                   // �ŏ���10uSec�[�d�ł̑��肩��J�n����
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
    if ((j == 9999) || isnan(j) || isinf(j)) {       //�R���f���T�̕��d�Ƀg���u����
      return (0);
    }
//  �v���l���玼�x�ւ̕ϊ�
    Rs = -(_chargeTIME10US) / CON_F / log(1.0 - (float)j / 1024.0); // �[�d���ꂽ�d������Z�o����HS15P��R�l�i10uS�����W��j
    Rs *= _hs15pFactor;                                             // HS15P�̌̍�����
    Rl = log10(Rs) + (float)i;                                      // �[�d���ԃ����W�␳����HS15P��R�ΐ��l
#ifdef DEBUG
    Serial.print("Rl=");
    Serial.print(Rl, 2);
    Serial.print(", ");
#endif
    if ( j > AI_THRESHOLD) {  //�[�d��̓d����臒l�ȏ�Ȃ瑪��I��
      break;
    }
    t *= 10;                  // �[�d��̓d����臒l�ȉ��̏ꍇ�́A�[�d���ԁi�����W�j��10�{���čđ���
  }


#ifdef CONVERSION_1

//  �f�[�^�V�[�g�̑ΐ��l����̃��j�A���e�B�␳����щ��x�␳�𕂓������_�A�R���֐��܂Ŏg���͋ƔŁB���x20���ȉ��͎g���Ȃ��B
  if ((i < 4) or ((-0.06 * temp + 9.04) > Rl)) {            // ���x20���ȉ��͕␳�ߎ������Ή����Ă��Ȃ��B
    float Pa = 0.00001373 * pow(temp, 3) - 0.001204 * pow(temp, 2) + 0.06165 * temp + 2.0327;
    float Pb = 0.0001802 * pow(temp, 3) - 0.01709 * pow(temp, 2) + 0.6645 * temp + 40.97;
    float Pc = 0.0005950 * pow(temp, 3) - 0.05359 * pow(temp, 2) + 0.8408 * temp + 221.8;
    rh = Pa * pow(Rl, 2) - Pb * Rl + Pc;
  } else {
     rh = -1;
  }
#endif

#ifdef CONVERSION_2

// �O�a���@�ɂ����5�C15, 25, 35���Ŏ����������ʂ���쐬�����ϊ��� (5�̃Z���T�̏o�͑ΐ��l�̕��ϒl���g����)
  float Pa = -0.0025 * pow(temp, 2) + 0.097 * temp + 1.122;
  float Pb = -0.0179 * pow(temp, 2) + 0.7607 * temp + 35.147;
  float Pc = -0.0304 * pow(temp, 2) + 0.722 * temp + 204;
  rh = Pa * pow(Rl, 2) - Pb * Rl + Pc;
#endif

#ifdef CONVERSION_3

// �O�a���@�ɂ����5�C15, 25, 30���Ŏ����������ʂ���쐬�����ϊ��� (�����̋߂�3�̃Z���T�̏o�͑ΐ��l�̕��ϒl���g����)
  float Pa = -0.0007 * pow(temp, 2) + 0.0556 * temp + 1.2651;
  float Pb = -0.0028 * pow(temp, 2) + 0.3813 * temp + 37.031;
  float Pc = -0.0036 * pow(temp, 2) + 0.0457 * temp + 206.91;
  rh = Pa * pow(Rl, 2) - Pb * Rl + Pc;
#endif

#ifdef CONVERSION_4

//  �uHS15P ���x�Z���T�[�̊ȈՓ������ Arduino�Łv(http://siesta.la.coocan.jp/zk/hs15/hs15_arduino.html)
//  �ł̕ϊ����i�{�v���O�����ɍ��킹�邽�߁A�ꕔ�ϐ�����ύX�j

  float R;  // HS15�̒�R�̑���l

  R= Rl + temp*(0.008363*Rl + 0.007695);       //���x�␳�@       ���␳�l�̓f�[�^�V�[�g�̃O���t���ڑ��ɂĐ���
  rh= 1.998 * R*R -40.74 *R +222.13;           //���j�A���e�B�␳ ���␳�l�̓f�[�^�V�[�g�̃O���t���ڑ��ɂĐ���
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
//  �O���ivapor pressure deficit�j���Z�o���ĕԂ�
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
