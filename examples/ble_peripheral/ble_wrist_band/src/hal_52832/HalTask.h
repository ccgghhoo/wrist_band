


#ifndef APP_TASK_DRV_H__
#define APP_TASK_DRV_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define TASK_INVALID_NUMBER		0xFFFF

#define TIMER_BASE		10 

#define TASK_TIMER_10MS		(10 / TIMER_BASE) 
#define TASK_TIMER_20MS		(20 / TIMER_BASE) 
#define TASK_TIMER_50MS		(50 / TIMER_BASE) 
#define TASK_TIMER_100MS	(100 / TIMER_BASE) 
#define TASK_TIMER_150MS	(150 / TIMER_BASE)
#define TASK_TIMER_200MS	(200 / TIMER_BASE) 
#define TASK_TIMER_500MS	(500 / TIMER_BASE) 
#define TASK_TIMER_600MS	(600 / TIMER_BASE)
#define TASK_TIMER_1S		(1000 / TIMER_BASE) 
#define TASK_TIMER_1_2S		(1200 / TIMER_BASE) 
#define TASK_TIMER_1_5S		(1500 / TIMER_BASE)
#define TASK_TIMER_2S		(2000 / TIMER_BASE) 
#define TASK_TIMER_3S		(3000 / TIMER_BASE) 
#define TASK_TIMER_4S		(4000 / TIMER_BASE) 
#define TASK_TIMER_5S		(5000 / TIMER_BASE) 
#define TASK_TIMER_10S		(10000 / TIMER_BASE) 

#define APP_TASK_DRV_DEF(task_id)		\
	static uint32_t task_id = TASK_INVALID_NUMBER
		
#define APP_TASK_DRV_CONFIG(timeout, handler) \
	{											\
	.time 		= 	timeout,					\
	.handle 	= 	handler,					\
	}		
	
typedef enum{
	APP_TASK_DRV_COMPLETED		= true,
	APP_TASK_DRV_CONTINUE		= false,
}APP_TASK_STATE_ENUM; 

typedef APP_TASK_STATE_ENUM (*app_task_handler_t)(void); 	

typedef struct{
	uint16_t 				time; 
	app_task_handler_t		handle; 
}app_task_drv_init_t; 



typedef struct{
	uint32_t 			task_no; 
	bool				enable; 	
	
	uint16_t 			ticks; 
	uint16_t 			timeout; 
	
	app_task_handler_t	handle; 
}app_task_drv_t; 

// 
bool app_task_drv_reg(app_task_drv_init_t *task, uint32_t *reg_task_id); 

void app_task_drv_completed( uint32_t task_no ); 

void app_task_drv_setup( uint32_t task_no, uint16_t resize_timeout);

void app_task_drv_init( void );


#endif 
