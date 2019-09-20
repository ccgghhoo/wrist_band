#ifndef __LIB_STACK_H
#define __LIB_STACK_H

#if 1
  #ifdef MAX_FUNC_SIZE
    #undef MAX_FUNC_SIZE
  #endif
//函数名最大的长度
  #define MAX_FUNC_SIZE 100
/**
 * 栈监控资源 
 *  
 *  
 */
typedef struct
{
  uint32_t start; //SP开始
  uint32_t maxSize; //监控中，最大使用值
  uint32_t line; //监控中，最大使用的行数
  uint8_t func[MAX_FUNC_SIZE]; //监控中，最大使用的函数名。
}LibStack_t;
extern LibStack_t* LibStack_Get(void);
extern void __LibStack_Init(void);
extern  void __LibStack_Debug(const char *func, uint32_t line);

/**
 * 获取初始化的SP值， 
 * 应该在在main函数的第一条指令 
 * 
 * @author hzhmcu (2018/5/18)
 */
  #define LibStack_Init() __LibStack_Init()

/**
 * 栈要监控的地方，定义
 * 
 * @author hzhmcu (2018/5/18)
 * 
 * @param func 
 * @param line 
 */
  #define LibStack_Debug()  __LibStack_Debug(__func__, __LINE__)
#else
#endif
#endif
