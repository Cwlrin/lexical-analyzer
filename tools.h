#pragma once
#include "scanner.h"
/**
 * @brief 将 Token 转换为其字符串表示形式。
 * @details 根据 Token 的类型，将 Token 转换成对应的易于阅读的字符串形式。
 * @param token 要转换的 Token。
 * @return 返回一个指向描述 Token 的字符串的指针。
 * @note 返回的字符串是静态分配的，因此调用者不需要释放内存。
 */
char *convert_to_str(Token token);