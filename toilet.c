/*------------------------------------------------------------------*/
/* --- STC MCU Limited ---------------------------------------------*/
/* --- STC12C56xx Series Programmable Clock Output Demo ------------*/
/* --- Mobile: (86)13922805190 -------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ---------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966----------------------*/
/* --- Web: www.STCMCU.com -----------------------------------------*/
/* --- Web: www.GXWMCU.com -----------------------------------------*/
/* If you want to use the program or the program referenced in the  */
/* article, please specify in which data and procedures from STC    */
/*------------------------------------------------------------------*/

#include "STC12C20XX.h"


sbit Watchdog=P3^7;  //看门狗输出

sbit Relay1=P3^0;  
sbit Relay2=P3^1;  

//第一版的电路板的行程开关和按钮的定义
//sbit Forward  =P1^4;  
//sbit Backward =P1^5;  
//sbit Button   =P1^7;  

sbit Forward  =P1^7;  		//电机反转限位开关点
sbit Backward =P1^6; 		//电机零位限位开关点 
sbit Button   =P1^4;  





sbit InOut1=P1^0;  
sbit InOut2=P1^1;  
sbit InOut3=P1^2;  
sbit InOut4=P1^3;  


unsigned int Timer_50mS;
unsigned int Timer_1S;
unsigned int TimerOut;

void Delay_ms (unsigned int a);
void Delay_2S(void);
void Timer0Init (void);
void RelayOutInit (void);
void StepLightInit(void);

void StepLight(unsigned int Step);

unsigned int PutButtonDown(void);
unsigned int GetPointForward(void);
unsigned int GetPointBackward(void);

void MotorMotionForward(void);
void MotorMotionReverse(void);
void MotorStopMotion(void);
void MotorMotionHome(void);


/*--------------------------------------主函数--------------------------------------*/
void main(void)
{
	Timer0Init(); 

	ET0=1;			//开定时器中断
	EA=1;			//开总中断
	TR0=1;			//启动定时器

	RelayOutInit();
	StepLightInit();

	Timer_50mS= 0 ;
	Timer_1S = 0 ;
	TimerOut = 0 ;

	MotorMotionHome();

	while(1)
	{
		StepLight(1);
		while(!PutButtonDown());

		StepLight(2);
		MotorMotionForward();

		StepLight(3);
		MotorStopMotion();
		Delay_2S();

		StepLight(4);
		MotorMotionReverse();

		Delay_2S();
	}
}


/*-----------------------------------中断服务程序-----------------------------------*/
void Timer0_Rountine(void) interrupt 1 
{
	TH0=0x3C;			//重新装载定时器时基值
	TL0=0xB0;

    Timer_50mS++;

	if (Timer_50mS==20)
	{					//定时器循环计数20次为一秒
		Timer_50mS=0;	//次数清零，从新循环计数
		Timer_1S++;		//秒加一
	}

	if(Timer_1S >= 20)	//当电机运行时间超过此设定时间后，置位标志位TimeOut.
	{
		Timer_1S = 0;
		TimerOut = 1 ;
	}

	Watchdog=~Watchdog;		//喂狗
}


/*----------------------------------定时器初始化函数----------------------------------*/
void Timer0Init (void)
{   
	TMOD=0x01;				//定时器工作方式1:16位

	TH0=(65536-50000)/256;	//定时时间50ms
	TL0=(65536-50000)%256;
} 


/*----------------------------------继电器/输出口初始化函数----------------------------------*/
void RelayOutInit (void)
{   
	P3M0=0x00;				//P3口工作方式:强推挽输出
	P3M1=0xff;	

	Relay1=1;  
	Relay2=1;  
} 


/*----------------------------------运行步骤指示LED灯输出口初始化函数----------------------------------*/
void StepLightInit(void)
{
	P1M0=0x00;				//P1口工作方式:强推挽输出
	P1M1=0x0f;	

	InOut1=0;  
	InOut2=0;  
	InOut3=0;  
	InOut4=0;  
}

/*-------------------------------------延时函数-------------------------------------*/
void Delay_ms (unsigned int a)
{
	unsigned int i;
	while( --a != 0)
	{
		for(i = 0; i < 600; i++);
	}
}


/*-------------------------------------长延时函数-------------------------------------*/
void Delay_2S()
{
	Delay_ms(1000);
	Delay_ms(1000);
}


/*----------------------------------启动按钮检测函数----------------------------------*/
//当按纽按下，函数返回1；否则，函数返回0
unsigned int PutButtonDown(void)
{
	unsigned int Flag;
	Flag = 0 ;

	if(Button==0)
	{
		Delay_ms(100);

		if(Button==0)
			Flag = 1 ;
	}

	return (Flag);
}


/*----------------------------------工作步骤指示LED显示函数----------------------------------*/
void StepLight(unsigned int Step)
{
	switch(Step)
	{
		case 1:	InOut1=1;
				InOut2=0;
				InOut3=0;
				InOut4=0;
				break;
		case 2:	InOut1=0;
				InOut2=1;
				InOut3=0;
				InOut4=0;
				break;
		case 3:	InOut1=0;
				InOut2=0;
				InOut3=1;
				InOut4=0;
				break;
		case 4:	InOut1=0;
				InOut2=0;
				InOut3=0;
				InOut4=1;
				break;
		default:InOut1=0;
				InOut2=0;
				InOut3=0;
				InOut4=0;
	}
}



/*----------------------------------电机向前动作到停止点函数----------------------------------*/
void MotorMotionForward(void)
{
	Relay1 =1;
	Relay2 =0;

	Timer_1S =0;
	TimerOut =0;

	while(!GetPointForward());

	Relay1 =1;
	Relay2 =1;
}


/*----------------------------------电机向后动作到停止点函数----------------------------------*/
void MotorMotionReverse(void)
{
	Relay1 =0;
	Relay2 =1;

	Timer_1S =0;
	TimerOut =0;

	while(!GetPointBackward());

	Relay1 =1;
	Relay2 =1;
}


/*----------------------------------电机停止动作函数----------------------------------*/
void MotorStopMotion(void)
{
	Relay1 =1;
	Relay2 =1;
}



/*----------------------------------电机回到初始位置函数----------------------------------*/
void MotorMotionHome(void)
{
	if(!GetPointBackward())
	{
		MotorMotionReverse();
	}
}


/*----------------------------------电机到达前停止点检测函数----------------------------------*/
//电机到达前停止位，函数返回1；否则函数返回0
unsigned int GetPointForward(void)
{
	unsigned int Point;
	
	Point=0;
	
	if(Forward==0 || TimerOut==1)
		Point=1;

	return(Point);
}


/*----------------------------------电机到达后停止点检测函数----------------------------------*/
//电机到达后停止位，函数返回1；否则函数返回0
unsigned int GetPointBackward(void)
{
	unsigned int Point;
	
	Point=0;
	
	if(Backward==0 || TimerOut==1)
		Point=1;

	return(Point);
}





