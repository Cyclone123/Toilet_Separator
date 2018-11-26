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

//20180824�޸İ棬���ݳ��ֵ����⣬������ȷ��ʱʱ�䣬���ǰ�����˶���6�룬�м���4�롣
//Ϊ�˷�ֹ��λ�����ϣ����µ�����������ֽ��߼��޸�Ϊ������ֶ���5�룬���жϵ�����λ���ء�

//20180825�޸İ棬����ϵ�����Զ�����һ�����̣�����ȷ�������·�Ƿ�������
//20180827�޸ģ�����ʱʱ����7���Ϊ6�룬ԭʱ��ϳ���
//20180923�޸ģ��������г̿�����ȷ���������λ�á����ѵ������ʱ��ӳ�(15��)����ֹ��λʧ�飬������ѭ��
//20181008�޸ģ��������ʱ����������µ������̫�����ڼ�Сʱ�䵽9��

#include "STC12C20XX.h"

sbit Watchdog=P3^7;  		//���Ź����

sbit Relay1=P3^0;  
sbit Relay2=P3^1;  

sbit Forward  =P1^7;  		//�����ת��λ���ص�
sbit Backward =P1^6; 		//�����λ��λ���ص� 
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


/*--------------------------------------������--------------------------------------*/
void main(void)
{
	Timer0Init(); 

	ET0=1;			//����ʱ���ж�
	EA=1;			//�����ж�
	TR0=1;			//������ʱ��

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


/*-----------------------------------�жϷ������-----------------------------------*/
void Timer0_Rountine(void) interrupt 1 
{
	TH0=0xFE;	
	TL0=0x3E;				//����װ�ض�ʱ��ʱ��ֵ

    Timer_1mS++;

	if (Timer_1mS==1000)
	{							//��ʱ��ѭ������1000��Ϊһ��
		Timer_1mS=0;			//�������㣬����ѭ������
		Timer_1S++;				//���һ
		Watchdog=~Watchdog;		//ι��
	}

	if(Timer_1S >= 9)	//���������ʱ�䳬�����趨ʱ�����λ��־λTimeOut.
	{
		Timer_1S = 0;
		TimerOut = 1 ;
	}	
}


/*----------------------------------��ʱ����ʼ������----------------------------------*/
void Timer0Init (void)
{   
	TMOD=0x01;				//��ʱ��������ʽ1:16λ

	TH0=0xFE;	
	TL0=0x3E;					//��ʱʱ��1ms
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

	InOut1=1;  
	InOut2=1;  
	InOut3=1;  
	InOut4=1;  
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
void Delay_4S()
{
	Delay_ms(1000);
	//Delay_ms(1000);
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



/*----------------------------------�����ǰ������ֹͣ�㺯��----------------------------------*/
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


/*----------------------------------����������ֹͣ�㺯��----------------------------------*/
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


/*----------------------------------���ֹͣ��������----------------------------------*/
void MotorStopMotion(void)
{
	Relay1 =1;
	Relay2 =1;
}



/*----------------------------------����ص���ʼλ�ú���----------------------------------*/
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





