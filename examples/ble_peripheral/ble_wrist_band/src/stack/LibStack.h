#ifndef __LIB_STACK_H
#define __LIB_STACK_H

#if 1
  #ifdef MAX_FUNC_SIZE
    #undef MAX_FUNC_SIZE
  #endif
//���������ĳ���
  #define MAX_FUNC_SIZE 100
/**
 * ջ�����Դ 
 *  
 *  
 */
typedef struct
{
  uint32_t start; //SP��ʼ
  uint32_t maxSize; //����У����ʹ��ֵ
  uint32_t line; //����У����ʹ�õ�����
  uint8_t func[MAX_FUNC_SIZE]; //����У����ʹ�õĺ�������
}LibStack_t;
extern LibStack_t* LibStack_Get(void);
extern void __LibStack_Init(void);
extern  void __LibStack_Debug(const char *func, uint32_t line);

/**
 * ��ȡ��ʼ����SPֵ�� 
 * Ӧ������main�����ĵ�һ��ָ�� 
 * 
 * @author hzhmcu (2018/5/18)
 */
  #define LibStack_Init() __LibStack_Init()

/**
 * ջҪ��صĵط�������
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
