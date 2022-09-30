#include "stm32f10x.h"                  // Device header
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_usart.h"            // Keil::Device:StdPeriph Drivers:USART
#include <stdio.h>

float distance = 0;
char message[20];

void gpioConfig(){

     GPIO_InitTypeDef GPIOInitStructure;
	   
	   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); 
	   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); 
	 
	   //trig pin config
	   GPIOInitStructure.GPIO_Mode = GPIO_Mode_Out_PP;               
	   GPIOInitStructure.GPIO_Pin = GPIO_Pin_1;      
	   GPIOInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	   GPIO_Init(GPIOA,&GPIOInitStructure);
	
	   //echo pini icin
	   GPIOInitStructure.GPIO_Mode = GPIO_Mode_IPD;               
	   GPIOInitStructure.GPIO_Pin = GPIO_Pin_0;      
	
	   GPIO_Init(GPIOA,&GPIOInitStructure);
	
	   //TX
	   GPIOInitStructure.GPIO_Mode = GPIO_Mode_AF_PP;               
	   GPIOInitStructure.GPIO_Pin = GPIO_Pin_9;      
	   GPIOInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	   GPIO_Init(GPIOA,&GPIOInitStructure);
	
     //LED
	   GPIOInitStructure.GPIO_Mode = GPIO_Mode_Out_PP;               
	   GPIOInitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;      
	   GPIOInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	   GPIO_Init(GPIOB,&GPIOInitStructure);

}

void delayUS(uint32_t time){

    uint32_t newTime = time*24;
	  while(newTime--);

}

float HCSR04_Read(){

     uint32_t time = 0;
	   float tempDistance = 0;
	
	   GPIO_ResetBits(GPIOA,GPIO_Pin_1);
	   delayUS(10);
     GPIO_SetBits(GPIOA,GPIO_Pin_1);
	   delayUS(10);
	   GPIO_ResetBits(GPIOA,GPIO_Pin_1);
	   delayUS(10);
	 
	   while(!(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)));    //echo pininin okumaya girip girmedigini kontrol ediyoruz girinceye kadar bu döngüde bekliyor 
	   
	   while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)){       //okumaya basladiysa bu döngünün içine girecek
		 
			time++;
			delayUS(1);
		
		 }
		 
		 tempDistance = (float)time/58;
     if(tempDistance > 400)  tempDistance = 400;            //degerimiz 400'den buyukse okuma yapmamasi gerekiyor 
		 if(tempDistance < 2)  tempDistance = 2; 
		 
		 delayUS(1000);
		 
		 return tempDistance;
}

void uartConfig(){

     USART_InitTypeDef UARTInitStructure;
	
	   RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE); 
	   
	   UARTInitStructure.USART_BaudRate = 9600;
	   UARTInitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	   UARTInitStructure.USART_Mode = USART_Mode_Tx;
	   UARTInitStructure.USART_Parity = USART_Parity_No;
     UARTInitStructure.USART_StopBits = USART_StopBits_1;
	   UARTInitStructure.USART_WordLength = USART_WordLength_8b;
	   
	   USART_Init(USART1,&UARTInitStructure);
	   USART_Cmd(USART1,ENABLE);
	   
}

void uartTrasnmit(char *string){

	  while(*string){
	  
		while(!(USART1->SR & 0x00000040));
	
		USART_SendData(USART1,*string);	
		*string++;
			
	  }

}


int main(){
	
	  gpioConfig();
	  uartConfig();

    while(1){
		
		distance = HCSR04_Read();
		sprintf(message,"Mesafe : %f cm \n",distance);     // %f degerler float turunde demek
		uartTrasnmit(message);
			
		if(distance >= 5.0){
		
		GPIO_SetBits(GPIOB,GPIO_Pin_0);
		GPIO_ResetBits(GPIOB,GPIO_Pin_1);
		
		
		}
		
		else if(distance < 5.0){
		
	  GPIO_SetBits(GPIOB,GPIO_Pin_1);
		GPIO_ResetBits(GPIOB,GPIO_Pin_0);
		
		}
	 
		delayUS(10000);
		
   }

}

