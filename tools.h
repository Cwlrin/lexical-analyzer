#pragma once
#include "scanner.h"
// 该函数根据 Token 的类型，转换成对应的 Token 字符串，比如标识符 Token，会输出一个"字符串"的 Token 名
char *convert_to_str(Token token);