#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "scanner.h"

/**
 * @brief Scanner 结构体
 * @details 词法分析器的核心数据结构，包含当前正在扫描的 Token 的起始指针、当前处理指针、行号等 \n
 * 该结构体为内部实现细节，不对外暴露
 */
typedef struct {
	const char *start;   ///< 指向当前正在扫描的 Token 的起始字符
	const char *current; ///< 当前处理的 Token 的字符，初始为 start，遍历完 Token 后指向下一个字符
	int line;            ///< 记录当前 Token 所处的行
} Scanner;

/**
 * @brief 全局 Scanner 实例
 * @details 静态全局变量，用于存储词法分析器的状态
 */
static Scanner scanner;

/**
 * @brief 错误信息缓冲区
 * @details 用于存储词法分析器处理错误 Token 时的错误信息
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
 * @return 如果当前字符符合预期返回 true，否则返回 false
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

/**
 * @brief 根据传入的 TokenType 类型来制造返回一个 Token
 * @param type 要创建的 Token 的类型
 * @return Token 结构体
 */
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
 * @details 遇到不能解析的情况时，创建一个 ERROR Token。\n
 * 比如：遇到 @，$ 等符号时，比如字符串，字符没有对应的右引号时
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
 * @details 此函数将 scanner 的当前位置向前移动，跳过所有空白字符和单行注释，
 * 直到遇到非空白字符或源代码结束
 */
static void skipWhitespace() {
	for (;;) {
		char c = peek(); // 查看当前字符
		switch (c) {
			case ' ':  // 空格
			case '\r': // 回车
			case '\t': // 制表符
			case '\n': // 换行符
				// 如果当前字符是空白字符，移动到下一个字符
				advance();
				break;
			case '/': // 正斜杠，可能是注释或除号
				// 如果当前字符是正斜杠，检查下一个字符以确定是否为注释
				if (peekNext() == '/') {
					// 单行注释，跳过直到行尾或源代码结束
					while (peek() != '\n' && !isAtEnd()) {
						advance();
					}
				} else {
					// 如果不是注释，说明已经到达有效的 Token，退出循环
					return;
				}
				break;
			default:
				// 对于任何其他字符，说明已经到达有效的 Token，退出循环
				return;
		}
	}
}

/**
 * @brief 检查关键字
 * 检查从当前 Scanner 起始位置开始的特定长度的字符串是否为指定的关键字
 * @param start 待检查序列的起始字符下标
 * @param length 待检查序列的长度
 * @param rest 待检查的剩余序列字符串
 * @param type 要检查的关键字 Token 类型
 * @return 如果匹配关键字，返回相应的 TokenType，否则返回 TOKEN_IDENTIFIER
 */
static TokenType checkKeyword(int start, int length, const char *rest, TokenType type) {
	// 检查当前扫描的位置与预期的关键字起始位置加上长度是否匹配
	// 即确认扫描器的当前位置是否在预期的关键字之后
	if (scanner.current - scanner.start == start + length && memcmp(scanner.start + start, rest, length) == 0) {
		return type;
	}
	return TOKEN_IDENTIFIER;
}

/**
 * @brief 确定 identifier 类型
 * 确定当前扫描的字符串是标识符还是关键字
 * @return 返回识别出的 TokenType，如果字符串是关键字，则返回对应的类型 \n
 * 如果字符串是标识符，则返回 TOKEN_IDENTIFIER。
 */
static TokenType identifierType() {
	// 使用 switch...switch...if 结构模拟 Trie 树的查找过程
	char first = scanner.start[0]; // 取得 Token 的第一个字符作为分类依据
	switch (first) {
		// keywords
		case 'b': return checkKeyword(1, 4, "reak", TOKEN_BREAK);
		case 'c': {
			int len = scanner.current - scanner.start;
			if (len > 1) {
				switch (scanner.start[1]) {
					case 'a': return checkKeyword(2, 2, "se", TOKEN_CASE);
					case 'h': return checkKeyword(2, 2, "ar", TOKEN_CHAR);
					case 'o':
						if (len > 3 && scanner.start[2] == 'n') {
							switch (scanner.start[3]) {
								case 's': return checkKeyword(4, 1, "t", TOKEN_CONST);
								case 't': return checkKeyword(4, 4, "inue", TOKEN_CONTINUE);
							}
						}
				}
			}
			break;
		}
		case 'd': {
			int len = scanner.current - scanner.start;
			if (len > 1) {
				switch (scanner.start[1]) {
					case 'e': return checkKeyword(2, 5, "fault", TOKEN_DEFAULT);
					case 'o':
						if (len == 2) return TOKEN_DO;
						else return checkKeyword(2, 4, "uble", TOKEN_DOUBLE);
				}
			}
			break;
		}
		case 'e': {
			int len = scanner.current - scanner.start;
			if (len > 1) {
				switch (scanner.start[1]) {
					case 'l': return checkKeyword(2, 2, "se", TOKEN_ELSE);
					case 'n': return checkKeyword(2, 2, "um", TOKEN_ENUM);
				}
			}
			break;
		}
		case 'f': {
			int len = scanner.current - scanner.start;
			if (len > 1) {
				switch (scanner.start[1]) {
					case 'l': return checkKeyword(2, 3, "oat", TOKEN_FLOAT);
					case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR);
				}
			}
			break;
		}
		case 'g': return checkKeyword(1, 3, "oto", TOKEN_GOTO);
		case 'i': {
			int len = scanner.current - scanner.start;
			if (len > 1) {
				switch (scanner.start[1]) {
					case 'f': return checkKeyword(2, 0, "", TOKEN_IF);
					case 'n': return checkKeyword(2, 1, "t", TOKEN_INT);
				}
			}
			break;
		}
		case 'l': return checkKeyword(1, 3, "ong", TOKEN_LONG);
		case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
		case 's': {
			int len = scanner.current - scanner.start;
			if (len > 1) {
				switch (scanner.start[1]) {
					case 'h': return checkKeyword(2, 3, "ort", TOKEN_SHORT);
					case 'i':
						if (len > 2) {
							switch (scanner.start[2]) {
								case 'g': return checkKeyword(3, 3, "ned", TOKEN_SIGNED);
								case 'z': return checkKeyword(3, 3, "eof", TOKEN_SIZEOF);
							}
						}
						break;
					case 't': return checkKeyword(2, 4, "ruct", TOKEN_STRUCT);
					case 'w': return checkKeyword(2, 4, "itch", TOKEN_SWITCH);
				}
			}
			break;
		}
		case 't': return checkKeyword(1, 6, "ypedef", TOKEN_TYPEDEF);
		case 'u': {
			int len = scanner.current - scanner.start;
			if (len > 2 && scanner.start[1] == 'n') {
				switch (scanner.start[2]) {
					case 'i': return checkKeyword(3, 2, "on", TOKEN_UNION);
					case 's': return checkKeyword(3, 5, "igned", TOKEN_UNSIGNED);
				}
			}
			break;
		}
		case 'v': return checkKeyword(1, 3, "oid", TOKEN_VOID);
		case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE);
	}
	// 默认情况，如果以上关键字都不匹配，则返回标识符类型
	return TOKEN_IDENTIFIER;
}

/**
 * @brief 处理标识符 Token
 * @details 识别并构造以字母或下划线开头，后跟任意数量的字母、下划线或数字的标识符 Token
 * @return 返回类型为 TOKEN_IDENTIFIER 或特定关键字类型的
 */
static Token identifier() {
	// 循环以识别标识符的字符序列，只要当前字符是字母、下划线或数字，就继续读取下一个字符
	// 循环将自动跳过标识符中的所有字符，直到遇到非标识符字符
	while (isAlpha(peek()) || isDigit(peek())) {
		advance(); // 移动到下一个字符，继续构建标识符 Token
	}
	// 循环结束后，当前字符不是字母、下划线或数字，表示已经到达标识符的末尾
	// 此时，scanner.start 指向标识符的开始，而 scanner.current 指向标识符的下一个字符
	// 使用 identifierType() 函数判断当前扫描的标识符是普通标识符还是关键字
	// 这个函数会根据标识符的字符串内容返回相应的 TokenType
	TokenType type = identifierType();
	// 构造并返回 Token
	return makeToken(type);
}

/**
 * @brief 处理数字 Token
 * @details 根据预定义的规则，识别数字（包括整数和小数），并构造相应的 Token
 * @return 返回类型为 TOKEN_NUMBER 的 Token
 */
static Token number() {
	// 识别数字的整数部分，包括数字零和正整数
	while (isDigit(peek())) {
		advance();
	}
	// 检查是否遇到小数点，并验证小数点后是否有数字
	if (peek() == '.' && isDigit(peekNext())) {
		// 确认小数点后跟有数字，构造小数 Token
		advance();
		// 继续循环以识别小数点后的数字部分，根据规则，小数点后必须有至少一位数字
		while (isDigit(peek())) {
			advance();
		}
	}
	// 无论数字是整数还是小数，都使用相同的 Token 类型 TOKEN_NUMBER
	return makeToken(TOKEN_NUMBER);
}

/**
 * @brief 处理字符串 Token
 * @details 处理以双引号开头和结尾的字符串 Token
 * @return Token
 */
static Token string() {
	// 字符串必须以双引号开头和结尾，不能跨越多行，不支持转义字符
	// 循环直到遇到双引号或文件结束符
	while (!isAtEnd() && peek() != '"') {
		// 如果当前字符是换行符，表示字符串跨越了多行，这是不允许的
		if (peek() == '\n') {
			return errorToken("不支持多行字符串!");
		}
		advance(); // 继续检查下一个字符，直到字符串结束或文件结束。
	}
	// 检查是否到达文件结束符，如果没有找到结束的双引号，表示字符串未终止
	if (isAtEnd()) {
		return errorToken("未终止的字符串字面量！");
	}
	// 找到字符串结束的双引号，构造字符串 Token 并返回
	advance();
	return makeToken(TOKEN_STRING);
}

/**
 * @brief 处理字符 Token
 * @details 处理以单引号开头和结尾的字符 Token
 * @return Token
 */
static Token character() {
	// 如果到达文件结束符，表示字符 Token 不完整，缺少右单引号
	if (isAtEnd()) {
		return errorToken("此字符不完整,缺少右单引号!");
	}
	// 确保当前字符不是空字符，且不是单引号
	while (!isAtEnd() && peek() != '\'') {
		// 循环直到遇到单引号或文件结束符
		if (peek() == '\n') {
			// 如果遇到换行符，表示字符 Token 跨越了多行，这是不允许的
			return errorToken("不支持多行字符!");
		}
		advance(); // 继续检查下一个字符
	}
	if (isAtEnd()) {
		return errorToken("此字符不完整,缺少右单引号!");
	}
	// 确认找到了右单引号，此时已经完成了字符 Token 的识别，需要检查单引号内是否正好有一个字符
	// 结束一个 Token 处理时，要保证 curr 指针移动向此 Token 的下一个位置
	advance();
	int charLen = scanner.current - scanner.start - 2;
	// 如果单引号内只有一个字符，则构造并返回正常的字符 Token
	if (charLen == 1 || charLen == 0) {
		return makeToken(TOKEN_CHARACTER);
	}
	// 如果单引号内字符数量不为一，这是不合法的字符 Token
	// 构造并返回一个错误 Token，描述非法字符 Token 的内容
	char *charStart = (char *)(scanner.start + 1); // 指向字符 Token 的起始位置
	sprintf(message, "非单字符Token: %.*s", charLen, charStart);
	return errorToken(message);
}

/**
 * @brief 处理无法识别的字符
 * @param character 字符
 * @return Token 错误信息
 */
static Token errorTokenWithChar(char character) {
	// 将无法识别的字符输出
	sprintf(message, "意外字符：%c", character);
	return errorToken(message);
}

/**
 * @brief 词法分析器核心逻辑
 * @details 返回一个制作好的 Token
 * @return Token
 */
Token scanToken() {
	// 跳过所有前置的空白字符和注释，将 scanner.current 指向下一个有效的 Token 起始位置
	skipWhitespace();
	// 记录下一个 Token 的起始位置
	scanner.start = scanner.current;
	// 如果 curr 指向了空字符, 那么就已经处理源代码完毕了, 直接返回 TOKEN_EOF
	if (isAtEnd()) {
		return makeToken(TOKEN_EOF);
	}
	char c = advance();
	// 如果当前字符是字母或下划线，进入标识符或关键字的识别流程
	if (isAlpha(c)) {
		return identifier();
	}
	// 如果当前字符是数字，进入数字的识别流程
	if (isDigit(c)) {
		return number();
	}
	// 根据当前字符，通过 switch 语句识别和处理各种单字符和多字符的 Token
	switch (c) {
		// 处理单字符 Token
		case '(': return makeToken(TOKEN_LEFT_PAREN);
		case ')': return makeToken(TOKEN_RIGHT_PAREN);
		case '{': return makeToken(TOKEN_LEFT_BRACE);
		case '}': return makeToken(TOKEN_RIGHT_BRACE);
		case ',': return makeToken(TOKEN_COMMA);
		case '.': return makeToken(TOKEN_DOT);
		case ';': return makeToken(TOKEN_SEMICOLON);
		case '~': return makeToken(TOKEN_TILDE);
		// 处理可能的双字符 Token
		case '+':
			if (match('+')) {
				return makeToken(TOKEN_PLUS_PLUS);
			} else if (match('=')) {
				return makeToken(TOKEN_PLUS_EQUAL);
			} else {
				return makeToken(TOKEN_PLUS);
			}
		case '-':
			if (match('-')) {
				return makeToken(TOKEN_MINUS_MINUS);
			} else if (match('=')) {
				return makeToken(TOKEN_MINUS_EQUAL);
			} else if (match('>')) {
				return makeToken(TOKEN_MINUS_GREATER);
			} else {
				return makeToken(TOKEN_MINUS);
			}
		case '*':
			return makeToken(match('=') ? TOKEN_STAR_EQUAL : TOKEN_STAR);
		case '/':
			return makeToken(match('=') ? TOKEN_SLASH_EQUAL : TOKEN_SLASH);
		case '%':
			return makeToken(match('=') ? TOKEN_PERCENT_EQUAL : TOKEN_PERCENT);
		case '&':
			if (match('=')) {
				return makeToken(TOKEN_AMPER_EQUAL);
			} else if (match('&')) {
				return makeToken(TOKEN_AMPER_AMPER);
			} else {
				return makeToken(TOKEN_AMPER);
			}
		case '|':
			if (match('=')) {
				return makeToken(TOKEN_PIPE_EQUAL);
			} else if (match('|')) {
				return makeToken(TOKEN_PIPE_PIPE);
			} else {
				return makeToken(TOKEN_PIPE);
			}
		case '^':
			return makeToken(match('=') ? TOKEN_HAT_EQUAL : TOKEN_HAT);
		case '=':
			return makeToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
		case '!':
			return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
		case '<':
			if (match('=')) {
				return makeToken(TOKEN_LESS_EQUAL);
			} else if (match('<')) {
				return makeToken(TOKEN_LESS_LESS);
			} else {
				return makeToken(TOKEN_LESS);
			}
		case '>':
			if (match('=')) {
				return makeToken(TOKEN_GREATER_EQUAL);
			} else if (match('>')) {
				return makeToken(TOKEN_GREATER_GREATER);
			} else {
				return makeToken(TOKEN_GREATER);
			}
		// 处理字符串和字符字面量 Token
		case '"': return string();     // 字符串处理模式
		case '\'': return character(); // 字符处理模式
		// 如果当前字符不匹配任何已知 Token 类型，则生成一个错误 Token
		default:
			return errorTokenWithChar(c);
	}
}
