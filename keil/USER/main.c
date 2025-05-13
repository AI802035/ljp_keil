#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "adc.h"
#include "dma.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
//void Show_other(void);
//void ShowCH_Voltage(void);
// �����ַ���
char uart2_send_str[100]; 
extern uint8_t g_usart_rx_buf[USART_REC_LEN];
uint8_t bussiness_flag = 0;
/*  ����״̬
 *  bit15��      ������ɱ�־
 *  bit14��      ���յ�0x0d
 *  bit13~0��    ���յ�����Ч�ֽ���Ŀ
*/
extern  uint16_t g_usart_rx_sta  ;
// ���͵����ַ�
void USART2_SendChar(char ch)
{
    // �ȴ����ͻ�����Ϊ��
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    // ��Ҫ���͵��ַ�д�뷢�����ݼĴ���
    USART_SendData(USART2, (uint8_t)ch);
}

// ���͵����ַ�
void USART3_SendChar(char ch)
{
    // �ȴ����ͻ�����Ϊ��
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    // ��Ҫ���͵��ַ�д�뷢�����ݼĴ���
    USART_SendData(USART3, (uint8_t)ch);
}
void USART2_SendString(char* str)
{
    while (*str)
    {
        USART2_SendChar(*str++);
    }
}

void USART3_SendString(char* str)  
{  
    while (*str)
    {
        USART3_SendChar(*str++);
    }
}

void esp8266_Config(void){
/*
	* 1.AT+CWMODE=1 :����ģ��ΪSTAģʽ�����������֣�
	* 2.AT+CWLAP :��ѯ����WIFI���������֣�
	* 3.AT+CWJAP="123123","12345678" :����?WIFI���������֣�
	* 4.AT+CIFSR :�鿴·���������ģ���IP��ַ,����192.168.43.104���������֣�
	* 5.AT+CIPMUX=1 :�򿪶����ӣ��������֣�
	* 6.AT+CIPSERVER=1,8800 :����ģ��������˿ڣ��������֣�
	*/
	delay_ms(3000);
	USART3_SendString("AT+RST\r\n");
	delay_ms(3000);
	USART3_SendString("AT+CWMODE=3\r\n");
	delay_ms(3000);
	USART3_SendString("AT+CWSAP=\"MyESPAP\",\"password\",5,3\r\n");
	delay_ms(3000);
	USART3_SendString("AT+CWJAP=\"pangshengli\",\"13759991202\\r\n");
	delay_ms(3000);
	USART3_SendString("AT+CIFSR\r\n");
	delay_ms(3000);
	USART3_SendString("AT+CIPMUX=1\r\n");    
	delay_ms(3000);
	USART3_SendString("AT+CIPSERVER=1,1233\r\n");
	delay_ms(3000);
	USART3_SendString("AT+CIPSEND=0,5\r\n");
	delay_ms(3000);
	USART3_SendString("want_access\r\n");    
	delay_ms(3000);
}                        

   
int main(void)
 { 
	delay_init();	    	 //��ʱ������ʼ��	  
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ9600
	uart2_init(115200);
	uart3_init(115200);
	 // ���䣺����ȫ���жϣ���δ�ڳ�ʼ����ִ�У�

	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
	LCD_Init();			   	//��ʼ��LCD 
	Adc_Init();		  		//ADC��ʼ��	  
 	MYDMA_Config(DMA1_Channel1,(u32)&ADC1->DR,(u32)&AD_Value,N*M);//DMA1ͨ��1,����ΪADC1,�洢����ַ&AD_Value,��N*M
	esp8266_Config();
//	Show_other();
  while(1)
	{

		while((DMA_GetFlagStatus(DMA1_FLAG_TC1)) == RESET);//ÿ�ΰ����꣬�ٴ�������ó���ѹ
		printf("%d#%d#%d#1000#",AD_Value[0][0],AD_Value[0][1],AD_Value[0][2]);
		sprintf(uart2_send_str,"%d,%d,%d,",AD_Value[0][0],AD_Value[0][1],AD_Value[0][2]);
		USART2_SendString(uart2_send_str);
		//printf("%d#1000#",AD_Value[0][0]);
		delay_ms(5);
//	 	POINT_COLOR=RED;//��������Ϊ��ɫ 
//		ShowCH_Voltage();
	   
		if(bussiness_flag)
		{
			USART3_SendString("AT+CIPSEND=0,20\r\n");
			delay_ms(3000);
			USART3_SendString(uart2_send_str);
		}

		if (g_usart_rx_sta & 0x8000) // �������
        {
            // ��黺�����Ƿ������ݣ��ҵ�һ���ֽ��Ƿ�Ϊ0x00
            if (g_usart_rx_sta > 2) // ���ٰ���0x00 + 0x0D + 0x0A��3�ֽڣ�
            {
                uint8_t first_byte = g_usart_rx_buf[0];
                if (first_byte == 0x00)
                {
                    printf("������ȷ����0x00��ͷ����\r\n��β\n");
                    // ִ����ȷ�����߼����縴λ��־����ջ�������
										bussiness_flag = 1;
                    g_usart_rx_sta = 0; // ������ձ�־
                    memset(g_usart_rx_buf, 0, USART_REC_LEN); // ��ջ�����
										USART3_SendString("AT+CIPSEND=0,5\r\n");
										delay_ms(3000);
										USART3_SendString("1");


                }
                else
                {
                    printf("���մ��󣺵�һ���ֽڲ���0x00\n");
                    g_usart_rx_sta = 0;
                    memset(g_usart_rx_buf, 0, USART_REC_LEN);
                }
            }
            else
            {
                printf("���մ������ݳ��Ȳ���\n");
                g_usart_rx_sta = 0;
                memset(g_usart_rx_buf, 0, USART_REC_LEN);
            }
        }
	}
}
	
//void ShowCH_Voltage(void)
//{
//	int i=0,j=0;
//	float temp,SUM;
//	for(j=0;j<M;j++)//M��ͨ������
//	 { 
//		for(i=0;i<N;i++)//N��ÿ��ͨ����������
//		 {
//	      SUM+=AD_Value[i][j]; //ÿ��ͨ����������ͣ�ע���Ƕ�ά�����ÿ�е��������
//		 }
//		Average[j]=SUM/N; //��ȡƽ��
//		temp=(float)Average[j]*(3.3/4096.0);
//		Average[j]=temp;
//		LCD_ShowxNum(144,40+30*j,Average[j],1,24,0);//��ʾ��ѹֵ����������
//		temp-=Average[j];
//		temp*=10000;
//		LCD_ShowxNum(144+24,40+30*j,temp,4,24,0X80);//��ʾ��ѹֵ��С������
//		Average[j]=0;//������������
//		SUM=0;//������������
//	 } 
//}

//void Show_other(void)
//{
//	int j=0;
//	u8 ADC_Ch[20];
//	POINT_COLOR=MAGENTA;//��������Ϊ��ɫ 
//	LCD_ShowString(20,15,200,24,24,"8 Channel ADC+DMA");
//	show_sentence24_24(20,275,200,24,24,"������ϲ�ɹ�");
//	LCD_ShowString(120,295,200,24,24,"2020/12/4");	
//	for(j=0;j<M;j++)
//	{
//		sprintf((char*)ADC_Ch,"ADC_CH%d_VOL:0.0000V",j);//��int����j��ӡ��ADC_Ch���顣	
//		LCD_ShowString(0,40+30*j,240,24,24,ADC_Ch);
//	}
//}




















////#include "led.h"
////#include "delay.h"
////#include "key.h"
////#include "sys.h"
////#include "lcd.h"
////#include "usart.h"	 
////#include "adc.h"
////#include "stm32f10x.h" 
////#include <stdio.h>
////#define N 3//ÿͨ����10��
////#define M 3 //Ϊ3��ͨ��
////vu16 AD_Value[N][M]; //�������ADCת�������Ҳ��DMA��Ŀ���ַ
////vu16 After_filter[M]; //���������ƽ��ֵ֮��Ľ��


//// void u32tostr1(unsigned long dat,char *str);
//// void u32tostr2(unsigned long dat,char *str);
//// void u32tostr3(unsigned long dat,char *str);
//// void  filter(void);
//// 
//// 
//// int main(void)
//// {	 

////	u16 value[M];  //M��ͨ��
////	 char temp1[10];
////	 char temp2[10];
////	 char temp3[10];
////	delay_init();	    	 //��ʱ������ʼ��	  
////	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
//// 	LED_Init();			     //LED�˿ڳ�ʼ��			 	
//// 	Adc_Init();		  		//ADC��ʼ��

////	//while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);  

////	while(1)
////	{
////		//adcx=Get_Adc_Average(ADC_Channel_1,3);
////	//	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
////		//adcx=Get_Adc(ADC_Channel_1);
//////	  u32tostr(adcx,temp);
//////	 	printf("%s#",temp);
////	//	printf("%d#",adcx);
////		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET); //�ȴ�������ɷ����һλ�������׶�ʧ
////	//	 filter();
////	 
//////       value[0]= AD_Value[0][0];  u32tostr1(value[0],temp1);  // 	printf("%s#",temp1);
//////       value[1]= AD_Value[0][1];  u32tostr2(value[1],temp2);	//	printf("%s@",temp2);
//////       value[2]= AD_Value[0][2];  u32tostr3(value[2],temp3);

//////		    u32tostr1(AD_Value[0][0],temp1);  // 	printf("%s#",temp1);
//////        u32tostr2(AD_Value[0][1],temp2);	//	printf("%s@",temp2);
//////        u32tostr3(AD_Value[0][2],temp3);
////		
//////    	printf("%s#",temp1);
//////			printf("%s#",temp2);
//////	   	printf("%s#",temp3);
//////			printf("1000#");
//////		 printf("%s#%s#%s#1000#",temp1,temp2,temp3);
////			printf("%d#%d#%d#1000#",AD_Value[0][0],AD_Value[0][1], AD_Value[0][2]);
////			//delay_us(500);
////		//delay_ms(5);
//////for(i=0;i<3;i++)
//////{
//////value[i]= GetVolt(After_filter[i]);

//////printf("value[%d]:\t%d.%dv\n",i,value[i]/100,value[i]) ;
//////delay_ms(100);
//////}
//////}

////		
////		LED0=!LED0;
////		
////	}
////}
//// 
////void  filter(void)
////   {
////      float sum = 0;
////      u8 count;
////		  int i;
////    for(i=0;i<M;i++)  //��ÿһ��ͨ������M

////     {

////      for ( count=0;count<N;count++)   //N�������ٴΣ�Mͨ����

////      {

////          sum += AD_Value[count][i];

////        }

////       After_filter[i]=sum/N;

////      sum=0;
////      }
////      }
////	 
////			
////void u32tostr1(unsigned long dat,char *str)
//// {
////	 char t1[20];
////	 unsigned char i=0,j=0;
////	 i=0;
////	 while(dat)
////	 {
////		 t1[i]=dat%10+0x30;
////		 i++;
////		 dat/=10;
////	 }
////	 j=i;
////	  for(i=0;i<j;i++)
////    {
////          str[i]=t1[j-i-1];
////    }
////    if(!i) {str[i++]='0';}
////    str[i]=0;
////	 
//// }
//// void u32tostr2(unsigned long dat,char *str)
//// {
////	 char t2[20];
////	 unsigned char i=0,j=0;
////	 i=0;
////	 while(dat)
////	 {
////		 t2[i]=dat%10+0x30;
////		 i++;
////		 dat/=10;
////	 }
////	 j=i;
////	  for(i=0;i<j;i++)
////    {
////          str[i]=t2[j-i-1];
////    }
////    if(!i) {str[i++]='0';}
////    str[i]=0;
////	 
//// }
//// void u32tostr3(unsigned long dat,char *str)
//// {
////	 char t3[20];
////	 unsigned char i=0,j=0;
////	 i=0;
////	 while(dat)
////	 {
////		 t3[i]=dat%10+0x30;
////		 i++;
////		 dat/=10;
////	 }
////	 j=i;
////	  for(i=0;i<j;i++)
////    {
////          str[i]=t3[j-i-1];
////    }
////    if(!i) {str[i++]='0';}
////    str[i]=0;
////	 
//// }
//// 
//	V_TAO=(float)ADCConvertedValue*(3.3/4096)*1000;

//#include "stm32f10x.h" //���ͷ�ļ�����STM32F10x������Χ�Ĵ�����λ���ڴ�ӳ��Ķ���
////#include "eval.h" //ͷ�ļ����������ڡ�������LED�ĺ���������
////#include "SysTickDelay.h"
////#include "UART_INTERFACE.h"
//#include <stdio.h>

//#define N 50 //ÿͨ����50��
//#define M 3 //Ϊ3��ͨ��

//__IO uint16_t ADCConvertedValue;	//ADC DMA  buffer

//vu16 AD_Value[N][M]; //�������ADCת�������Ҳ��DMA��Ŀ���ַ
//vu16 After_filter[M]; //���������ƽ��ֵ֮��Ľ��
//int i;
// 
// void GPIO_Configuration(void)
//{
//GPIO_InitTypeDef GPIO_InitStructure;

//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //��ΪUSART1�ܽ����Ը��õ���ʽ�ӵ�GPIO���ϵģ�����ʹ�ø�������ʽ���
//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//GPIO_Init(GPIOA, &GPIO_InitStructure);


//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//GPIO_Init(GPIOA, &GPIO_InitStructure);



////PA0/1/2 ��Ϊģ��ͨ����������
//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //ģ����������
//GPIO_Init(GPIOA, &GPIO_InitStructure);

////PB0/1 ��Ϊģ��ͨ����������
//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //ģ����������
//GPIO_Init(GPIOB, &GPIO_InitStructure);

////PC0/1/2/3/4/5 ��Ϊģ��ͨ����������
//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //ģ����������
//GPIO_Init(GPIOC, &GPIO_InitStructure);
//}



//void RCC_Configuration(void)
//{
//ErrorStatus HSEStartUpStatus;

//RCC_DeInit(); //RCC ϵͳ��λ
//RCC_HSEConfig(RCC_HSE_ON); //����HSE
//HSEStartUpStatus = RCC_WaitForHSEStartUp(); //�ȴ�HSE׼����
//if(HSEStartUpStatus == SUCCESS)
//{
//FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); //Enable Prefetch Buffer
//FLASH_SetLatency(FLASH_Latency_2); //Set 2 Latency cycles
//RCC_HCLKConfig(RCC_SYSCLK_Div1); //AHB clock = SYSCLK
//RCC_PCLK2Config(RCC_HCLK_Div1); //APB2 clock = HCLK
//RCC_PCLK1Config(RCC_HCLK_Div2); //APB1 clock = HCLK/2
//RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_6); //PLLCLK = 12MHz * 6 = 72 MHz
//RCC_PLLCmd(ENABLE); //Enable PLL
//while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET); //Wait till PLL is ready
//RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); //Select PLL as system clock source
//while(RCC_GetSYSCLKSource() != 0x08); //Wait till PLL is used as system clock source

//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB
//| RCC_APB2Periph_GPIOC |RCC_APB2Periph_ADC1 | RCC_APB2Periph_AFIO |RCC_APB2Periph_USART1, ENABLE ); //ʹ��ADC1ͨ��ʱ�ӣ������ܽ�ʱ��

//RCC_ADCCLKConfig(RCC_PCLK2_Div6); //72M/6=12,ADC���ʱ�䲻�ܳ���14M
//RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //ʹ��DMA����

//}
//}


//void ADC1_Configuration(void)
//{
//ADC_InitTypeDef ADC_InitStructure;

//ADC_DeInit(ADC1); //������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ


//ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
//ADC_InitStructure.ADC_ScanConvMode =ENABLE; //ģ��ת��������ɨ��ģʽ
//ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //ģ��ת������������ת��ģʽ
//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //�ⲿ����ת���ر�
//ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //ADC�����Ҷ���
//ADC_InitStructure.ADC_NbrOfChannel = M; //˳����й���ת����ADCͨ������Ŀ
//ADC_Init(ADC1, &ADC_InitStructure); //����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���


////����ָ��ADC�Ĺ�����ͨ�����������ǵ�ת��˳��Ͳ���ʱ��
////ADC1,ADCͨ��x,�������˳��ֵΪy,����ʱ��Ϊ239.5����
//ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5 );
//ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5 );
//ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_239Cycles5 );
//ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_239Cycles5 );
//ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 5, ADC_SampleTime_239Cycles5 );
//ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 6, ADC_SampleTime_239Cycles5 );
//ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 7, ADC_SampleTime_239Cycles5 );
//ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 8, ADC_SampleTime_239Cycles5 );
//ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 9, ADC_SampleTime_239Cycles5 );
//ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 10, ADC_SampleTime_239Cycles5 );
//ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 11, ADC_SampleTime_239Cycles5 );
//ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 12, ADC_SampleTime_239Cycles5 );

//// ����ADC��DMA֧�֣�Ҫʵ��DMA���ܣ������������DMAͨ���Ȳ�����
//ADC_DMACmd(ADC1, ENABLE);


//ADC_Cmd(ADC1, ENABLE); //ʹ��ָ����ADC1

//ADC_ResetCalibration(ADC1); //��λָ����ADC1��У׼�Ĵ���

//while(ADC_GetResetCalibrationStatus(ADC1)); //��ȡADC1��λУ׼�Ĵ�����״̬,����״̬��ȴ�


//ADC_StartCalibration(ADC1); //��ʼָ��ADC1��У׼״̬

//while(ADC_GetCalibrationStatus(ADC1)); //��ȡָ��ADC1��У׼����,����״̬��ȴ�


//}


//void DMA_Configuration(void)
//{

//DMA_InitTypeDef DMA_InitStructure;
//DMA_DeInit(DMA1_Channel1); //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
//DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR; //DMA����ADC����ַ
//DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AD_Value; //DMA�ڴ����ַ
//DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //�ڴ���Ϊ���ݴ����Ŀ�ĵ�
//DMA_InitStructure.DMA_BufferSize = N*M; //DMAͨ����DMA����Ĵ�С
//DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //�����ַ�Ĵ�������
//DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //�ڴ��ַ�Ĵ�������
//DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //���ݿ��Ϊ16λ
//DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //���ݿ��Ϊ16λ
//DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //������ѭ������ģʽ
//DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMAͨ�� xӵ�и����ȼ�
//DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
//DMA_Init(DMA1_Channel1, &DMA_InitStructure); //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��

//}



////������������
//void Init_All_Periph(void)
//{

//RCC_Configuration();

//GPIO_Configuration();

//ADC1_Configuration();

//DMA_Configuration();

////USART1_Configuration();
////USART_Configuration(9600);
//	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200


//}



//u16 GetVolt(u16 advalue)

//{

//return (u16)(advalue * 330 / 4096); //��Ľ��������100���������������С��

//}




//void filter(void)
//{
//int sum = 0;
//u8 count;
//for(i=0;i<12;i++)  //��ÿһ��ͨ������M

//{

//for ( count=0;count<N;count++)   //N�������ٴΣ�Mͨ����

//{

//sum += AD_Value[count][i];

//}

//After_filter[i]=sum/N;

//sum=0;
//}

//}




//int main(void)
//{

//u16 value[M];  //M��ͨ��

//delay_init();	    	 //��ʱ������ʼ��	  
////NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
////init_All_Periph();
////SysTick_Initaize();


//ADC_SoftwareStartConvCmd(ADC1, ENABLE);
//DMA_Cmd(DMA1_Channel1, ENABLE); //����DMAͨ��
//while(1)
//{
//while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);//�ȴ�������ɷ����һλ�������׶�ʧ

//filter();
//for(i=0;i<12;i++)
//{
//value[i]= GetVolt(After_filter[i]);

//printf("value[%d]:\t%d.%dv\n",i,value[i]/100,value[i]) ;
//delay_ms(100);
//}
//}

//}
// 
