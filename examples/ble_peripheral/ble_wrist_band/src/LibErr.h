#ifndef __LIB_ERR_H
#define __LIB_ERR_H

/**
 * ���ڵ���0�Ĳ�����ȷֵ�� 
 * С��0�Ķ��Ǵ���ֵ
 */
typedef enum {
  SUCCESS = 0,
  ERR_PARAM = -1, //��������
  ERR_OBJ_NULL=-2, //����Ϊ��
  ERR_LLI_NULL=-3, //û�еײ�ӿڡ�

}ERR_en;


#endif
