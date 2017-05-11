#include<reg52.h>

#define uchar unsigned char
#define uint unsigned int
#define GPIO_DIG P0
#define GPIO_KEY P1
uint STH0,STL0,voice[6]={0},code sound[]={64021,64103,64185,64260,
				 					   	64400,64463,64524,64580,
				 						64684,64732,64777,64820,
							 			64898,64934,64968,64994,
				 						65030,65058,65085,65110,
							   			 65157,65178,65198,65217};//吉他前3品的所有音阶?若T值太低会发不出音？ 
uchar Time,KeyValue,Irvalue,code smgduan[23]={0x39,0x3d,0x79,0x71,0xde,0x77,0x3f,0x06,0x5b,
														0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,
														0x39,0x5e,0x79,0x71,0X76};//显示c~a和弦
sbit LSA=P2^2;
sbit LSB=P2^3;
sbit LSC=P2^4;
sbit Beep=P1^5;
uint count=0;//count用于主函数 

//初始化函数,不开TR0
void init(void)
{
    TMOD=0x01;
	EA=1;
	ET0=1;
}

//延时函数 
void delay(uint i)
{
	while(i--);	
}
void yjjudge(uchar i); 
void KeyDown(void);
void Toperate(void);
void display(void); 
void Irray(void);
//主函数 
void main()
{		
	while(1)
	{
		uint j=100;
		init();
		KeyDown();
		yjjudge(KeyValue);//键值输和弦 
		Irray();	//读取红外线值
		Toperate();//将红外键值转换成定时器初值
		while(j--)
		{	
			display();
		}//数码管亮时会调用P2，因此不能在使用数码管时读取红外键值 
		while(Irvalue!=0)
		{
			TH0=STH0;
			TL0=STL0;
 		    TR0=1;
			delay(10000000);//每当有弦拨动时输出一定时间的音然后停止
			TR0=0;
			Beep=1;
			break;
		}
		while(1)
		{
			if(P2^0!=0)//重新赋值的条件为1)和弦改变2)音阶改变
			break;
		}
	}
}

//键盘扫描函数 
void KeyDown(void)
{
	char a=0;
	GPIO_KEY=0x0f;
	if(GPIO_KEY!=0x0f)//读取按键是否按下
	{
		delay(1000);//延时10ms进行消抖
		if(GPIO_KEY!=0x0f)//再次检测键盘是否按下
		{	
			//测试列
			GPIO_KEY=0X0F;
			switch(GPIO_KEY)
			{
				case(0X07):	KeyValue=0;break;
				case(0X0b):	KeyValue=1;break;
				case(0X0d): KeyValue=2;break;
				case(0X0e):	KeyValue=3;break;
			}
			//测试行
			 GPIO_KEY=0XF0;
			switch(GPIO_KEY)
			{
				case(0X70):	KeyValue=KeyValue;break;
				case(0Xb0):	KeyValue=KeyValue+4;break;
				case(0Xd0): KeyValue=KeyValue+8;break;
				case(0Xe0):	KeyValue=KeyValue+12;break;
			}
			while((a<50)&&(GPIO_KEY!=0xf0))	 //检测按键松手检测
			{
				delay(100);
				a++;
			}
		}
	}
}
// 和弦判断
void yjjudge(uchar i)
{
	switch(i)
	{
		case 0:	
				voice[0]=sound[0];
				voice[2]=sound[8];
				voice[5]=sound[20];
				voice[1]=sound[5];//2弦1品 
				voice[3]=sound[14];//4弦2品 
				voice[4]=sound[19];//5弦3品 
			break;//C和弦 
		case 1:	
				voice[1]=sound[4];
				voice[2]=sound[8];
				voice[3]=sound[12];
				voice[0]=sound[3];//1弦3品 
				voice[4]=sound[18];//5弦2品 
				voice[5]=sound[23];//6弦3品 
			break;//G和弦 
		case 2:	
				voice[0]=sound[0];
				voice[1]=sound[4];
				voice[2]=sound[8];
				voice[5]=sound[20];
				voice[3]=sound[14];//4弦2品 
				voice[4]=sound[19];//5弦3品 
			break;//E和弦 
		case 3:	
				voice[0]=sound[2];
				voice[1]=sound[5];//2弦1品 
				voice[2]=sound[10];//3弦2品 
				voice[3]=sound[15];//4弦3品 
				voice[4]=sound[19];//5弦3品 
				voice[5]=sound[21];//6弦1品 
			break;//F和弦 
		case 4:	
				voice[3]=sound[12];
				voice[4]=sound[16];
				voice[5]=sound[20];
				voice[0]=sound[2];//1弦2品 
				voice[1]=sound[7];//2弦3品 
				voice[2]=sound[10];//3弦2品 
			break;//D和弦 
		case 5:
			    voice[0]=sound[0];
				voice[4]=sound[16];
				voice[5]=sound[20];
	 			voice[1]=sound[6];//2弦2品 
				voice[2]=sound[10];//3弦2品 
				voice[3]=sound[14];//4弦2品 
			break;//A和弦 
		default:break;
	}
}
//数码管显示和弦及拨弦的值
void display(void)
{
	int j; 
	for(j=0;j<=1;j++)
	{	LSA=0; //给一个数码管提供位选
		LSB=j;
		LSC=1;
		switch(j)
		{
			case 0:
			if(KeyValue<=5)	
			GPIO_DIG=smgduan[KeyValue];
			break;
			case 1:	
			GPIO_DIG=smgduan[(Irvalue)+6];
			break;
		}
		delay(100);
		P0=0x00;//消隐
	}
}

//定时器初值
void Toperate(void)
{
	switch(Irvalue)
	{
		case 1:
			STH0=(voice[0])/256;
			STL0=(voice[0])%256;
			break;
		case 2:
			STH0=(voice[1])/256;
			STL0=(voice[1])%256;
			break;
		case 3:
			STH0=(voice[2])/256;
			STL0=(voice[2])%256;
			break;
		case 4:
			STH0=(voice[3])/256;
			STL0=(voice[3])%256;
			break;
		case 5:
			STH0=(voice[4])/256;
			STL0=(voice[4])%256;
			break;
		case 6:
			STH0=(voice[5])/256;
			STL0=(voice[5])%256;
			break;
		default:break;
	}
}
void Irray(void)//读取红外键值
{
	if(P2^0==0)
		Irvalue=1;
	else if(P2^1==0)
		Irvalue=2;
	else if(P2^2==0)
		Irvalue=3;
	else if(P2^3==0)
		Irvalue=4;
	else if(P2^4==0)
		Irvalue=5;
	else if(P2^5==0)
		Irvalue=6;
	else
		Irvalue=0;
}

//定时器
void time0_int(void) interrupt 1 using 0
{
	
	TH0=STH0;
	TL0=STL0;
	Beep=~Beep;
}













