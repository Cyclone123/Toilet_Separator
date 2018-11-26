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

//20180824修改版，根据出现的问题，调整精确定时时间，电机前进后退都是6秒，中间间隔4秒。
//为了防止限位器故障，导致电机不动作。现将逻辑修改为：电机现动作5秒，再判断到达限位开关。

//20180825修改版，添加上电后电机自动运行一个流程，用于确定电机电路是否正常。
//20180827修改，将定时时间由7秒改为6秒，原时间较长。
//20180923修改，还是用行程开关来确定电机工作位置。并把电机运行时间加长(15秒)，防止限位失灵，程序死循环
//20181008修改，电机运行时间过长，导致电机声音太大，现在减小时间到9秒

#include "STC12C20XX.h"

sbit Watchdog=P3^7;  		//看门狗输出

sbit Relay1=P3^0;  
sbit Relay2=P3^1;  

sbit Forward  =P1^7;  		//电机反转限位开关点
sbit Backward =P1^6; 		//电机零位限位开关点 
sbit Button   =P1^4;  

sbit InOut1=P1^0;  
sbit InOut2=P1^1;  
sbit InOut3=P1^2;  
sbit InOut4=P1^3;  


unsigned int Timer_1mS;
unsigned int Timer_1S;
unsigned int TimerOut;

void Delay_ms (unsigned int a);
void Delay_4S(void);
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

	Timer_1mS= 0 ;
	Timer_1S = 0 ;
	TimerOut = 0 ;

	MotorMotionHome();
	Delay_4S();
	MotorMotionForward();
	Delay_4S();
	MotorStopMotion();
	Delay_4S();
	MotorMotionReverse();
	Delay_4S();

	while(1)
	{
		StepLight(1);
		while(!PutButtonDown());

		StepLight(2);
		MotorMotionForward();

		StepLight(3);
		MotorStopMotion();
		Delay_4S();

		StepLight(4);
		MotorMotionReverse();

		Delay_4S();
	}
}


/*-----------------------------------中断服务程序-----------------------------------*/
void Timer0_Rountine(void) interrupt 1 
{
	TH0=0xFE;	
	TL0=0x3E;				//重新装载定时器时基值

    Timer_1mS++;

	if (Timer_1mS==1000)
	{							//定时器循环计数1000次为一秒
		Timer_1mS=0;			//次数清零，从新循环计数
		Timer_1S++;				//秒加一
		Watchdog=~Watchdog;		//喂狗
	}

	if(Timer_1S >= 9)	//当电机运行时间超过此设定时间后，置位标志位TimeOut.
	{
		Timer_1S = 0;
		TimerOut = 1 ;
	}	
}


/*----------------------------------定时器初始化函数----------------------------------*/
void Timer0Init (void)
{   
	TMOD=0x01;				//定时器工作方式1:16位

	TH0=0xFE;	
	TL0=0x3E;					//定时时间1ms
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

	InOut1=1;  
	InOut2=1;  
	InOut3=1;  
	InOut4=1;  
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
void Delay_4S()
{
	Delay_ms(1000);
	//Delay_ms(1000);
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
		case 1:	InOut1=0;
				InOut2=1;
				InOut3=1;
				InOut4=1;
				break;
		case 2:	InOut1=1;
				InOut2=0;
				InOut3=1;
				InOut4=1;
				break;
		case 3:	InOut1=1;
				InOut2=1;
				InOut3=0;
				InOut4=1;
				break;
		case 4:	InOut1=1;
				InOut2=1;
				InOut3=1;
				InOut4=0;
				break;
		default:InOut1=1;
				InOut2=1;
				InOut3=1;
				InOut4=1;
	}
}



/*----------------------------------电机向前动作到停止点函数----------------------------------*/
void MotorMotionForward(void)
{
	Relay1 =1;
	Relay2 =0;

	Timer_1mS=0;
	Timer_1S =0;
	TimerOut =0;

	while(Timer_1S<5);

	while(!GetPointForward());

	Relay1 =1;
	Relay2 =1;
}


/*----------------------------------电机向后动作到停止点函数----------------------------------*/
void MotorMotionReverse(void)
{
	Relay1 =0;
	Relay2 =1;

	Timer_1mS=0;
	Timer_1S =0;
	TimerOut =0;

	while(Timer_1S<5);

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

	Relay1 =0;
	Relay2 =1;

	Timer_1mS=0;
	Timer_1S =0;
	TimerOut =0;

	while(!GetPointBackward());

	Relay1 =1;
	Relay2 =1;

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





