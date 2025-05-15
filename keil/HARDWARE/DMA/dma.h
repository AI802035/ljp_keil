#ifndef __DMA_H
#define	__DMA_H	   
#include "sys.h"

void dma_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);
void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx);//ʹ��DMA1_CHx		   
#endif











//#ifndef __DMA_H
//#define	__DMA_H	   
//#include "sys.h"
////////////////////////////////////////////////////////////////////////////////////	 
////������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
////ALIENTEKս��STM32������
////DMA ����	   
////����ԭ��@ALIENTEK
////������̳:www.openedv.com
////�޸�����:2012/9/8
////�汾��V1.0
////��Ȩ���У�����ؾ���
////Copyright(C) �������������ӿƼ����޹�˾ 2009-2019
////All rights reserved									  
////////////////////////////////////////////////////////////////////////////////////
//							    					    

//void dma_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);//����DMA1_CHx

//void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx);//ʹ��DMA1_CHx
//		   
//#endif




