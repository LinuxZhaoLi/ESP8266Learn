#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
 
typedef unsigned char uint8;
typedef unsigned int  uint16;
typedef unsigned long uint32;
 
#define HIGH_TIME 32
 
int pinNumber = 1;  //use gpio1 to read data
uint32 databuf;   // 定义一个数据32位变量
 
uint8 readSensorData(void)
{
    uint8 crc; 
    uint8 i;
 
    pinMode(pinNumber,OUTPUT); // set mode to output
    digitalWrite(pinNumber, 1); // output a low level
    delayMicroseconds(4);	// 延时
    digitalWrite(pinNumber, 0); // output a high level 
    delay(25);
    digitalWrite(pinNumber, 1); // output a low level
    delayMicroseconds(60); 
    pinMode(pinNumber, INPUT); // set mode to input
    pullUpDnControl(pinNumber,PUD_UP); //使用函数pullUpDnControl来激活其内部的上拉电阻或下拉电阻
 
    if(digitalRead(pinNumber)==0) //SENSOR ANS
    {
        while(!digitalRead(pinNumber)); //等待DHT 输入高电平
        delayMicroseconds(80);
        for(i=0;i<32;i++)
        {
          while(digitalRead(pinNumber)); //data clock start 接收数据时可以先等待低电平过去
          while(!digitalRead(pinNumber)); //data start
          delayMicroseconds(HIGH_TIME);  //再延时60us
          databuf*=2;
          if(digitalRead(pinNumber)==1) //1
          {
            databuf++;
          }
        }
 
        for(i=0;i<8;i++) //8bit 校验位。
        {
          while(digitalRead(pinNumber)); //data clock start
          while(!digitalRead(pinNumber)); //data start
          delayMicroseconds(HIGH_TIME);
          crc*=2;  
          if(digitalRead(pinNumber)==1) //1
          {
            crc++;
          }
        }
      return 1;
    }
    else
    {
      return 0;
    }
}
 
int main (void)
{
    if (-1 == wiringPiSetup()) {
      //printf("Setup wiringPi failed!");
      return 1;
    }
 
    pinMode(pinNumber, OUTPUT); // set mode to output
    digitalWrite(pinNumber, 1); // 输出高电平
 
    //while(1) 
    //{
    pinMode(pinNumber,OUTPUT); // set mode to output
    digitalWrite(pinNumber, 1); // output a high level 
    //delay(3000);
    if(readSensorData())
    {
      printf("OK!\n");
      printf("RH:%d.%d\n",(databuf>>24)&0xff,(databuf>>16)&0xff); 
      printf("TMP:%d.%d\n",(databuf>>8)&0xff,databuf&0xff);
      databuf=0;
    }
    else
    {
      printf("Error!\n");
      databuf=0;
    }
    //}
  return 0;
}