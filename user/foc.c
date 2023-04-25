/*
 * File: foc.c
 *
 * Code generated for Simulink model 'foc'.
 *
 * Model version                  : 1.39
 * Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
 * C/C++ source code generated on : Tue Jan 11 15:22:36 2022
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 * Validation result: Not run
 */

#include "foc.h"

#define SIN_COS_TABLE {\
    0x0000,0x00C9,0x0192,0x025B,0x0324,0x03ED,0x04B6,0x057F,\
    0x0648,0x0711,0x07D9,0x08A2,0x096A,0x0A33,0x0AFB,0x0BC4,\
    0x0C8C,0x0D54,0x0E1C,0x0EE3,0x0FAB,0x1072,0x113A,0x1201,\
    0x12C8,0x138F,0x1455,0x151C,0x15E2,0x16A8,0x176E,0x1833,\
    0x18F9,0x19BE,0x1A82,0x1B47,0x1C0B,0x1CCF,0x1D93,0x1E57,\
    0x1F1A,0x1FDD,0x209F,0x2161,0x2223,0x22E5,0x23A6,0x2467,\
    0x2528,0x25E8,0x26A8,0x2767,0x2826,0x28E5,0x29A3,0x2A61,\
    0x2B1F,0x2BDC,0x2C99,0x2D55,0x2E11,0x2ECC,0x2F87,0x3041,\
    0x30FB,0x31B5,0x326E,0x3326,0x33DF,0x3496,0x354D,0x3604,\
    0x36BA,0x376F,0x3824,0x38D9,0x398C,0x3A40,0x3AF2,0x3BA5,\
    0x3C56,0x3D07,0x3DB8,0x3E68,0x3F17,0x3FC5,0x4073,0x4121,\
    0x41CE,0x427A,0x4325,0x43D0,0x447A,0x4524,0x45CD,0x4675,\
    0x471C,0x47C3,0x4869,0x490F,0x49B4,0x4A58,0x4AFB,0x4B9D,\
    0x4C3F,0x4CE0,0x4D81,0x4E20,0x4EBF,0x4F5D,0x4FFB,0x5097,\
    0x5133,0x51CE,0x5268,0x5302,0x539B,0x5432,0x54C9,0x5560,\
    0x55F5,0x568A,0x571D,0x57B0,0x5842,0x58D3,0x5964,0x59F3,\
    0x5A82,0x5B0F,0x5B9C,0x5C28,0x5CB3,0x5D3E,0x5DC7,0x5E4F,\
    0x5ED7,0x5F5D,0x5FE3,0x6068,0x60EB,0x616E,0x61F0,0x6271,\
    0x62F1,0x6370,0x63EE,0x646C,0x64E8,0x6563,0x65DD,0x6656,\
    0x66CF,0x6746,0x67BC,0x6832,0x68A6,0x6919,0x698B,0x69FD,\
    0x6A6D,0x6ADC,0x6B4A,0x6BB7,0x6C23,0x6C8E,0x6CF8,0x6D61,\
    0x6DC9,0x6E30,0x6E96,0x6EFB,0x6F5E,0x6FC1,0x7022,0x7083,\
    0x70E2,0x7140,0x719D,0x71F9,0x7254,0x72AE,0x7307,0x735E,\
    0x73B5,0x740A,0x745F,0x74B2,0x7504,0x7555,0x75A5,0x75F3,\
    0x7641,0x768D,0x76D8,0x7722,0x776B,0x77B3,0x77FA,0x783F,\
    0x7884,0x78C7,0x7909,0x794A,0x7989,0x79C8,0x7A05,0x7A41,\
    0x7A7C,0x7AB6,0x7AEE,0x7B26,0x7B5C,0x7B91,0x7BC5,0x7BF8,\
    0x7C29,0x7C59,0x7C88,0x7CB6,0x7CE3,0x7D0E,0x7D39,0x7D62,\
    0x7D89,0x7DB0,0x7DD5,0x7DFA,0x7E1D,0x7E3E,0x7E5F,0x7E7E,\
    0x7E9C,0x7EB9,0x7ED5,0x7EEF,0x7F09,0x7F21,0x7F37,0x7F4D,\
    0x7F61,0x7F74,0x7F86,0x7F97,0x7FA6,0x7FB4,0x7FC1,0x7FCD,\
    0x7FD8,0x7FE1,0x7FE9,0x7FF0,0x7FF5,0x7FF9,0x7FFD,0x7FFE}

#define SIN_MASK        0x0300u
#define U0_90           0x0200u
#define U90_180         0x0300u
#define U180_270        0x0000u
#define U270_360        0x0100u
const int16_t hSin_Cos_Table[256] = SIN_COS_TABLE;

/* External inputs (root inport signals with default storage) */
ExtU rtU;

/* External outputs (root outports fed by signals with default storage) */
ExtY rtY;

/* Real-time model */
static RT_MODEL rtM_;
RT_MODEL *const rtM = &rtM_;

/* Model step function */
void foc_step(void)
{
  int32_T sector;
  real32_T T1;
  real32_T T1_tmp;
  real32_T rtb_ualpha;
  real32_T rtb_ubeta;
  Trig_Components Local_Vector_Components1;
  /* MATLAB Function: '<S1>/rePark' incorporates:
   *  Inport: '<Root>/theta'
   *  Inport: '<Root>/ud'
   *  Inport: '<Root>/uq'
   */
  // T1 = sinf(rtU.theta);
  // rtb_ubeta = cosf(rtU.theta);
  Local_Vector_Components1 = MCM_Trig_Functions(rtU.theta);
	printf("%d\r\n",Local_Vector_Components1.hSin);
  T1 = (Local_Vector_Components1.hSin)/(float)32768;
  rtb_ubeta = (Local_Vector_Components1.hCos)/(float)32768;
	printf("%f\r\n",T1);
  rtb_ualpha = (real32_T)rtU.ud * rtb_ubeta - (real32_T)rtU.uq * T1;
  rtb_ubeta = (real32_T)rtU.ud * T1 + (real32_T)rtU.uq * rtb_ubeta;

  /* MATLAB Function: '<S1>/SVPWM MATLAB Function' */
  sector = 0;

  /* Outport: '<Root>/Tcmp1' incorporates:
   *  MATLAB Function: '<S1>/SVPWM MATLAB Function'
   */
  rtY.Tcmp1 = 0.0F;

  /* Outport: '<Root>/Tcmp2' incorporates:
   *  MATLAB Function: '<S1>/SVPWM MATLAB Function'
   */
  rtY.Tcmp2 = 0.0F;

  /* Outport: '<Root>/Tcmp3' incorporates:
   *  MATLAB Function: '<S1>/SVPWM MATLAB Function'
   */
  rtY.Tcmp3 = 0.0F;

  /* MATLAB Function: '<S1>/SVPWM MATLAB Function' incorporates:
   *  Inport: '<Root>/Tpwm'
   *  Inport: '<Root>/udc'
   */
  if (rtb_ubeta > 0.0F) {
    sector = 1;
  }

  if ((1.73205078F * rtb_ualpha - rtb_ubeta) / 2.0F > 0.0F) {
    sector += 2;
  }

  if ((-1.73205078F * rtb_ualpha - rtb_ubeta) / 2.0F > 0.0F) {
    sector += 4;
  }

  switch (sector) {
   case 1:
    T1_tmp = rtU.Tpwm / rtU.udc;
    T1 = (-1.5F * rtb_ualpha + 0.866025388F * rtb_ubeta) * T1_tmp;
    rtb_ualpha = (1.5F * rtb_ualpha + 0.866025388F * rtb_ubeta) * T1_tmp;
    break;

   case 2:
    T1 = (1.5F * rtb_ualpha + 0.866025388F * rtb_ubeta) * (rtU.Tpwm / rtU.udc);
    rtb_ualpha = -(1.73205078F * rtb_ubeta * rtU.Tpwm / rtU.udc);
    break;

   case 3:
    T1 = -((-1.5F * rtb_ualpha + 0.866025388F * rtb_ubeta) * (rtU.Tpwm / rtU.udc));
    rtb_ualpha = 1.73205078F * rtb_ubeta * rtU.Tpwm / rtU.udc;
    break;

   case 4:
    T1 = -(1.73205078F * rtb_ubeta * rtU.Tpwm / rtU.udc);
    rtb_ualpha = (-1.5F * rtb_ualpha + 0.866025388F * rtb_ubeta) * (rtU.Tpwm /
      rtU.udc);
    break;

   case 5:
    T1 = 1.73205078F * rtb_ubeta * rtU.Tpwm / rtU.udc;
    rtb_ualpha = -((1.5F * rtb_ualpha + 0.866025388F * rtb_ubeta) * (rtU.Tpwm /
      rtU.udc));
    break;

   default:
    T1 = -((1.5F * rtb_ualpha + 0.866025388F * rtb_ubeta) * (rtU.Tpwm / rtU.udc));
    rtb_ualpha = -((-1.5F * rtb_ualpha + 0.866025388F * rtb_ubeta) * (rtU.Tpwm /
      rtU.udc));
    break;
  }

  rtb_ubeta = T1 + rtb_ualpha;
  if (rtb_ubeta > rtU.Tpwm) {
    T1 /= rtb_ubeta;
    rtb_ualpha /= T1 + rtb_ualpha;
  }

  rtb_ubeta = (rtU.Tpwm - (T1 + rtb_ualpha)) / 4.0F;
  T1 = T1 / 2.0F + rtb_ubeta;
  switch (sector) {
   case 1:
    /* Outport: '<Root>/Tcmp1' */
    rtY.Tcmp1 = T1;

    /* Outport: '<Root>/Tcmp2' */
    rtY.Tcmp2 = rtb_ubeta;

    /* Outport: '<Root>/Tcmp3' */
    rtY.Tcmp3 = rtb_ualpha / 2.0F + T1;
    break;

   case 2:
    /* Outport: '<Root>/Tcmp1' */
    rtY.Tcmp1 = rtb_ubeta;

    /* Outport: '<Root>/Tcmp2' */
    rtY.Tcmp2 = rtb_ualpha / 2.0F + T1;

    /* Outport: '<Root>/Tcmp3' */
    rtY.Tcmp3 = T1;
    break;

   case 3:
    /* Outport: '<Root>/Tcmp1' */
    rtY.Tcmp1 = rtb_ubeta;

    /* Outport: '<Root>/Tcmp2' */
    rtY.Tcmp2 = T1;

    /* Outport: '<Root>/Tcmp3' */
    rtY.Tcmp3 = rtb_ualpha / 2.0F + T1;
    break;

   case 4:
    /* Outport: '<Root>/Tcmp1' */
    rtY.Tcmp1 = rtb_ualpha / 2.0F + T1;

    /* Outport: '<Root>/Tcmp2' */
    rtY.Tcmp2 = T1;

    /* Outport: '<Root>/Tcmp3' */
    rtY.Tcmp3 = rtb_ubeta;
    break;

   case 5:
    /* Outport: '<Root>/Tcmp1' */
    rtY.Tcmp1 = rtb_ualpha / 2.0F + T1;

    /* Outport: '<Root>/Tcmp2' */
    rtY.Tcmp2 = rtb_ubeta;

    /* Outport: '<Root>/Tcmp3' */
    rtY.Tcmp3 = T1;
    break;

   case 6:
    /* Outport: '<Root>/Tcmp1' */
    rtY.Tcmp1 = T1;

    /* Outport: '<Root>/Tcmp2' */
    rtY.Tcmp2 = rtb_ualpha / 2.0F + T1;

    /* Outport: '<Root>/Tcmp3' */
    rtY.Tcmp3 = rtb_ubeta;
    break;
  }

  /* Outport: '<Root>/sector' incorporates:
   *  MATLAB Function: '<S1>/SVPWM MATLAB Function'
   */
  rtY.sector = (real32_T)sector;
}

/* Model initialize function */
void foc_initialize(void)
{
  /* (no initialization code required) */
}

__weak Trig_Components MCM_Trig_Functions(int16_t hAngle)
{

  int32_t shindex;
  uint16_t uhindex;

  Trig_Components Local_Components;

  /* 10 bit index computation  */
  shindex = (((int32_t)32768) + ((int32_t)hAngle));
  uhindex = (uint16_t)shindex;
  uhindex /= ((uint16_t)64);

  switch (((uint16_t)uhindex) & SIN_MASK)
  {
    case U0_90:
    {
      Local_Components.hSin = hSin_Cos_Table[( uint8_t )( uhindex )];
      Local_Components.hCos = hSin_Cos_Table[( uint8_t )( 0xFFu - ( uint8_t )( uhindex ) )];
      break;
    }

    case U90_180:
    {
      Local_Components.hSin = hSin_Cos_Table[( uint8_t )( 0xFFu - ( uint8_t )( uhindex ) )];
      Local_Components.hCos = -hSin_Cos_Table[( uint8_t )( uhindex )];
      break;
    }

    case U180_270:
    {
      Local_Components.hSin = -hSin_Cos_Table[( uint8_t )( uhindex )];
      Local_Components.hCos = -hSin_Cos_Table[( uint8_t )( 0xFFu - ( uint8_t )( uhindex ) )];
      break;
    }

    case U270_360:
    {
      Local_Components.hSin =  -hSin_Cos_Table[( uint8_t )( 0xFFu - ( uint8_t )( uhindex ) )];
      Local_Components.hCos =  hSin_Cos_Table[( uint8_t )( uhindex )];
      break;
    }

    default:
      break;
  }
  return (Local_Components);
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
