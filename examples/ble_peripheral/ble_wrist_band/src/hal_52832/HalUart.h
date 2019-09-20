

#ifndef __HAL_UART_H
#define __HAL_UART_H

#include <stdint.h>
#include <stdbool.h>


typedef enum {
	UART_PIORITY_LOWEST = 0,
	
	UART_PIORITY_GPS,
	UART_PIORITY_GSM, 
	UART_PIORITY_GPS_TLE, 
	
	UART_PIORITY_HIGHEST = 255, 	 
}UART_ARBITRATE_PIORITY;

typedef enum {
	UART_EVT_BASE		= 0,
	UART_EVT_END_RX		,		// receive completed 
	UART_EVT_RXTO		,		// receive timeout 
	UART_EVT_RXSTARTED	, 		// reception started 
	UART_EVT_RXRDY		, 		// data ready (1byte at least) 
	
	UART_EVT_END_TX		, 		// transmission completed 
	UART_EVT_TXSTOPPED	, 		// transmission completed 
	UART_EVT_LOSE_CTRL	,		// uart lost control ( high piority use it ) 
}UART_EVT_ID_ENUM; 

typedef struct{
	uint8_t*			p_buff;
	uint16_t 			len;
}uart_data_array_t; 

typedef struct{
	uint8_t 			evt_id;
	
	union{
		uart_data_array_t	end_rx;
	}params;
}uart_evt_t; 

typedef uint32_t 		UART_INSTANCE_SEQ_T; 
typedef void (*uart_evt_handle_t)(uart_evt_t * p_uart_evt); 


typedef struct{
	bool 							in_used; 		// uart instance is used 
	uint8_t 						piority; 		// current instance piority 
	uint32_t 						baudrate;		// baudrate 
	uint32_t 						txd_pin;		// TXD Pin 
	uint32_t 						rxd_pin; 		// RXD Pin 
	
	UART_INSTANCE_SEQ_T				seq_id;			// defined an instance seqid for function call compare 
	
	uart_evt_handle_t				evt_handle; 
}uart_instance_t; 


//void UART_enable(void);

//void UART_disable(void);

//void UART_receive_data_start( uint8_t* p_buff, uint16_t len);
void uart_select_baudrate(uint32_t bps);

void uart_release_control(UART_INSTANCE_SEQ_T instance_seq) ;

void uart_receive_data_abort(bool restart, UART_INSTANCE_SEQ_T instance_seq);

void uart_transmit_data_abort(UART_INSTANCE_SEQ_T instance_seq);
	
bool uart_data_packet_put(UART_INSTANCE_SEQ_T  instance_seq, const uint8_t* p_buff, uint16_t len);

void uart_required_init(void);

bool uart_arbitrate_apply(const uart_instance_t* p_config, UART_INSTANCE_SEQ_T *instance_seq);

typedef enum {
  UART_State_VALID,
  UART_State_GSM,
  UART_State_GPS,
  UART_State_CLOSE,
}UART_State_en;
//only for test
UART_State_en UartGet_Instance();
#endif 
