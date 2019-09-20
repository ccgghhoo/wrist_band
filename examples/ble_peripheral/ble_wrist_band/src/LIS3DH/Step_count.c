
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
   0    	/* �Ʋ����Ʋ���	*/
  ,0        /* ��·��*/
  ,0        /* ·�� */
  ,0        /* �ٶ�*/
  ,0		/* �Ʋ�ʱ��*/
  ,170      /* ����趨*/
  ,60		/* �����趨*/
  ,150		/* �����趨*/
  ,3		/* �������趨*/
};

/*�Ʋ�Ҫ���������Ԫ�ر�������*/
StepData StepDataSave;

/*
 *	��������		 	 filterlp
 *	������������� sample:	�˲�����ֵ
 *	�������أ�		 z��			���˲����ֵ
 *	�漰������		���ա�
 *
 *	��Ҫ���ܣ�		 FIR ��ͨ�˲���
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
 *	��������		 	 xielv
 *	������������� newval:	����ֵ
 *	�������أ�		 cha��		б��ֵ
 *	�漰������		���ա�
 *
 *	��Ҫ���ܣ�		 ��б�ʣ�΢���˲���
 */	
int xielv(int newval)													  
{
	static int pre = 0;
	static int cha = 0;	
													   
	cha = newval - pre;	//��ֵ=������-ǰһ������
	pre = newval;//�����ݸ�ֵ��ǰһ������
	return cha;	//���ز�ֵ�ۼӺ�
}

/*
 *	��������		 	 area
 *	������������� k:		б��ֵ
 *	�������أ�		 ����0����1
 *	�漰������		���ա�
 *
 *	��Ҫ���ܣ�		 ���㲨�岨��������ҷ����Ƿ����Ӳ������ֵ
 */	
signed char area(signed int k)											  
{
	static signed 	int  time_max,time_min,k_max,k_min,S_max,S_min,S;//�������ʱ�䣬���ȳ���ʱ�䣬�������ֵ���������ֵ������������������������+�������
	static unsigned char flag = 0;//״̬��־λ
	static unsigned int  zero_num;//0����������

  switch(flag)
	{
	 	case 0:	//��Ϊ��0��̬ʱ����������
				time_max = time_min = 0;								  
		        k_max = k_min = 0;
				zero_num = 0;
				if(k >= K_LIMIT)//��б�ʴ���0ʱ��״̬��־λ��ɡ�1��
				{
				    flag = 1;
				}
				 break;

	   	case 1: //����־λΪ��1��̬ʱ����Ҫ�����б�ʴ���0��
				if(k >= K_LIMIT)//��б�ʴ���0ʱ�����忪ʼ��ʱ
				{
				  	time_max++;
					if(k>k_max)//�ҳ�����ķ�ֵ
					{
					 	k_max = k;
					}
				}
			    else if((k < K_LIMIT) && (k > -K_LIMIT))//��б���ɴ���0��Ϊ����0ʱ������0���ֲ�����3��ʱ�������ʱ��������
				{
				    time_max++;
				 	zero_num++;
					if(zero_num>4)//��0����5��֮�󣬽��롰2��̬
					{
					 	flag = 2;
					}
			    }
				else//��б���ɴ���0���С��0ʱ�����롰3��̬
				{
					flag = 3;
				}
				 break;

		 case 2://����־λΪ��2��ʱ ����Ҫ���б�ʵ���0��
		 		 if((k < K_LIMIT) && (k > -K_LIMIT))//��б�ʵ���0ʱ��0����������
		 		 {
				  	zero_num++;
					if(zero_num>ZEROTIME)//��0����������ZEROTIME��20��ʱ����־λ��Ϊ��0��
					{
					 	flag = 0;
					}
				 }
				 else if(k >= K_LIMIT)//��б���ɵ���0��ɴ���0ʱ����־λ��Ϊ��0��
				 {
					flag = 0;	 
			     }
				 else//��б���ɵ���0���С��0ʱ��0�ļ������㣬��־λ��Ϊ��3��
				 {
					zero_num = 0;
					flag = 3;
				 }
				  break;

         case 3://����־λΪ��3��ʱ����Ҫ���б��С��0��
		 		if(k <= -K_LIMIT)//��б��С��0ʱ��ͳ�Ʋ���ʱ�䣬�ҳ��������ֵ
				 {
				  	time_min++;
					if(k<k_min)
					{
					 	k_min = k;
					}
				 }
				 else if((k < K_LIMIT) && (k > -K_LIMIT))//��б����С��0��ɵ���0ʱ������0���ֲ�����3��ʱ�����ȼ�ʱ��������
				 {
				    time_min++;
				 	zero_num++;
					if(zero_num>4)//��0����5��֮�󣬽��롰4��̬
					{
					 	flag = 4;
					}
				 }
				 else//��б����С��0��ɴ���0ʱ�����롰4��̬
				 {
					flag = 4;
				 }
				 break;

		 case 4://��˳����ֲ��塪>����ʱ�����߲���->0����ʱ�䲻����20��->����ʱ
		 		 S_max = time_max*k_max;//���㲨������
		         S_min = -(time_min*k_min);//���㲨�ȵ����
				 S = S_max + S_min;	//���㲨��+���ȵ����
				 
				 /*�жϲ��塢���ȳ���ʱ��ֱ��Ƿ����CMPMIN����С��CMPMAX���жϲ���+���ȳ���ʱ���Ƿ����TIME
				   �жϲ���Ͳ��ȵ�����Ƿ�С��SMIN���жϲ���+���ȵ�����Ƿ����SMAX���жϲ�������ֵ�Ƿ����KMAX*/
		 		 if((time_max>SStepCounter.SCSetUpMenuSValue)&&(time_min>SStepCounter.SCSetUpMenuSValue)&&(time_max<CMPMAX)&&(time_min<CMPMAX)	
		  			&&(time_min+time_max>=TIME)&&(S_max>SMIN)&&(S_min>SMIN)&&(S>SMAX)&&(k_max>=SStepCounter.SCSetUpMenuEValue))					
					{
					 	flag = 0;
					 	return 1;//���ϵ����������㣬�����棬�ǲ�+1
					}
				 else//��������������״̬��0������������
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
 *	��������		 	 Speed_and_Calorie
 *	������������� S_STEP_COUNTER *SStepCounter
 *	�������أ�		 void
 *	�漰������		���ա�
 *
 *	��Ҫ���ܣ�		 ͨ�����뵱ǰ��������ߺ����أ����ؿ�·��ٶȡ��;���
 */	
void Speed_and_Calorie(S_STEP_COUNTER *SStepCounter)
{
	float steplength = 0; 
	float heigh;									  			    	
	static unsigned int pre_step = 0;
	
	unsigned int step_2s = 0;
	float distance_2s = 0;
	float Calorie_2s = 0;
	
	heigh =(float)SStepCounter->SCSetUpMenuHValue/100.0;//���

	if(SStepCounter->StepCount == 0)//û�в�����ʱ��
	{
		pre_step = 0;
		SStepCounter->distance = 0;
		SStepCounter->Calorie  =0;
	}
	else if	(SStepCounter->StepCount < pre_step)//�����쳣
	{
		pre_step = SStepCounter->StepCount;	
	}
	step_2s	= SStepCounter->StepCount - pre_step;//����������ڲ����Ĳ���

	/*�ж������ڵĲ�������*/
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

	/*�ٶ�=2s�Ĳ���*��߳���2��m/s��*/
	SStepCounter->speed =step_2s*steplength/2.0;
	
	/*2s���ܹ���·�̡���·��*/
	distance_2s = step_2s*steplength;
	//��·��(KCAL)=1.25*�ܲ��ٶȣ�km/h��=1.25*�ٶȣ�m/s��*3600/1000=4.5*�ٶȣ�m/s��
	//��·��=4.5*�ٶ�*����/1800=�ٶ�*����/400
	Calorie_2s = (SStepCounter->speed*SStepCounter->SCSetUpMenuWValue)/400.0;

	/*������ܵ�·��*/
	SStepCounter->distance += distance_2s;
	/*������ܵĿ�·��*/
	SStepCounter->Calorie += Calorie_2s;
	
	/*���¾ɲ���*/
	pre_step = SStepCounter->StepCount;

	/*�˶�Ҫ�����·�̡���·�����ݻ�ȡ*/
	StepDataSave.StepMemoryDistance += distance_2s;
	StepDataSave.StepMemoryCalories += Calorie_2s;
}

/************************************************* 
*      Function:	GetAcceleration 
*      Description:��������ۺϼ��ٶ�ֵ
*      Calls:	// �����������õĺ����嵥 
*      Called By:	// ���ñ������ĺ����嵥 
*      Input:	void
*      Output:	// �����������˵���� 
*      Return:	void
*      Others:	// ����˵�� 
*************************************************/
int GetAcceleration(unsigned char *step_data, unsigned char len)
{
#if 0
	signed int XOUT_Value,YOUT_Value,ZOUT_Value;//X��Y��Z�������ó�����ֵ
	signed int 	XOUT_K,YOUT_K,ZOUT_K; //X��Y��Z������ٴο����ó�����ֵ
	signed int 	Acceleration;//���ٶ���ֵ									  	 			  //����ó��ļ��ٶ�ֵ

	/*X�ᡢY�ᡢZ������ݻ�ȡ*/ 
	XOUT_Value = ReadGSensorX();
	YOUT_Value = ReadGSensorY();												
	ZOUT_Value = ReadGSensorZ();
	//printf("XOUT_Value=%d,YOUT_Value=%d,ZOUT_Value=%d\n\r",XOUT_Value,YOUT_Value,ZOUT_Value);

	/*���ݵĿ���*/
	XOUT_K = pow(XOUT_Value,2);//����x^2
	YOUT_K = pow(YOUT_Value,2);//����y^2
	ZOUT_K = pow(ZOUT_Value,2);//����z^2

	/*���ٶȵ���(x^2+y^2+z^2)��Ĥ���ټ�ȥ1024��1g��ֵ��*/	
	Acceleration = sqrt(XOUT_K + YOUT_K + ZOUT_K)-1024;	

	/*���ؼ��ٶ�ֵ*/
	return Acceleration;
#else
	unsigned char 	x[2],y[2],z[2];	//X��Y��Z����ߵ�λ����
	signed char xh,yh,zh;				
	signed int 	x_k,y_k,z_k,x_value,y_value,z_value; //X��Y��Z�������ֵ
	signed int 	Acceleration;	//����ó��ļ��ٶ�ֵ

	//I2C_Read(KX022_IIC, KX022_ADDR,XOUT, 2, x, 2);	//���X��Y��Zֵ�ߵ�λ����
	//I2C_Read(KX022_IIC, KX022_ADDR,YOUT, 2, y, 2);						
	//I2C_Read(KX022_IIC, KX022_ADDR,ZOUT, 2, z, 2);

//	unsigned char step_data[6];

//	kx022_spi_master_read_sensor(step_data);
//	kx022_spi_master_read_buff(step_data);


	x[0] = step_data[1];//��8λ
	x[1] = step_data[0];//��8λ
	y[0] = step_data[3];//��8λ
	y[1] = step_data[2];//��8λ
	z[0] = step_data[5];//��8λ
	z[1] = step_data[4];//��8λ
	

	//����ĸ�λ�з���
	xh=x[0];	
	yh=y[0];
	zh=z[0];

	if((xh & 0x80) == 1)//X��Y��Z����ĸ�λ�����з���
	{
		xh = (~xh) + 1;	
		x_value = ((x[1] | (xh << 8))>>XXX);	//16λ�����ݣ���ЧλΪ12λ�������Ҫ��������4λ
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
	
	x_k = pow(x_value,2);	//����x^2
	y_k = pow(y_value,2);	//����y^2
	z_k = pow(z_value,2);	//����z^2

	Acceleration = sqrt(x_k + y_k + z_k)-512;	
//	Acceleration = sqrt(x_k + y_k + z_k)-1024;	//���ٶȵ���(x^2+y^2+z^2)��Ĥ���ټ�ȥ1024��1g��ֵ��

	
//	DEBUG("%d	%d	%d  ",x_value,y_value,z_value);
//	DEBUG("%d	%d	%d  ",x_k,y_k,z_k);
//	DEBUG("%d\r\n", Acceleration);
	return Acceleration;//���ؼ��ٶ�ֵ 
#endif
}

/*
 *	��������		 	 STEP_count
 *	����������������ա�
 *	�������أ�		 int	
 *	�漰������		 int 	filterlp(int sample)  
 *					 			 int 	xielv(int newval)
 *					 			 signed char area(signed int k)
 *					 			 void 	I2C_Ctrl(I2C_TypeDef *I2cPort, uint8_t start, uint8_t stop, uint8_t ack)
 *
 *	��Ҫ���ܣ�		 ��ʱ6msȡ���ݣ��������ݲ��ν���FIR�˲�����б�ʺͶ����ݲ��ν���������㣬�ó�����
 */	
int StepConter(unsigned char *p_src, unsigned char len)//�������ԭʼ����
{															  		
	signed int data_new;	//ˢ�µļ��ٶ�ֵ��ű���
	signed int l_data;		//�˲�֮��ļ��ٶ�ֵ
	signed int b_avreage;	//����б��֮��ļ��ٶ�ֵ
	
	data_new  = GetAcceleration(p_src, len);	//��������ۺϼ��ٶ�ֵ
	l_data 	  = filterlp(data_new);	//�˲�
	b_avreage = xielv(l_data);//��б�ʣ�΢�֣�

	if(b_avreage==1||b_avreage==-1)	//�˳�һЩС�Ķ���
	{
			b_avreage = 0;
	}

	if(area(b_avreage))	//ͨ������������ҷ����Ƿ����Ӳ���
	{
		return 1;
	}	
		
	return 0;	
}




