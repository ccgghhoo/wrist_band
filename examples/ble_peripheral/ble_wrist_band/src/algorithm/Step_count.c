
#include "app_typedef.h"
#include "app_fir_filter.h"
#include "Step_count.h"


#if 0
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#define DEBUG				NRF_LOG_DEBUG
#else
#define DEBUG(...) 
#endif 
#define   AVERAGE_COUNT    15
#define   K_LIMIT          5
#define   XXX              4


S_STEP_COUNTER SStepCounter = {
   0    	/* 计步器计步数	*/
  ,0        /* 卡路里*/
  ,0        /* 路程 */
  ,0        /* 速度*/
  ,0		/* 计步时间*/
  ,170      /* 身高设定*/
  ,60		/* 体重设定*/
  ,150		/* 力度设定*/
  ,3		/* 灵敏度设定*/
};

/*计步要保存的数据元素变量定义*/
StepData StepDataSave;

/*
 *	函数名：		 	 filterlp
 *	函数输入参数： sample:	滤波采样值
 *	函数返回：		 z：			已滤波后的值
 *	涉及函数：		【空】
 *
 *	主要功能：		 FIR 低通滤波器
 */	
int filterlp(int sample)                  								  
{
	float32_t src;
	float32_t dst;
	int in_data;
	int sum = 0;
	static int data[AVERAGE_COUNT] = {0};

	src = sample;
	app_fir_filter(&src, &dst);
	in_data = (int)dst;
	
	for (int i = 0; i < AVERAGE_COUNT - 1; i++)
	{
		data[i] = data[i + 1];
	}
	data[AVERAGE_COUNT - 1] = in_data;
	
	for (int i = 0; i < AVERAGE_COUNT; i++)
	{
		sum += data[i];
	}
	
	return (sum / AVERAGE_COUNT);
}
										  
/*
 *	函数名：		 	 xielv
 *	函数输入参数： newval:	采样值
 *	函数返回：		 cha：		斜率值
 *	涉及函数：		【空】
 *
 *	主要功能：		 求斜率（微分滤波）
 */	
int xielv(int newval)													  
{
	static int pre = 0;
	static int cha = 0;	
													   
	cha = newval - pre;	//差值=新数据-前一个数据
	pre = newval;//新数据赋值给前一个数据
	return cha;	//返回差值累加和
}

/*
 *	函数名：		 	 area
 *	函数输入参数： k:		斜率值
 *	函数返回：		 返回0或者1
 *	涉及函数：		【空】
 *
 *	主要功能：		 计算波峰波谷面积并且返回是否增加步数真假值
 */	
signed char area(signed int k)											  
{
	static signed 	int  time_max,time_min,k_max,k_min,S_max,S_min,S;//波峰持续时间，波谷持续时间，波峰最大值，波谷最大值，波峰面积，波谷面积，波峰+波谷面积
	static unsigned char flag = 0;//状态标志位
	static unsigned int  zero_num;//0持续的数量

  switch(flag)
	{
	 	case 0:	//当为“0”态时，数据清零
				time_max = time_min = 0;								  
		        k_max = k_min = 0;
				zero_num = 0;
				if(k >= K_LIMIT)//当斜率大于0时，状态标志位变成“1”
				{
				    flag = 1;
				}
				 break;

	   	case 1: //当标志位为“1”态时（主要是针对斜率大于0）
				if(k >= K_LIMIT)//当斜率大于0时，波峰开始计时
				{
				  	time_max++;
					if(k>k_max)//找出波峰的峰值
					{
					 	k_max = k;
					}
				}
			    else if((k < K_LIMIT) && (k > -K_LIMIT))//当斜率由大于0变为等于0时，并且0出现不超过3次时，波峰计时继续增加
				{
				    time_max++;
				 	zero_num++;
					if(zero_num>4)//当0出现5次之后，进入“2”态
					{
					 	flag = 2;
					}
			    }
				else//当斜率由大于0变成小于0时，进入“3”态
				{
					flag = 3;
				}
				 break;

		 case 2://当标志位为“2”时 （主要针对斜率等于0）
		 		 if((k < K_LIMIT) && (k > -K_LIMIT))//当斜率等于0时，0的数量计数
		 		 {
				  	zero_num++;
					if(zero_num>ZEROTIME)//当0的数量大于ZEROTIME（20）时，标志位变为“0”
					{
					 	flag = 0;
					}
				 }
				 else if(k >= K_LIMIT)//当斜率由等于0变成大于0时，标志位变为“0”
				 {
					flag = 0;	 
			     }
				 else//当斜率由等于0变成小于0时，0的计数清零，标志位变为“3”
				 {
					zero_num = 0;
					flag = 3;
				 }
				  break;

         case 3://当标志位为“3”时（主要针对斜率小于0）
		 		if(k <= -K_LIMIT)//当斜率小于0时，统计波谷时间，找出波谷最大值
				 {
				  	time_min++;
					if(k<k_min)
					{
					 	k_min = k;
					}
				 }
				 else if((k < K_LIMIT) && (k > -K_LIMIT))//当斜率由小于0变成等于0时，并且0出现不超过3次时，波谷计时继续增加
				 {
				    time_min++;
				 	zero_num++;
					if(zero_num>4)//当0出现5次之后，进入“4”态
					{
					 	flag = 4;
					}
				 }
				 else//当斜率由小于0变成大于0时，进入“4”态
				 {
					flag = 4;
				 }
				 break;

		 case 4://当顺序出现波峰—>波谷时，或者波峰->0持续时间不超过20个->波谷时
		 		 S_max = time_max*k_max;//计算波峰的面积
		         S_min = -(time_min*k_min);//计算波谷的面积
				 S = S_max + S_min;	//计算波峰+波谷的面积
				 
				 /*判断波峰、波谷持续时间分别是否大于CMPMIN并且小于CMPMAX；判断波峰+波谷持续时间是否大于TIME
				   判断波峰和波谷的面积是否小于SMIN；判断波峰+波谷的面积是否大于SMAX；判断波峰的最大值是否大于KMAX*/
		 		 if((time_max>SStepCounter.SCSetUpMenuSValue)&&(time_min>SStepCounter.SCSetUpMenuSValue)&&(time_max<CMPMAX)&&(time_min<CMPMAX)	
		  			&&(time_min+time_max>=TIME)&&(S_max>SMIN)&&(S_min>SMIN)&&(S>SMAX)&&(k_max>=SStepCounter.SCSetUpMenuEValue))					
					{
					 	flag = 0;
					 	return 1;//以上的条件都满足，返回真，记步+1
					}
				 else//不满足条件，回状态“0”，清零数据
				 {
				 	flag = 0;
				 }
				 break;

		default : flag = 0;
		          break;
	}
	return 0;
}

/*
 *	函数名：		 	 Speed_and_Calorie
 *	函数输入参数： S_STEP_COUNTER *SStepCounter
 *	函数返回：		 void
 *	涉及函数：		【空】
 *
 *	主要功能：		 通过输入当前步数、身高和体重，返回卡路里、速度、和距离
 */	
void Speed_and_Calorie(S_STEP_COUNTER *SStepCounter)
{
	float steplength = 0; 
	float heigh;									  			    	
	static unsigned int pre_step = 0;
	
	unsigned int step_2s = 0;
	float distance_2s = 0;
	float Calorie_2s = 0;
	
	heigh =(float)SStepCounter->SCSetUpMenuHValue/100.0;//身高

	if(SStepCounter->StepCount == 0)//没有步数的时候
	{
		pre_step = 0;
		SStepCounter->distance = 0;
		SStepCounter->Calorie  =0;
	}
	else if	(SStepCounter->StepCount < pre_step)//步数异常
	{
		pre_step = SStepCounter->StepCount;	
	}
	step_2s	= SStepCounter->StepCount - pre_step;//就算出两秒内产生的步数

	/*判断两秒内的步伐长度*/
	switch(step_2s)
	{									 
		case 0:	
			steplength = 0;
			break;
		case 1: 
			steplength =  heigh/5.0;
			break;
		case 2:	
			steplength =  heigh/5.0;
			break;
		case 3:	
			steplength =  heigh/4.0;
			break;
		case 4:	
			steplength =  heigh/3.0;
			break;
		case 5:	
			steplength =  heigh/2.0;
			break;
		case 6:	
			steplength =  heigh/1.2;
			break;
		case 7:	
			steplength =  heigh;
			break;	
		case 8:	
			steplength =  heigh;
			break;			   
		default: 
			steplength =  heigh*1.2;
			break;
	}

	/*速度=2s的步数*身高除以2（m/s）*/
	SStepCounter->speed =step_2s*steplength/2.0;
	
	/*2s内跑过的路程、卡路里*/
	distance_2s = step_2s*steplength;
	//卡路里(KCAL)=1.25*跑步速度（km/h）=1.25*速度（m/s）*3600/1000=4.5*速度（m/s）
	//卡路里=4.5*速度*体重/1800=速度*体重/400
	Calorie_2s = (SStepCounter->speed*SStepCounter->SCSetUpMenuWValue)/400.0;

	/*计算出总的路程*/
	SStepCounter->distance += distance_2s;
	/*计算出总的卡路里*/
	SStepCounter->Calorie += Calorie_2s;
	
	/*更新旧步数*/
	pre_step = SStepCounter->StepCount;

	/*运动要保存的路程、卡路里数据获取*/
	StepDataSave.StepMemoryDistance += distance_2s;
	StepDataSave.StepMemoryCalories += Calorie_2s;
}

/************************************************* 
*      Function:	GetAcceleration 
*      Description:获得三轴综合加速度值
*      Calls:	// 被本函数调用的函数清单 
*      Called By:	// 调用本函数的函数清单 
*      Input:	void
*      Output:	// 对输出参数的说明。 
*      Return:	void
*      Others:	// 其它说明 
*************************************************/
int GetAcceleration(unsigned char *step_data, unsigned char len)
{
#if 0
	signed int XOUT_Value,YOUT_Value,ZOUT_Value;//X、Y、Z三轴计算得出的数值
	signed int 	XOUT_K,YOUT_K,ZOUT_K; //X、Y、Z三轴多少次开方得出的数值
	signed int 	Acceleration;//加速度数值									  	 			  //计算得出的加速度值

	/*X轴、Y轴、Z轴的数据获取*/ 
	XOUT_Value = ReadGSensorX();
	YOUT_Value = ReadGSensorY();												
	ZOUT_Value = ReadGSensorZ();
	//printf("XOUT_Value=%d,YOUT_Value=%d,ZOUT_Value=%d\n\r",XOUT_Value,YOUT_Value,ZOUT_Value);

	/*数据的开方*/
	XOUT_K = pow(XOUT_Value,2);//计算x^2
	YOUT_K = pow(YOUT_Value,2);//计算y^2
	ZOUT_K = pow(ZOUT_Value,2);//计算z^2

	/*加速度等于(x^2+y^2+z^2)后开膜，再减去1024（1g的值）*/	
	Acceleration = sqrt(XOUT_K + YOUT_K + ZOUT_K)-1024;	

	/*返回加速度值*/
	return Acceleration;
#else
	unsigned char 	x[2],y[2],z[2];	//X、Y、Z三轴高低位数据
	signed char xh,yh,zh;				
	signed int 	x_k,y_k,z_k,x_value,y_value,z_value; //X、Y、Z三轴计算值
	signed int 	Acceleration;	//计算得出的加速度值

	//I2C_Read(KX022_IIC, KX022_ADDR,XOUT, 2, x, 2);	//获得X、Y、Z值高低位数据
	//I2C_Read(KX022_IIC, KX022_ADDR,YOUT, 2, y, 2);						
	//I2C_Read(KX022_IIC, KX022_ADDR,ZOUT, 2, z, 2);

//	unsigned char step_data[6];

//	kx022_spi_master_read_sensor(step_data);
//	kx022_spi_master_read_buff(step_data);


	x[0] = step_data[1];//高8位
	x[1] = step_data[0];//低8位
	y[0] = step_data[3];//高8位
	y[1] = step_data[2];//低8位
	z[0] = step_data[5];//高8位
	z[1] = step_data[4];//低8位
	

	//三轴的高位有符号
	xh=x[0];	
	yh=y[0];
	zh=z[0];

	if((xh & 0x80) == 1)//X、Y、Z三轴的高位都是有符号
	{
		xh = (~xh) + 1;	
		x_value = ((x[1] | (xh << 8))>>XXX);	//16位的数据，有效位为12位，因此需要进行右移4位
		x_value = (~x_value) + 1; 
	}
	else  
	{
		x_value = ((x[1] | (xh << 8))>>XXX);
	}
		
	if((yh & 0x80) == 1)
	{
		yh = (~yh) + 1;	
		y_value = ((y[1] | (yh << 8))>>XXX);
		y_value = (~y_value) + 1; 
	}
	else  
	{
		y_value = ((y[1] | (yh << 8))>>XXX);	
	}
		
	if((zh & 0x80) == 1)
	{
		zh = (~zh) + 1;	
		z_value = ((z[1] | (zh << 8))>>XXX);
		z_value = (~z_value) + 1; 
	}
	else  
	{
		z_value = ((z[1] | (zh << 8))>>XXX);	
	}
	
	x_k = pow(x_value,2);	//计算x^2
	y_k = pow(y_value,2);	//计算y^2
	z_k = pow(z_value,2);	//计算z^2

	Acceleration = sqrt(x_k + y_k + z_k)-512;	
//	Acceleration = sqrt(x_k + y_k + z_k)-1024;	//加速度等于(x^2+y^2+z^2)后开膜，再减去1024（1g的值）

	
//	DEBUG("%d	%d	%d  ",x_value,y_value,z_value);
//	DEBUG("%d	%d	%d  ",x_k,y_k,z_k);
//	DEBUG("%d\r\n", Acceleration);
	return Acceleration;//返回加速度值 
#endif
}

/*
 *	函数名：		 	 STEP_count
 *	函数输入参数：【空】
 *	函数返回：		 int	
 *	涉及函数：		 int 	filterlp(int sample)  
 *					 			 int 	xielv(int newval)
 *					 			 signed char area(signed int k)
 *					 			 void 	I2C_Ctrl(I2C_TypeDef *I2cPort, uint8_t start, uint8_t stop, uint8_t ack)
 *
 *	主要功能：		 定时6ms取数据，并把数据波形进行FIR滤波、求斜率和对数据波形进行面积计算，得出步数
 */	
int StepConter(unsigned char *p_src, unsigned char len)//传入采样原始数据
{															  		
	signed int data_new;	//刷新的加速度值存放变量
	signed int l_data;		//滤波之后的加速度值
	signed int b_avreage;	//计算斜率之后的加速度值
	
	data_new  = GetAcceleration(p_src, len);	//获得三轴综合加速度值
	l_data 	  = filterlp(data_new);	//滤波
	b_avreage = xielv(l_data);//求斜率（微分）

	if(b_avreage==1||b_avreage==-1)	//滤除一些小的抖动
	{
			b_avreage = 0;
	}

	if(area(b_avreage))	//通过计算面积并且返回是否增加步数
	{
		return 1;
	}	
		
	return 0;	
}




