 #include "adc.h"
 #include "delay.h"
 u16 AD_Value[N][M]; //用来存放ADC转换结果，也是DMA的目标地址，每完成一次转换，DMA将数据依次存放到数组
 u16 Average[M]; //用来存放求平均值之后的结果   
//初始化ADC
//这里我们仅以规则通道为例														   
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1, ENABLE );	  //使能ADC1通道时钟

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	//PA0/PA1/PA5 作为模拟通道输入引脚                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为默认值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1独立模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;	//模数转换工作在扫描通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//模数转换工作在连续转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel =3;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 3, ADC_SampleTime_239Cycles5 );

	

  ADC_DMACmd(ADC1, ENABLE);  //使能 ADC_DMA
	
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	
	ADC_ResetCalibration(ADC1);	//使能复位校准  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
	ADC_StartCalibration(ADC1);	 //开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
	
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
}		





























// #include "adc.h"
// #include "delay.h"

//#define N 3 //每通道采50次
//#define M 3 //为3个通道	
////extern __IO uint16_t ADCConvertedValue;
////extern vu16 AD_Value[N][M];


////初始化ADC
////这里我们仅以规则通道为例
////我们默认将开启通道0~3																	   
//void  Adc_Init(void)
//{ 	
////	ADC_InitTypeDef ADC_InitStructure; 
////	GPIO_InitTypeDef GPIO_InitStructure;
//	ADC_InitTypeDef ADC_InitStructure;
//	DMA_InitTypeDef DMA_InitStructure;
//	GPIO_InitTypeDef GPIO_InitStructure;
//	
//	
////  	RCC_ADCCLKConfig(RCC_PCLK2_Div6);			   //6分频，12M
////	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
////  	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

////	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
////	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
////	GPIO_Init(GPIOA, &GPIO_InitStructure);

//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1, ENABLE );	  //使能ADC1通道时钟
//  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
//	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

//	
//	//PA0 作为模拟通道输入引脚                         
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
//	
//	//PA1作为模拟通道输入引脚                         
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
//	
//	//PA5 作为模拟通道输入引脚                         
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
//	
//	GPIO_Init(GPIOA, &GPIO_InitStructure);	
//	
//	
//	
//   DMA_DeInit(DMA1_Channel1); //将DMA的通道1寄存器重设为缺省值
//  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR; //DMA外设ADC基地址
//  //DMA_InitStructure.DMA_PeripheralBaseAddr =ADC1_DR_Address;
//		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AD_Value; //DMA内存基地址
//   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //内存作为数据传输的目的地
//   //DMA_InitStructure.DMA_BufferSize = N*M; //DMA通道的DMA缓存的大小
//	 DMA_InitStructure.DMA_BufferSize = M*N; //DMA通道的DMA缓存的大小
//   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //外设地址寄存器不变
//   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //内存地址寄存器递增
//   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //数据宽度为16位
//   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //数据宽度为16位
//   DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //工作在循环缓存模式
//   DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMA通道 x拥有高优先级
//   DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //DMA通道x没有设置为内存到内存传输
//   DMA_Init(DMA1_Channel1, &DMA_InitStructure); //根据DMA_InitStruct中指定的参数初始化DMA的通道
//	
//	/* Enable DMA1 channel1 */
//	DMA_Cmd(DMA1_Channel1, ENABLE);
//	

//	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

//	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
//	ADC_InitStructure.ADC_ScanConvMode = ENABLE;	//模数转换工作在扫描模式
//	ADC_InitStructure.ADC_ContinuousConvMode =  ENABLE;	//模数转换工作在连续转换模式
//	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
//	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
//	ADC_InitStructure.ADC_NbrOfChannel = M;	//顺序进行规则转换的ADC通道的数目
//	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   


////设置指定ADC的规则组通道，设置它们的转化顺序和采样时间
////ADC1,ADC通道x,规则采样顺序值为y,采样时间为239.5周期
//ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5 );
//ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5 );
//ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 3, ADC_SampleTime_239Cycles5 );

//  
//	/* Enable ADC1 DMA */
//	ADC_DMACmd(ADC1, ENABLE);
//	
//// 开启ADC的DMA支持（要实现DMA功能，还需独立配置DMA通道等参数）
////ADC_DMACmd(ADC1, ENABLE);

//	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
//	
//	ADC_ResetCalibration(ADC1);	//使能复位校准  
//	 
//	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
//	
//	ADC_StartCalibration(ADC1);	 //开启AD校准
// 
//	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束

//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
////	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

//}				  
////获得ADC值
////ch:通道值 0~3
//u16 Get_Adc(u8 ch)   
//{
//  	//设置指定ADC的规则组通道，一个序列，采样时间
//	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
//  
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
//	 
//	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

//	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
//}

//u16 Get_Adc_Average(u8 ch,u8 times)
//{
//	u32 temp_val=0;
//	u8 t;
//	for(t=0;t<times;t++)
//	{
//		temp_val+=Get_Adc(ch);
//		delay_ms(5);
//	}
//	return temp_val/times;
//} 	 




///*
//该程序中的两个宏定义，M和N，分别代表有多少个通道，每个通道转换多少次，可以修改其值。
//曾出现的问题：配置时钟时要知道外部晶振是多少，以便准确配置时钟。将转换值由二进制转换为十进制时，
//要先扩大100倍，方便显示小数。最后串口输出时在 printf语句之前加这句代码，
//防止输出的第一位数据丢失：while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
//分类: STM32(Notes)
//*/
///*
//STM32 ADC多通道转换
//描述：用ADC连续采集11路模拟信号，并由DMA传输到内存。ADC配置为扫描并且连续转换模式，
//ADC的时钟配置为12MHZ。在每次转换结束后，由DMA循环将转换的数据传输到内存中。
//ADC可以连续采集N次求平均值。最后通过串口传输出最后转换的结果。
//*/





















