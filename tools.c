#include "tools.h"

char *convert_to_str(Token token) {
	switch (token.type) {
		case TOKEN_LEFT_PAREN: return "左小括号";
		case TOKEN_RIGHT_PAREN: return "右小括号";
		case TOKEN_LEFT_BRACKET: return "左中括号";
		case TOKEN_RIGHT_BRACKET: return "右中括号";
		case TOKEN_LEFT_BRACE: return "左大括号";
		case TOKEN_RIGHT_BRACE: return "右大括号";
		case TOKEN_COMMA: return "逗号";
		case TOKEN_DOT: return "点";
		case TOKEN_SEMICOLON: return "分号";
		case TOKEN_TILDE: return "波浪号";
		case TOKEN_PLUS: return "加号";
		case TOKEN_PLUS_PLUS: return "自增运算符";
		case TOKEN_PLUS_EQUAL: return "加赋运算符";
		case TOKEN_MINUS: return "减号";
		case TOKEN_MINUS_MINUS: return "自减运算符";
		case TOKEN_MINUS_EQUAL: return "减赋运算符";
		case TOKEN_MINUS_GREATER: return "结构体指针访问运算符";
		case TOKEN_STAR: return "乘号";
		case TOKEN_STAR_EQUAL: return "乘赋运算符";
		case TOKEN_SLASH: return "除号";
		case TOKEN_SLASH_EQUAL: return "除赋运算符";
		case TOKEN_PERCENT: return "取模运算符";
		case TOKEN_PERCENT_EQUAL: return "取模赋运算符";
		case TOKEN_AMPER: return "按位与运算符";
		case TOKEN_AMPER_EQUAL: return "按位与赋运算符";
		case TOKEN_AMPER_AMPER: return "逻辑与运算符";
		case TOKEN_PIPE: return "按位或运算符";
		case TOKEN_PIPE_EQUAL: return "按位或赋运算符";
		case TOKEN_PIPE_PIPE: return "逻辑或运算符";
		case TOKEN_HAT: return "按位异或运算符";
		case TOKEN_HAT_EQUAL: return "按位异或赋运算符";
		case TOKEN_EQUAL: return "赋值运算符";
		case TOKEN_EQUAL_EQUAL: return "等于比较运算符";
		case TOKEN_BANG: return "逻辑非运算符";
		case TOKEN_BANG_EQUAL: return "不等于比较运算符";
		case TOKEN_LESS: return "小于比较运算符";
		case TOKEN_LESS_EQUAL: return "小于等于比较运算符";
		case TOKEN_LESS_LESS: return "左移运算符";
		case TOKEN_GREATER: return "大于比较运算符";
		case TOKEN_GREATER_EQUAL: return "大于等于比较运算符";
		case TOKEN_GREATER_GREATER: return "右移运算符";
		case TOKEN_IDENTIFIER: return "标识符";
		case TOKEN_CHARACTER: return "字符";
		case TOKEN_STRING: return "字符串";
		case TOKEN_NUMBER: return "数字";
		case TOKEN_SIGNED: return "SIGNED";
		case TOKEN_UNSIGNED: return "UNSIGNED";
		case TOKEN_CHAR: return "CHAR";
		case TOKEN_SHORT: return "SHORT";
		case TOKEN_INT: return "INT";
		case TOKEN_LONG: return "LONG";
		case TOKEN_FLOAT: return "FLOAT";
		case TOKEN_DOUBLE: return "DOUBLE";
		case TOKEN_STRUCT: return "STRUCT";
		case TOKEN_UNION: return "UNION";
		case TOKEN_ENUM: return "ENUM";
		case TOKEN_VOID: return "VOID";
		case TOKEN_IF: return "IF";
		case TOKEN_ELSE: return "ELSE";
		case TOKEN_SWITCH: return "SWITCH";
		case TOKEN_CASE: return "CASE";
		case TOKEN_DEFAULT: return "DEFAULT";
		case TOKEN_WHILE: return "WHILE";
		case TOKEN_DO: return "DO";
		case TOKEN_FOR: return "FOR";
		case TOKEN_BREAK: return "BREAK";
		case TOKEN_CONTINUE: return "CONTINUE";
		case TOKEN_RETURN: return "RETURN";
		case TOKEN_GOTO: return "GOTO";
		case TOKEN_CONST: return "CONST";
		case TOKEN_SIZEOF: return "SIZEOF";
		case TOKEN_TYPEDEF: return "TYPEDEF";
		case TOKEN_ERROR: return "ERROR";
		case TOKEN_EOF: return "EOF";
		default: return "未知";
	}
}
