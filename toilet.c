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


sbit Watchdog=P3^7;  //���Ź����

sbit Relay1=P3^0;  
sbit Relay2=P3^1;  

//��һ��ĵ�·����г̿��غͰ�ť�Ķ���
//sbit Forward  =P1^4;  
//sbit Backward =P1^5;  
//sbit Button   =P1^7;  

sbit Forward  =P1^7;  		//�����ת��λ���ص�
sbit Backward =P1^6; 		//�����λ��λ���ص� 
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


/*--------------------------------------������--------------------------------------*/
void main(void)
{
	Timer0Init(); 

	ET0=1;			//����ʱ���ж�
	EA=1;			//�����ж�
	TR0=1;			//������ʱ��

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


/*-----------------------------------�жϷ������-----------------------------------*/
void Timer0_Rountine(void) interrupt 1 
{
	TH0=0x3C;			//����װ�ض�ʱ��ʱ��ֵ
	TL0=0xB0;

    Timer_50mS++;

	if (Timer_50mS==20)
	{					//��ʱ��ѭ������20��Ϊһ��
		Timer_50mS=0;	//�������㣬����ѭ������
		Timer_1S++;		//���һ
	}

	if(Timer_1S >= 20)	//���������ʱ�䳬�����趨ʱ�����λ��־λTimeOut.
	{
		Timer_1S = 0;
		TimerOut = 1 ;
	}

	Watchdog=~Watchdog;		//ι��
}


/*----------------------------------��ʱ����ʼ������----------------------------------*/
void Timer0Init (void)
{   
	TMOD=0x01;				//��ʱ��������ʽ1:16λ

	TH0=(65536-50000)/256;	//��ʱʱ��50ms
	TL0=(65536-50000)%256;
} 


/*----------------------------------�̵���/����ڳ�ʼ������----------------------------------*/
void RelayOutInit (void)
{   
	P3M0=0x00;				//P3�ڹ�����ʽ:ǿ�������
	P3M1=0xff;	

	Relay1=1;  
	Relay2=1;  
} 


/*----------------------------------���в���ָʾLED������ڳ�ʼ������----------------------------------*/
void StepLightInit(void)
{
	P1M0=0x00;				//P1�ڹ�����ʽ:ǿ�������
	P1M1=0x0f;	

	InOut1=0;  
	InOut2=0;  
	InOut3=0;  
	InOut4=0;  
}

/*-------------------------------------��ʱ����-------------------------------------*/
void Delay_ms (unsigned int a)
{
	unsigned int i;
	while( --a != 0)
	{
		for(i = 0; i < 600; i++);
	}
}


/*-------------------------------------����ʱ����-------------------------------------*/
void Delay_2S()
{
	Delay_ms(1000);
	Delay_ms(1000);
}


/*----------------------------------������ť��⺯��----------------------------------*/
//����Ŧ���£���������1�����򣬺�������0
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


/*----------------------------------��������ָʾLED��ʾ����----------------------------------*/
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



/*----------------------------------�����ǰ������ֹͣ�㺯��----------------------------------*/
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


/*----------------------------------����������ֹͣ�㺯��----------------------------------*/
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


/*----------------------------------���ֹͣ��������----------------------------------*/
void MotorStopMotion(void)
{
	Relay1 =1;
	Relay2 =1;
}



/*----------------------------------����ص���ʼλ�ú���----------------------------------*/
void MotorMotionHome(void)
{
	if(!GetPointBackward())
	{
		MotorMotionReverse();
	}
}


/*----------------------------------�������ǰֹͣ���⺯��----------------------------------*/
//�������ǰֹͣλ����������1������������0
unsigned int GetPointForward(void)
{
	unsigned int Point;
	
	Point=0;
	
	if(Forward==0 || TimerOut==1)
		Point=1;

	return(Point);
}


/*----------------------------------��������ֹͣ���⺯��----------------------------------*/
//��������ֹͣλ����������1������������0
unsigned int GetPointBackward(void)
{
	unsigned int Point;
	
	Point=0;
	
	if(Backward==0 || TimerOut==1)
		Point=1;

	return(Point);
}





