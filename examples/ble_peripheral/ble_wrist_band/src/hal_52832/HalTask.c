
#include "app_error.h"
#include "app_timer.h"

#include "HalTask.h" 


#define APP_TASK_SIZE			10 

#define APP_TASK_INTERVAL		APP_TIMER_TICKS(10) 

APP_TIMER_DEF( app_task_drv ); 
static bool					m_is_timer_running = false; 
static uint32_t 			m_task_count = 0 ; 
static app_task_drv_t		m_task[ APP_TASK_SIZE ]; 


static void app_task_drv_start(void)
{
	if( !m_is_timer_running ){
		m_is_timer_running = true; 
		
		uint32_t err_code = app_timer_start( app_task_drv, APP_TASK_INTERVAL, NULL); 
		if( err_code != NRF_SUCCESS) {
			APP_ERROR_CHECK( err_code ); 
		}
	}
}

static void app_timer_drv_stop(void)
{
	if( m_is_timer_running) {
		m_is_timer_running = false; 
		uint32_t err_code = app_timer_stop( app_task_drv ); 
		if( err_code != NRF_SUCCESS) {
			APP_ERROR_CHECK( err_code ); 
		}
	}
}

/**@brief register new task 
 * @input task -- timeout parameter & callback handle 
 * @output none 
 * @return task handle number 
*/

bool app_task_drv_reg(app_task_drv_init_t *task, uint32_t * reg_task_id)
{
	if( m_task_count + 1 >= APP_TASK_SIZE )
		return false; 
	
	m_task[m_task_count].timeout 	= task->time; 
	m_task[m_task_count].handle	= task->handle; 
	
	*reg_task_id = m_task_count; 
	m_task_count ++; 
	
	return true ; 
}

void app_task_drv_completed( uint32_t task_no )
{
	if( task_no > m_task_count )
		return; 
	
	m_task[ task_no].enable = false; 
}

void app_task_drv_setup( uint32_t task_no, uint16_t resize_timeout)
{
	if( task_no > m_task_count )
		return; 
	
	m_task[ task_no ].ticks = 0; 
	if( resize_timeout > 0){
		m_task[ task_no ].timeout = resize_timeout; 
	}
	
	m_task[ task_no ].enable = true; 	
	
	app_task_drv_start();
}

static void app_task_drv_timeout_handler(void* pcontext)
{
	bool m_is_task_over = true; 
	
	for( uint32_t i = 0 ; i < m_task_count; i++){
		if( !m_task[i].enable )
			continue; 
		
		m_is_task_over = false; 
		
		m_task[i].ticks ++; 
		if( m_task[i].ticks >= m_task[i].timeout ){
			if( m_task[i].handle() ){
				m_task[i].enable = false; 
			}
			m_task[i].ticks = 0; 
		}
	}
	
	if( m_is_task_over ){
		app_timer_drv_stop();
	}
}

void app_task_drv_init( void )
{
	uint32_t err_code = app_timer_create(& 	app_task_drv, 
											APP_TIMER_MODE_REPEATED, 
											app_task_drv_timeout_handler); 
	if( err_code != NRF_SUCCESS ){
		APP_ERROR_CHECK( err_code ); 
	}
	
	memset( m_task, 0 , sizeof( m_task ) ); 
}

 

