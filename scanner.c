#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "scanner.h"

/**
 * @brief Scanner 结构体
 * @details 词法分析器的核心数据结构，包含当前正在扫描的 Token 的起始指针、当前处理指针、行号等。\n
 * 该结构体为内部实现细节，不对外暴露。
 */
typedef struct {
	const char *start;   ///< 指向当前正在扫描的 Token 的起始字符
	const char *current; ///< 当前处理的 Token 的字符，初始为 start，遍历完 Token 后指向下一个字符
	int line;            ///< 记录当前 Token 所处的行
} Scanner;

/**
 * @brief 全局 Scanner 实例
 * @details 静态全局变量，用于存储词法分析器的状态。
 */
static Scanner scanner;

/**
 * @brief 错误信息缓冲区
 * @details 用于存储词法分析器处理错误 Token 时的错误信息。
 */
static char message[50];

/**
 * @brief 初始化词法分析器
 * @details 初始化全局变量 Scanner 的状态，传入要分析的源码字符串，开始词法分析
 * @param source 源码字符串
 */
void initScanner(const char *source) {
	scanner.start = source;
	scanner.current = source;
	scanner.line = 1;
}

/**
 * @brief 判断字符是否为字母或下划线
 * @param c 待判断的字符
 * @return 如果 c 是字母或下划线，返回 true，否则返回 false
 */
static bool isAlpha(char c) {
	return (c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z') ||
			c == '_';
}

/**
 * @brief 判断字符是否为数字
 * @param c 待判断的字符
 * @return 如果 c 是数字，返回 true，否则返回 false
 */
static bool isDigit(char c) {
	return c >= '0' && c <= '9';
}

/**
 * @brief 判断是否到达字符串末尾
 * @return 如果当前字符是字符串末尾的空字符，返回 true，否则返回 false
 */
static bool isAtEnd() {
	return *scanner.current == '\0';
}

/**
 * @brief 词法分析器辅助函数，将当前指针向前移动一个字符
 * @return 之前当前指针所指向的字符
 */
static char advance() {
	return *scanner.current++;
}

/**
 * @brief 查看当前正在处理的字符，不移动当前指针
 * @return 当前指针所指向的字符
 */
static char peek() {
	return *scanner.current;
}

/**
 * @brief 查看下一个待处理的字符，不移动当前指针
 * @return 下一个待处理的字符，如果是字符串末尾则返回 '\0'
 */
static char peekNext() {
	if (isAtEnd()) {
		return '\0';
	}
	return *(scanner.current + 1);
}

/**
 * @brief 匹配预期字符
 * @details 如果当前字符符合预期，则将当前指针向前移动一个字符。
 * @param expected 预期的字符
 * @return 如果当前字符符合预期返回 true，否则返回 false。
 */
static bool match(char expected) {
	if (isAtEnd()) {
		return false; // 如果正在处理的是空字符, 那就返回 false
	}
	if (peek() != expected) {
		return false; // 如果不符合预期, 也返回 false
	}
	scanner.current++; // 符合预期那就 curr 向后移动一个字符
	return true;       // 只有符合预期才会返回 true 而且 curr 会前进一位
}

// 根据传入的 TokenType 类型来制造返回一个 Token
static Token makeToken(TokenType type) {
	Token token;
	token.type = type;
	token.start = scanner.start;
	token.length = (int)(scanner.current - scanner.start); // 计算 Token 字符串的长度
	token.line = scanner.line;
	return token;
}

/**
 * @brief 创建错误 Token
 * @details 遇到不能解析的情况时，我们创建一个 ERROR Token。\n
 * 比如：遇到 @，$ 等符号时，比如字符串，字符没有对应的右引号时。
 * @param message 错误信息
 * @return 错误 Token
 */
static Token errorToken(const char *message) {
	Token token;
	token.type = TOKEN_ERROR;
	token.start = message;
	token.length = (int)strlen(message);
	token.line = scanner.line;
	return token;
}

/**
 * @brief 跳过空白字符
 * 跳过包括空格、制表符、回车和换行在内的空白字符，以及单行注释。
 */
static void skipWhitespace() {
	// @TODO
	// 跳过空白字符: ' ', '\r', '\t', '\n'和单行注释
	// 注释以'//'开头, 一直到行尾
	// 注意更新 scanner.line！

	// 提示: 整个过程需要跳过中间的很多字符, 所以需要死循环
	// 最终目的是让 curr 指针指向下一个 Token 的首字符
}

/**
 * @brief 检查关键字
 * 检查从当前 Scanner 起始位置开始的特定长度的字符串是否为指定的关键字。
 * @param start 待检查序列的起始字符下标
 * @param length 待检查序列的长度
 * @param rest 待检查的剩余序列字符串
 * @param type 要检查的关键字 Token 类型
 * @return 如果匹配关键字，返回相应的 TokenType，否则返回 TOKEN_IDENTIFIER
 */
static TokenType checkKeyword(int start, int length, const char *rest, TokenType type) {
	/*
		start: 待检查序列的起始字符下标
			比如要检查关键字 break，那么在 case b 的前提下，需要传入 reak 来进行检查
			这里 start 就等于 1，scanner.start[1]
		length: 待检查序列的长度，如果检查的是 break，就是检查剩余的 reak
			需要传入 4
		rest 指针，待检查的剩余序列字符串，这里直接传入一个字面值字符串就行了
			比如检查 break，传入"reak"就好了
		type：你要检查的关键字 Token 的类型，比如检查 break，那就传入 Token_BREAK
	*/
	if (scanner.current - scanner.start == start + length &&
		/*
					int memcmp(const void *s1, const void *s2, size_t n);
					这里的参数分别是：

					s1：指向第一块内存区域的指针。
					s2：指向第二块内存区域的指针。
					n：要比较的字节数。
					memcmp 函数会逐字节比较 s1 和 s2 指向的内存区域，直到有不相等的字节或比较了 n 个字节为止。
					如果两个内存区域完全相同，
					则 memcmp 返回 0；如果第一个不同的字节在 s1 中的值小于 s2 中对应的值，返回负数；
					反之，返回正数。
		*/
		memcmp(scanner.start + start, rest, length) == 0) {
		return type;
	}
	return TOKEN_IDENTIFIER;
}

/**
 * @brief 确定 identifier 类型
 * 确定当前扫描的字符串是标识符还是关键字。
 * @return TokenType
 */
static TokenType identifierType() {
	// 确定 identifier 类型主要有两种方式：
	// 1. 将所有的关键字放入哈希表中，然后查表确认
	// Key-Value 就是"关键字-TokenType" 可以做 但存在额外内存占用且效率不如下一个方式好
	// 2. 将所有的关键字放入 Trie 树 (字典查找树) 中，然后查表确认
	// Trie 树的方式不管是空间上还是时间上都优于哈希表的方式
	// 用 switch...switch...if 组合构建逻辑上的 trie 树
	char first = scanner.start[0]; // 该 Token 的第一个字符
	switch (first) {
		case 'b': return checkKeyword(1, 4, "reak", TOKEN_BREAK);
		case 'c': ;
		// TODO
	}
	// 没有进 switch 一定是标识符
	return TOKEN_IDENTIFIER;
}

/**
 * @brief 处理标识符 Token
 * 如果当前字符是字母或下划线，处理标识符 Token。
 * @return Token
 */
static Token identifier() {
	// 判断 curr 指针当前正在处理的字符是不是 字母 下划线 数字
	while (isAlpha(peek()) || isDigit(peek())) {
		advance(); // 继续前进看下一个字符 直到碰到下一个字符不是字母 下划线 以及数字 结束 Token
	}
	// @TODO
	// 当 while 循环结束时，curr 指针指向的是该 Token 字符串的下一个字符
	// 这个函数的意思是: 只要读到字母或下划线开头的 Token 我们就进入标识符模式
	// 然后一直找到此 Token 的末尾
	// 但代码运行到这里还不确定 Token 是标识符还是关键字, 因为它可能是 break, var, goto, max_val...
	// 于是执行 identifierType() 函数，它是用来确定 Token 类型的
	return makeToken(identifierType());
}

/**
 * @brief 处理数字 Token
 * 处理数字 Token，包括整数和小数。
 * @return Token
 */
static Token number() {
	// @TODO
	// 简单起见，我们将 NUMBER 的规则定义如下:
	// 1. NUMBER 可以包含数字和最多一个'.'号
	// 2. '.'号前面要有数字
	// 3. '.'号后面也要有数字
	// 这些都是合法的 NUMBER: 123, 3.14
	// 这些都是不合法的 NUMBER: 123., .14(虽然在 C 语言中合法)
	// 提示: 这个过程需要不断的跳过中间的数字包括小数点, 所以也需要循环
}

/**
 * @brief 处理字符串 Token
 * 处理以双引号开头和结尾的字符串 Token。
 * @return Token
 */
static Token string() {
	// TODO
	// 字符串必须以"开头，以"结尾，而且不能跨行，不支持转义字符
	// 如果下一个字符不是末尾也不是双引号，全部跳过 (curr 可以记录长度，不用担心)
}
/*
	词法分析器处理字符的模式
	首先要分析此词法分析的状态
	此时 scanner.start 指向左单引号
	而 scanner.curr 指向左单引号的下一个字符, 也就是单引号中间的第一个字符
*/
/**
 * @brief 处理字符 Token
 * 处理以单引号开头和结尾的字符 Token。
 * @return Token
 */
static Token character() {
	// 字符'开头，以'结尾，而且不能跨行，不支持转义字符，''中间必须只有一个字符
	// 如果下一个字符不是末尾也不是单引号，全部跳过 (curr 可以记录长度，不用担心)

	// 1. 判断当前 curr 指向的是不是空字符, 如果是空字符, 那就返回一个 errToken
	//if (isAtEnd()) {
	//	return errorToken("此字符不完整,缺少右单引号!\n");
	//}
	// 2. 让 curr 指针继续走, 找到右单引号
	while (!isAtEnd() && peek() != '\'') {
		// 如果当前正在处理的字符既不是空字符, 也不是右单引号, 那就继续走
		if (peek() == '\n') {
			return errorToken("不支持多行字符!\n");
		}
		advance();
	} // while 循环结束时, 要么处理到了空字符, 要么处理到了右单引号
	if (isAtEnd()) {
		return errorToken("此字符不完整,缺少右单引号!\n");
	}
	// 找到了右单引号, 要判断一下此字符是不是单个长度, 单引号中出现多个字符是不允许的
	int charLen = scanner.current - scanner.start - 1;
	if (charLen == 1) {
		return makeToken(TOKEN_CHARACTER);
	}
	// 需求: 现在单引号中间的字符序列长度超长了, 于是要打印这个超长的序列
	// 输出效果是: 非单字符 Token: xxx(单引号中间的序列)
	char *charStart = scanner.start + 1;
	// 我们需要格式化的向目标字符数组中输出一个字符串
	sprintf(message, "非单字符Token: %.*s", charLen, charStart);
	return errorToken(message);
}

// 处理无法识别的字符
static Token errorTokenWithChar(char character) {
	// 将无法识别的字符是什么输出
	sprintf(message, "Unexpected character: %c", character);
	return errorToken(message);
}

/**
 * @brief 词法分析器核心逻辑
 * 返回一个制作好的 Token。
 * @return Token
 */
Token scanToken() {
	// 跳过前置空白字符和注释
	skipWhitespace();
	// 记录下一个 Token 的起始位置
	scanner.start = scanner.current;

	// 如果 curr 指向了空字符, 那么就已经处理源代码完毕了, 直接返回 TOKEN_EOF
	if (isAtEnd()) {
		return makeToken(TOKEN_EOF);
	}
	// 代码运行到这里, 说明 curr 指向的不是空字符, 那就继续处理
	/*
		(重点!!!!)
		调用 advance 函数,curr 指针指向了此 Token 的第二个字符
		但是 c 仍然是 Token 的第一个字符!!!!
	*/
	char c = advance();

	// 如果此 Token 的第一个字符是字母或下划线, 那么就进入标识符的处理模式
	// 同时关键字也合并在这里处理
	if (isAlpha(c)) {
		return identifier();
	}
	// 如果此 Token 的第一个字符是数字, 那就进入数字的处理模式
	if (isDigit(c)) {
		return number();
	}
	// 如果 Token 的第一个字符既不是数字也不是字母和下划线, 那么说明此 Token 既不是标识符, 也不是数字
	// 那就 switch 处理这个 Token
	switch (c) {
		// 第一部分: 处理单字符 Token
		case '(': return makeToken(TOKEN_LEFT_PAREN);
		case ')': return makeToken(TOKEN_RIGHT_PAREN);
		// ...TODO 还有比如 {};,.

		// 第二部分: 可单可双字符的 Token 处理会稍微复杂一点, 但不多
		// 如果 Token 的第一个字符是 + 号
		case '+':
			// curr 指针当前已经指向了 Token 的第二个字符, 也就是当前正在处理第二个字符
			// 如果 Token 的第二个字符也是 +, 那就生产 ++ 双字符 Token ++ 返回
			if (match('+')) {
				return makeToken(TOKEN_PLUS_PLUS);
			} else if (match('=')) {
				return makeToken(TOKEN_PLUS_EQUAL);
			} else {
				return makeToken(TOKEN_PLUS);
			}
		// ...TODO -,*,/
		case '"': return string();     // 如果 Token 的第一个字符是双引号, 那就进入字符串的处理模式
		case '\'': return character(); // 如果 Token 的第一个字符是单引号, 那就进入字符的处理模式
	}
	// 如果上述处理都没有处理成功, 都没有生成合适的 Token, 说明该字符无法识别
	return errorTokenWithChar(c);
}
