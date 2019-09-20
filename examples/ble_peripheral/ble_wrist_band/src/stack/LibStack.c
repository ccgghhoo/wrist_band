#include "LibConfig.h"

//#define MAX_FUNC_SIZE 100


//�������
volatile LibStack_t svg_sLibStack;

LibStack_t* LibStack_Get()
{
  return (LibStack_t* )&svg_sLibStack;
}

/**
 * ��ȡ��ʼ����SPֵ�� 
 * Ӧ������main�����ĵ�һ��ָ�� 
 * 
 * @author hzhmcu (2018/5/18)
 */
void __LibStack_Init()
{
  svg_sLibStack.start =  GET_SP();
}


/**
 * ջҪ��صĵط�������
 * 
 * @author hzhmcu (2018/5/18)
 * 
 * @param func 
 * @param line 
 */
void __LibStack_Debug(const char *func, uint32_t line)
{
  uint32_t msp  = GET_SP();
  uint32_t len = 0;
  int64_t value = svg_sLibStack.start - msp;
  if (value > svg_sLibStack.maxSize)
  {
    memset((void*)svg_sLibStack.func, 0, MAX_FUNC_SIZE); 
    svg_sLibStack.line = line;
    len = strlen(func);
    if (len> MAX_FUNC_SIZE)
    {
      len = MAX_FUNC_SIZE;
    }
    memcpy((void*)svg_sLibStack.func, func, len);
    svg_sLibStack.maxSize = value;
  }
}


