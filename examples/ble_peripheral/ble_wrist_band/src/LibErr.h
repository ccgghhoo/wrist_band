#ifndef __LIB_ERR_H
#define __LIB_ERR_H

/**
 * 大于等于0的才是正确值， 
 * 小于0的都是错误值
 */
typedef enum {
  SUCCESS = 0,
  ERR_PARAM = -1, //参数错误
  ERR_OBJ_NULL=-2, //对象为空
  ERR_LLI_NULL=-3, //没有底层接口。

}ERR_en;


#endif
