#ifndef HS15P_h
#define HS15P_h

#include <Arduino.h>

#define ANALOG_10US     362     // 1k��, 10us�����W�̃A�i���O�v���l�B�ɗ�356�ɋ߂��Ȃ�悤�ɏ[�d���Ԃ𒲐��������ʁA6us��362�ƂȂ����B
#define CON_F           2.2e-8  // HS15P�̃C���s�[�_���X����p�R���f���T�̗e�ʁi�e�ʂ̈��肵���t�B�����R���f���T�����g���j0.022uF
#define AI_THRESHOLD    128     // ���x�A�i���O�v�������l�@����ȉ��Ȃ�[�d���ԃ����W���グ��B
#define HS15P_FACTOR    1.00    // HS15P�̌̍����������l�i0.50-1.50���炢�j�B�X�P�b�`���ł�void setFactor(float factor);�Őݒ�\�B
                                // ���ۂ�莼�x�������o��ꍇ�͒l���グ��i�C���s�[�_���X�v�Z�l��傫�����A�Z�o�l��Ꮌ�x���j�B

class HS15P { 
public:
  HS15P(int vfPin, int vrPin, int vaPin); // �R���X�g���N�^�[ // vfPin --- HS15P ---- vaPin---- capacitor -----vrPin
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
  const float _chargeTIME10US  = -log(1.0 - ((float)ANALOG_10US / 1023.0)) * ((float)CON_F * 1000.0); //�v�Z��̏[�d����

  void init(void);
  int readSensor(unsigned int chargeTime);

};
#endif
