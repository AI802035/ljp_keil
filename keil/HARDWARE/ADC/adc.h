#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"

#define N 1 //ÿͨ����1�Σ�����ƽ��
#define M 3 //Ϊ3��ͨ��
extern u16 AD_Value[N][M]; //�������ADCת�������Ҳ��DMA��Ŀ���ַ
extern u16 Average[M]; //���������ƽ��ֵ֮��Ľ��
void Adc_Init(void);
#endif 












//#ifndef __ADC_H
//#define __ADC_H	
//#include "sys.h"
////������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
////ALIENTEK��ӢSTM32������
////ADC ����	   
////����ԭ��@ALIENTEK
////������̳:www.openedv.com
////�޸�����:2012/9/7
////�汾��V1.0
////��Ȩ���У�����ؾ���
////Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
////All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////// 

//void Adc_Init(void);
//u16  Get_Adc(u8 ch); 
//u16 Get_Adc_Average(u8 ch,u8 times); 
// 
//#endif 
