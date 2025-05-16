#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "adc.h"
#include "dma.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "stm32f10x.h"

//void Show_other(void);
//void ShowCH_Voltage(void);
// 发藄妥址?串
char uart2_send_str[100]; 
char uart3_send_str[100]; 
extern uint8_t g_usart_rx_buf[USART_REC_LEN];
uint8_t bussiness_flag = 0;
/*  接收状态
 *  bit15，      接收完成标志
 *  bit14，      接收到0x0d
 *  bit13~0，    接收到的有效字节数目
*/
extern  uint16_t g_usart_rx_sta;
extern uint16_t g_usart3_flag;
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
    // 1. 复位模块（需等待重启完成）
    USART3_SendString("AT+RST\r\n");
    delay_ms(1500); // 复位后等待2秒（模块重启约1~2秒）
    
    // 2. 测试AT指令（快速响应，无需长延迟）
    USART3_SendString("AT\r\n");    
    delay_ms(1500);  // 仅需500ms即可接收响应
    
    // 3. 断开当前WiFi（若未连接，快速响应）
    USART3_SendString("AT+CWQAP\r\n");
    delay_ms(1500);
    
    // 4. 连接WiFi（耗时最长，需足够延迟或响应检测）
    USART3_SendString("AT+CWJAP=\"oneplus\",\"hc123456\"\r\n");
    delay_ms(1500); // 延长至5秒，提高成功率
    delay_ms(1500);
    delay_ms(1500);
    delay_ms(1500);
	delay_ms(1500);
    // 5. 设置多连接（快速响应）
    USART3_SendString("AT+CIPMUX=1\r\n");    
    delay_ms(1500);
    delay_ms(1500);
    // 6. 获取IP地址（需确保WiFi已连接）
    USART3_SendString("AT+CIFSR\r\n");    
    delay_ms(1500); // 响应较快，短延迟即可
    delay_ms(1500);
    // 7. 设置服务器端口（例如设置为8080）
    USART3_SendString("AT+CIPSERVER=1,3030\r\n");
    delay_ms(1500);
       
}         
              


   
int main(void)
 { 
	delay_init();	    	 //延时函数初始化	  
	uart_init(115200);	 	//串口初始化为9600
	uart2_init(115200);
	uart3_init(115200);
	Adc_Init();		  		//ADC初始化	  
 	dma_Config(DMA1_Channel1,(u32)&ADC1->DR,(u32)&AD_Value,N*M);
	printf("*****************system begin***************\r\n");
    esp8266_Config();
    
    while(1)
	{

		while((DMA_GetFlagStatus(DMA1_FLAG_TC1)) == RESET);//每次搬运完，再处理数组得出电压
		printf("%d#%d#%d#1000#\r\n",AD_Value[0][0],AD_Value[0][1],AD_Value[0][2]);
		sprintf(uart2_send_str,"%d,%d,%d,",AD_Value[0][0],AD_Value[0][1],AD_Value[0][2]);
        sprintf(uart3_send_str,"%4d#%4d#%4d#1000",AD_Value[0][0],AD_Value[0][1],AD_Value[0][2]);
		USART2_SendString(uart2_send_str);
		//printf("%d#1000#",AD_Value[0][0]);
		delay_ms(5);

		USART3_SendString("AT+CIPSEND=0,19\r\n");
		delay_ms(1000);
		USART3_SendString(uart3_send_str);

	}
}
