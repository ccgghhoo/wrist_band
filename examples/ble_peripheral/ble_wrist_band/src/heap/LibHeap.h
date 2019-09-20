

#ifndef LIB_HEAP_H__
#define LIB_HEAP_H__

#include "LibConfig.h"
#include "LibHeapLabel.h"


uint32_t LibHeap_Get(void); 
uint8_t* __LibHeap_MallocWithLable(uint32_t label,const char *func, uint32_t line, uint16_t size); 

void LibHeap_Free( void * targ );
#include "DateTime.h"

#define GetTimeTick() RunTime_GetValue()


#if 0
#define APP_MALLOC( label, size)		app_malloc( size ) 
#else
#define LIB_MALLOC( label, size)	 __LibHeap_MallocWithLable( label,__func__, __LINE__,  size)
//	LibHeap_MallocWithLabel( label, size )
#endif 


#define LIB_FREE(tag)				do{	LibHeap_Free(tag);tag= NULL ;}while(0)


//需要Appen 
#define MALLOC_EX_APPEN (50)

#define APP_MALLOC( label, size)	LIB_MALLOC(label,(size+MALLOC_EX_APPEN))//	app_malloc_with_label( label, size )
#define APP_FREE(tag)		LIB_FREE(tag)


#define LIB_HEAP_DEBUG  1



//HEAP 最大的函数名长度
#define MAX_HEAP_FUNC_SIZE 20
//记录 HEAP的最多项目
#define MAX_HEAP_ITEM 20



/**
 * 每一个项的信息。 
 *  
 */
typedef struct
{
  uint32_t addr; //分配到的内存地址信息。
  uint16_t size; //分配内存的大小
  uint32_t times; //分配内存的时间
  uint32_t interval; //现在到分配内存的时间间隔
  uint32_t line; //分配内存的函数在文件第几行，
  uint8_t func[MAX_HEAP_FUNC_SIZE]; //分配内存的函数名
}LibHeapItem_t;



int32_t LibHeap_GetItem(LibHeapItem_t *item,int index);
int32_t LibHeap_GetCount(void);

#endif 

