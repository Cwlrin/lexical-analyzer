#ifndef SCANNER_H
#define SCANNER_H

/**
 * @brief TokenType 枚举
 * @details 定义一个 TokenType 枚举，用于标记不同种类的 Token
 */
typedef enum {
	// SingleCharTokens 单字符 Token
	TOKEN_LEFT_PAREN,    ///< '(' 左小括号
	TOKEN_RIGHT_PAREN,   ///< ')' 右小括号
	TOKEN_LEFT_BRACKET,  ///< '[' 左中括号
	TOKEN_RIGHT_BRACKET, ///< ']' 右中括号
	TOKEN_LEFT_BRACE,    ///< '{' 左大括号
	TOKEN_RIGHT_BRACE,   ///< '}' 右大括号
	TOKEN_COMMA,         ///< ',' 逗号
	TOKEN_DOT,           ///< '.' 点
	TOKEN_SEMICOLON,     ///< ';' 分号
	TOKEN_TILDE,         ///< '~' 波浪号

	// SingleOrDoubleCharTokens 可能为单字符或双字符的 Token
	TOKEN_PLUS,            ///< '+' 加号
	TOKEN_PLUS_PLUS,       ///< '++' 自增运算符
	TOKEN_PLUS_EQUAL,      ///< '+=' 加赋运算符
	TOKEN_MINUS,           ///< '-' 减号或负号
	TOKEN_MINUS_MINUS,     ///< '--' 自减运算符
	TOKEN_MINUS_EQUAL,     ///< '-=' 减赋运算符
	TOKEN_MINUS_GREATER,   ///< '->' 结构体指针访问
	TOKEN_STAR,            ///< '*' 乘号
	TOKEN_STAR_EQUAL,      ///< '*=' 乘赋运算符
	TOKEN_SLASH,           ///< '/' 除号
	TOKEN_SLASH_EQUAL,     ///< '/=' 除赋运算符
	TOKEN_PERCENT,         ///< '%' 取模运算符
	TOKEN_PERCENT_EQUAL,   ///< '%=' 取模赋运算符
	TOKEN_AMPER,           ///< '&' 按位与运算符
	TOKEN_AMPER_EQUAL,     ///< '&=' 按位与赋运算符
	TOKEN_AMPER_AMPER,     ///< '&&' 逻辑与运算符
	TOKEN_PIPE,            ///< '|' 按位或运算符
	TOKEN_PIPE_EQUAL,      ///< '|=' 按位或赋运算符
	TOKEN_PIPE_PIPE,       ///< '||' 逻辑或运算符
	TOKEN_HAT,             ///< '^' 按位异或运算符
	TOKEN_HAT_EQUAL,       ///< '^=' 按位异或赋运算符
	TOKEN_EQUAL,           ///< '=' 赋值运算符
	TOKEN_EQUAL_EQUAL,     ///< '==' 等于比较运算符
	TOKEN_BANG,            ///< '!' 逻辑非运算符
	TOKEN_BANG_EQUAL,      ///< '!=' 不等于比较运算符
	TOKEN_LESS,            ///< '<' 小于比较运算符
	TOKEN_LESS_EQUAL,      ///< '<=' 小于等于比较运算符
	TOKEN_LESS_LESS,       ///< '<<' 左移运算符
	TOKEN_GREATER,         ///< '>' 大于比较运算符
	TOKEN_GREATER_EQUAL,   ///< '>=' 大于等于比较运算符
	TOKEN_GREATER_GREATER, ///< '>>' 右移运算符

	// MultiCharTokens 多字符 Token
	TOKEN_IDENTIFIER, ///< 标识符
	TOKEN_CHARACTER,  ///< 字符
	TOKEN_STRING,     ///< 字符串
	TOKEN_NUMBER,     ///< 数字，包含整数和浮点数

	// Keywords 关键字 Token
	TOKEN_SIGNED, TOKEN_UNSIGNED,
	TOKEN_CHAR, TOKEN_SHORT, TOKEN_INT, TOKEN_LONG,
	TOKEN_FLOAT, TOKEN_DOUBLE,
	TOKEN_STRUCT, TOKEN_UNION, TOKEN_ENUM, TOKEN_VOID,
	TOKEN_IF, TOKEN_ELSE, TOKEN_SWITCH, TOKEN_CASE, TOKEN_DEFAULT,
	TOKEN_WHILE, TOKEN_DO, TOKEN_FOR,
	TOKEN_BREAK, TOKEN_CONTINUE, TOKEN_RETURN, TOKEN_GOTO,
	TOKEN_CONST, TOKEN_SIZEOF, TOKEN_TYPEDEF,

	// AuxiliaryTokens 辅助 Token
	TOKEN_ERROR, ///< 错误
				 ///< Token 词法分析时遇到无法识别的文本
	TOKEN_EOF    ///< 文件结束
				 ///< Token 表示源代码已经分析完毕
} TokenType;

/**
 * @brief Token 结构体
 * @details 词法分析器的目的就是生产一个一个的 Token 对象\n
 * 这个 Token 只涉及一个字符指针指向源代码的字符信息, 没有在内部保存字符数据
 */
typedef struct {
	TokenType type; ///< Token 的类型, 取任一枚举值
	/*!< Token 的起始字符指针 */
	const char *start; ///< start 指向 source 中的字符，source 为读入的源代码。
	int length;        ///< length 表示这个 Token 的长度
	int line;          ///< line 表示这个 Token 在源代码的哪一行, 方便后面的报错和描述 Token
} Token;

/**
 * @brief 初始化词法分析器
 * @param source 源代码字符串
 * @details 将源码转换成字符串，供词法分析器使用。
 */
void initScanner(const char *source);
/**
 * @brief 词法分析器的核心 API
 * @details 调用此函数，生成源代码中下一段字符数据的 Token。
 * @return 返回一个 Token 对象\n
 * 当 Token 返回的是 TOKEN_EOF 时，表示源文件已完全分析。
 */
Token scanToken();

#endif  // !SCANNER_H
