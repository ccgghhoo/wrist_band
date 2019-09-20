#include "LibConfig.h"
#include "LibHeap.h"
#include "stdlib.h"
#if 0
  #define HM_LOG			HZH_LOG
  #define HM_ERROR			HZH_LOG

#else
  #define HM_LOG(...)
  #define HM_ERROR(...)

#endif

/**
 * Heap信息
 */
typedef struct {
  int32_t cnt; //分配的多少项

#if  LIB_HEAP_DEBUG
  bool isFirst; //是否第一次分配
  uint32_t startAddress; //每一次分配的地址。
  LibHeapItem_t item[MAX_HEAP_ITEM]; //项目的具体信息
#endif

}LibHeap_t;


//初始化信息
static volatile LibHeap_t svg_sLibHeap = {
  .cnt = 0,
#if  LIB_HEAP_DEBUG
  .isFirst = true,
#endif
};


/**
 * 获取当前分配项
 * 
 * @author hzhmc (2018/5/19)
 * 
 * @param void 
 * 
 * @return uint32_t 
 */
uint32_t LibHeap_Get(void)
{
  return  svg_sLibHeap.cnt;
}


/**
 * 把分配到的信息，加载到信息内。
 * 
 * @author hzhmc (2018/5/19)
 * 
 * @param func 
 * @param line 
 * @param address 
 * @param size 
 * 
 * @return __INLINE int32_t 
 */
__INLINE static  int32_t Malloc_Item(const char *func, uint32_t line,
                                     uint8_t *address, uint16_t size)
{
  svg_sLibHeap.cnt++;
#if  LIB_HEAP_DEBUG
  uint32_t index = 0;

  if (MAX_HEAP_ITEM < svg_sLibHeap.cnt)
  {}
  if (svg_sLibHeap.isFirst == true)
  {
    svg_sLibHeap.isFirst = false;

    for (int i = 0; i < MAX_HEAP_ITEM; i++)
    {
      svg_sLibHeap.item[i].addr = 0;
    }
    index = 0;
    goto SET_ITEM;
  } else
  {
    for (int i = 0; i < MAX_HEAP_ITEM; i++)
    {
      if (svg_sLibHeap.item[i].addr == 0)
      {
        index = i;
        goto SET_ITEM;
      }
    }
    return -1; //没有是空的
  }
SET_ITEM:
  svg_sLibHeap.item[index].addr = (uint32_t)address;
  svg_sLibHeap.item[index].size = size;
  svg_sLibHeap.item[index].line = line;
  memset((void *)svg_sLibHeap.item[index].func, '\0', MAX_HEAP_FUNC_SIZE);
  int32_t len = strlen(func);
  if (len > MAX_HEAP_FUNC_SIZE)
  {
    len = MAX_HEAP_FUNC_SIZE;
  }
  memcpy((void *)svg_sLibHeap.item[index].func, func, len);
  svg_sLibHeap.item[index].times = GetTimeTick();
  svg_sLibHeap.item[index].interval = 0;
#endif
  return 0;

}

/**
 * 释放Heap，把信息内的信息删除。
 * 
 * @author hzhmc (2018/5/19)
 * 
 * @param address 
 * 
 * @return __INLINE int32_t 
 */
__INLINE static  int32_t Free_Item(uint8_t *address)
{

  svg_sLibHeap.cnt--;
#if  LIB_HEAP_DEBUG
  if (svg_sLibHeap.cnt < 0)
  {}
  for (int i = 0; i < MAX_HEAP_ITEM; i++)
  {
    if (svg_sLibHeap.item[i].addr == (uint32_t)address)
    {
      svg_sLibHeap.item[i].addr = 0;
      return 0;
    }
  }
  return -1;
#else
  return 0;
#endif
}



uint8_t* __LibHeap_MallocWithLable(uint32_t label, const char *func, uint32_t line, uint16_t size)
{
  uint8_t *tmp;

  CRITICAL_Enter();
  tmp = (uint8_t *)malloc(size);
  if (tmp != NULL)
  {
    Malloc_Item(func, line, tmp, size);
  }
  CRITICAL_Exit();
  if (tmp != NULL)
  {
    HM_LOG("[HEAP]: label = %d, start = 0x%X, count = %d\r\n", label, (uint32_t)tmp, LibHeap_Get());
  } else
  {
    HM_LOG("[HEAP]: label = %d, fail !!!!\r\n", label);
  }

  return tmp;
}



void LibHeap_Free(void *targ) // safe free
{
  if (targ != NULL)
  {
    HM_LOG("[HEAP]: free at 0x%X, remain = %d \r\n", (uint32_t)targ, LibHeap_Get());
    CRITICAL_Enter();
    free(targ);
    Free_Item(targ);
    targ = NULL;
    CRITICAL_Exit();
  } else
  {
    HM_LOG("[HEAP]: targ is NULL!!!! 0x%x\r\n",(uint32_t)targ);
  }
}

int32_t LibHeap_GetItem(LibHeapItem_t *item, int index)
{
  if (index > MAX_HEAP_ITEM || item == NULL)
  {
    return -1;
  }
  if (svg_sLibHeap.item[index].addr == 0)
  {
    return 0;
  }

  uint32_t tick = GetTimeTick();
  if (tick >= svg_sLibHeap.item[index].times)
  {
    tick -= svg_sLibHeap.item[index].times;
  } else
  {
    tick = 0xFFFFFFFF;
  }
  svg_sLibHeap.item[index].interval = tick;

  memcpy((uint8_t *)item, (uint8_t *)&svg_sLibHeap.item[index], sizeof(LibHeapItem_t));

  return 1;
}

int32_t LibHeap_GetCount(void)
{
  return svg_sLibHeap.cnt;
}











