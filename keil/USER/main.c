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
// 发送字符串
char uart2_send_str[100]; 
extern uint8_t g_usart_rx_buf[USART_REC_LEN];
uint8_t bussiness_flag = 0;
/*  接收状态
 *  bit15，      接收完成标志
 *  bit14，      接收到0x0d
 *  bit13~0，    接收到的有效字节数目
*/
extern  uint16_t g_usart_rx_sta  ;
// 发送单个字符
void USART2_SendChar(char ch)
{
    // 等待发送缓冲区为空
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    // 将要发送的字符写入发送数据寄存器
    USART_SendData(USART2, (uint8_t)ch);
}

// 发送单个字符
void USART3_SendChar(char ch)
{
    // 等待发送缓冲区为空
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    // 将要发送的字符写入发送数据寄存器
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
	* 1.AT+CWMODE=1 :设置模组为STA模式。（串口助手）
	* 2.AT+CWLAP :查询附近WIFI（串口助手）
	* 3.AT+CWJAP="123123","12345678" :连接?WIFI（串口助手）
	* 4.AT+CIFSR :查看路由器分配给模组的IP地址,例如192.168.43.104（串口助手）
	* 5.AT+CIPMUX=1 :打开多连接（串口助手）
	* 6.AT+CIPSERVER=1,8800 :设置模块服务器端口（串口助手）
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
	delay_init();	    	 //延时函数初始化	  
	uart_init(115200);	 	//串口初始化为9600
	uart2_init(115200);
	uart3_init(115200);
	 // 补充：开启全局中断（若未在初始化中执行）

	LED_Init();		  		//初始化与LED连接的硬件接口
	LCD_Init();			   	//初始化LCD 
	Adc_Init();		  		//ADC初始化	  
 	MYDMA_Config(DMA1_Channel1,(u32)&ADC1->DR,(u32)&AD_Value,N*M);//DMA1通道1,外设为ADC1,存储器地址&AD_Value,长N*M
	esp8266_Config();
//	Show_other();
  while(1)
	{

		while((DMA_GetFlagStatus(DMA1_FLAG_TC1)) == RESET);//每次搬运完，再处理数组得出电压
		printf("%d#%d#%d#1000#",AD_Value[0][0],AD_Value[0][1],AD_Value[0][2]);
		sprintf(uart2_send_str,"%d,%d,%d,",AD_Value[0][0],AD_Value[0][1],AD_Value[0][2]);
		USART2_SendString(uart2_send_str);
		//printf("%d#1000#",AD_Value[0][0]);
		delay_ms(5);
//	 	POINT_COLOR=RED;//设置字体为红色 
//		ShowCH_Voltage();
	   
		if(bussiness_flag)
		{
			USART3_SendString("AT+CIPSEND=0,20\r\n");
			delay_ms(3000);
			USART3_SendString(uart2_send_str);
		}

		if (g_usart_rx_sta & 0x8000) // 接收完成
        {
            // 检查缓冲区是否有数据，且第一个字节是否为0x00
            if (g_usart_rx_sta > 2) // 至少包含0x00 + 0x0D + 0x0A（3字节）
            {
                uint8_t first_byte = g_usart_rx_buf[0];
                if (first_byte == 0x00)
                {
                    printf("接收正确：以0x00开头且以\r\n结尾\n");
                    // 执行正确处理逻辑（如复位标志、清空缓冲区）
										bussiness_flag = 1;
                    g_usart_rx_sta = 0; // 清除接收标志
                    memset(g_usart_rx_buf, 0, USART_REC_LEN); // 清空缓冲区
										USART3_SendString("AT+CIPSEND=0,5\r\n");
										delay_ms(3000);
										USART3_SendString("1");


                }
                else
                {
                    printf("接收错误：第一个字节不是0x00\n");
                    g_usart_rx_sta = 0;
                    memset(g_usart_rx_buf, 0, USART_REC_LEN);
                }
            }
            else
            {
                printf("接收错误：数据长度不足\n");
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
//	for(j=0;j<M;j++)//M，通道个数
//	 { 
//		for(i=0;i<N;i++)//N，每个通道采样次数
//		 {
//	      SUM+=AD_Value[i][j]; //每个通道的数据求和，注意是二维数组的每列的数据求和
//		 }
//		Average[j]=SUM/N; //再取平均
//		temp=(float)Average[j]*(3.3/4096.0);
//		Average[j]=temp;
//		LCD_ShowxNum(144,40+30*j,Average[j],1,24,0);//显示电压值的整数部分
//		temp-=Average[j];
//		temp*=10000;
//		LCD_ShowxNum(144+24,40+30*j,temp,4,24,0X80);//显示电压值的小数部分
//		Average[j]=0;//开众提醒置零
//		SUM=0;//开众提醒置零
//	 } 
//}

//void Show_other(void)
//{
//	int j=0;
//	u8 ADC_Ch[20];
//	POINT_COLOR=MAGENTA;//设置字体为紫色 
//	LCD_ShowString(20,15,200,24,24,"8 Channel ADC+DMA");
//	show_sentence24_24(20,275,200,24,24,"棒棒恭喜成功");
//	LCD_ShowString(120,295,200,24,24,"2020/12/4");	
//	for(j=0;j<M;j++)
//	{
//		sprintf((char*)ADC_Ch,"ADC_CH%d_VOL:0.0000V",j);//将int变量j打印到ADC_Ch数组。	
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
////#define N 3//每通道采10次
////#define M 3 //为3个通道
////vu16 AD_Value[N][M]; //用来存放ADC转换结果，也是DMA的目标地址
////vu16 After_filter[M]; //用来存放求平均值之后的结果


//// void u32tostr1(unsigned long dat,char *str);
//// void u32tostr2(unsigned long dat,char *str);
//// void u32tostr3(unsigned long dat,char *str);
//// void  filter(void);
//// 
//// 
//// int main(void)
//// {	 

////	u16 value[M];  //M个通道
////	 char temp1[10];
////	 char temp2[10];
////	 char temp3[10];
////	delay_init();	    	 //延时函数初始化	  
////	uart_init(115200);	 	//串口初始化为115200
//// 	LED_Init();			     //LED端口初始化			 	
//// 	Adc_Init();		  		//ADC初始化

////	//while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);  

////	while(1)
////	{
////		//adcx=Get_Adc_Average(ADC_Channel_1,3);
////	//	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
////		//adcx=Get_Adc(ADC_Channel_1);
//////	  u32tostr(adcx,temp);
//////	 	printf("%s#",temp);
////	//	printf("%d#",adcx);
////		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET); //等待传输完成否则第一位数据容易丢失
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
////    for(i=0;i<M;i++)  //对每一个通道而言M

////     {

////      for ( count=0;count<N;count++)   //N采样多少次，M通道数

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

//#include "stm32f10x.h" //这个头文件包括STM32F10x所有外围寄存器、位、内存映射的定义
////#include "eval.h" //头文件（包括串口、按键、LED的函数声明）
////#include "SysTickDelay.h"
////#include "UART_INTERFACE.h"
//#include <stdio.h>

//#define N 50 //每通道采50次
//#define M 3 //为3个通道

//__IO uint16_t ADCConvertedValue;	//ADC DMA  buffer

//vu16 AD_Value[N][M]; //用来存放ADC转换结果，也是DMA的目标地址
//vu16 After_filter[M]; //用来存放求平均值之后的结果
//int i;
// 
// void GPIO_Configuration(void)
//{
//GPIO_InitTypeDef GPIO_InitStructure;

//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //因为USART1管脚是以复用的形式接到GPIO口上的，所以使用复用推挽式输出
//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//GPIO_Init(GPIOA, &GPIO_InitStructure);


//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//GPIO_Init(GPIOA, &GPIO_InitStructure);



////PA0/1/2 作为模拟通道输入引脚
//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //模拟输入引脚
//GPIO_Init(GPIOA, &GPIO_InitStructure);

////PB0/1 作为模拟通道输入引脚
//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //模拟输入引脚
//GPIO_Init(GPIOB, &GPIO_InitStructure);

////PC0/1/2/3/4/5 作为模拟通道输入引脚
//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //模拟输入引脚
//GPIO_Init(GPIOC, &GPIO_InitStructure);
//}



//void RCC_Configuration(void)
//{
//ErrorStatus HSEStartUpStatus;

//RCC_DeInit(); //RCC 系统复位
//RCC_HSEConfig(RCC_HSE_ON); //开启HSE
//HSEStartUpStatus = RCC_WaitForHSEStartUp(); //等待HSE准备好
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
//| RCC_APB2Periph_GPIOC |RCC_APB2Periph_ADC1 | RCC_APB2Periph_AFIO |RCC_APB2Periph_USART1, ENABLE ); //使能ADC1通道时钟，各个管脚时钟

//RCC_ADCCLKConfig(RCC_PCLK2_Div6); //72M/6=12,ADC最大时间不能超过14M
//RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //使能DMA传输

//}
//}


//void ADC1_Configuration(void)
//{
//ADC_InitTypeDef ADC_InitStructure;

//ADC_DeInit(ADC1); //将外设 ADC1 的全部寄存器重设为缺省值


//ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ADC工作模式:ADC1和ADC2工作在独立模式
//ADC_InitStructure.ADC_ScanConvMode =ENABLE; //模数转换工作在扫描模式
//ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //模数转换工作在连续转换模式
//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //外部触发转换关闭
//ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //ADC数据右对齐
//ADC_InitStructure.ADC_NbrOfChannel = M; //顺序进行规则转换的ADC通道的数目
//ADC_Init(ADC1, &ADC_InitStructure); //根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器


////设置指定ADC的规则组通道，设置它们的转化顺序和采样时间
////ADC1,ADC通道x,规则采样顺序值为y,采样时间为239.5周期
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

//// 开启ADC的DMA支持（要实现DMA功能，还需独立配置DMA通道等参数）
//ADC_DMACmd(ADC1, ENABLE);


//ADC_Cmd(ADC1, ENABLE); //使能指定的ADC1

//ADC_ResetCalibration(ADC1); //复位指定的ADC1的校准寄存器

//while(ADC_GetResetCalibrationStatus(ADC1)); //获取ADC1复位校准寄存器的状态,设置状态则等待


//ADC_StartCalibration(ADC1); //开始指定ADC1的校准状态

//while(ADC_GetCalibrationStatus(ADC1)); //获取指定ADC1的校准程序,设置状态则等待


//}


//void DMA_Configuration(void)
//{

//DMA_InitTypeDef DMA_InitStructure;
//DMA_DeInit(DMA1_Channel1); //将DMA的通道1寄存器重设为缺省值
//DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR; //DMA外设ADC基地址
//DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AD_Value; //DMA内存基地址
//DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //内存作为数据传输的目的地
//DMA_InitStructure.DMA_BufferSize = N*M; //DMA通道的DMA缓存的大小
//DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //外设地址寄存器不变
//DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //内存地址寄存器递增
//DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //数据宽度为16位
//DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //数据宽度为16位
//DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //工作在循环缓存模式
//DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMA通道 x拥有高优先级
//DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //DMA通道x没有设置为内存到内存传输
//DMA_Init(DMA1_Channel1, &DMA_InitStructure); //根据DMA_InitStruct中指定的参数初始化DMA的通道

//}



////配置所有外设
//void Init_All_Periph(void)
//{

//RCC_Configuration();

//GPIO_Configuration();

//ADC1_Configuration();

//DMA_Configuration();

////USART1_Configuration();
////USART_Configuration(9600);
//	uart_init(115200);	 	//串口初始化为115200


//}



//u16 GetVolt(u16 advalue)

//{

//return (u16)(advalue * 330 / 4096); //求的结果扩大了100倍，方便下面求出小数

//}




//void filter(void)
//{
//int sum = 0;
//u8 count;
//for(i=0;i<12;i++)  //对每一个通道而言M

//{

//for ( count=0;count<N;count++)   //N采样多少次，M通道数

//{

//sum += AD_Value[count][i];

//}

//After_filter[i]=sum/N;

//sum=0;
//}

//}




//int main(void)
//{

//u16 value[M];  //M个通道

//delay_init();	    	 //延时函数初始化	  
////NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
////init_All_Periph();
////SysTick_Initaize();


//ADC_SoftwareStartConvCmd(ADC1, ENABLE);
//DMA_Cmd(DMA1_Channel1, ENABLE); //启动DMA通道
//while(1)
//{
//while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);//等待传输完成否则第一位数据容易丢失

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
