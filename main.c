#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"
#include "tools.h"

/**
 * @brief 运行词法分析器并打印 Token 分析结果。
 * @param source 源代码字符串，将被词法分析器处理。
 */
static void run(const char *source) {
	initScanner(source); // 初始化词法分析器
	int line = -1;       // 用于记录当前处理的行号,-1 表示还未开始解析
	for (;;) {
		Token token = scanToken(); // 获取下一个 TOKEN
		if (token.line != line) {
			// 如果 Token 中记录行和现在的 lin 不同就执行换行打印的效果
			printf("%4d ", token.line);
			line = token.line;
		} else {
			// 没有换行的打印效果，使用竖杠是为了美观
			printf("   | ");
		}
		char *str = convert_to_str(token);
		// 打印 Token 的字符序列，使用 %.*s 避免打印到字符串末尾的空字符
		printf("%s '%.*s'\n", str, token.length, token.start);

		if (token.type == TOKEN_EOF) {
			break; // 读到 TOKEN_EOF 结束循环
		}
	}
}

/**
 * @brief 定义一个交互式的读取 - 求值 - 打印循环（REPL）。
 * @details 用户可以输入源代码行，逐行进行词法分析，并打印分析结果。
 */
static void repl() {
	char line[1024]; // 1024 字符的缓冲区
	for (;;) {
		printf("> "); // 打印提示符
		if (fgets(line, sizeof(line), stdin) == NULL) {
			// 如果用户输入了 Ctrl+D, 则 fgets 返回 0, 此时退出循环
			printf("\n");
			break;
		}
		run(line); // 调用 run 函数处理用户输入的一行字符串
	}
}

/**
 * @brief 从文件读取内容到内存。
 * @param path 文件路径。
 * @return 动态分配的字符串，包含文件的全部字符信息。
 * @note 使用者负责释放返回的内存。
 */
static char *readFile(const char *path) {
	FILE *file = fopen(path, "rb");
	if (file == NULL) {
		fprintf(stderr, "无法打开文件 \"%s\".\n", path);
		exit(1);
	}

	fseek(file, 0, SEEK_END); // 获取文件大小
	size_t size = ftell(file);
	rewind(file); // 重置文件指针
	char *buffer = malloc(size + 1);
	if (buffer == NULL) {
		fprintf(stderr, "内存不足，无法读取文件 \"%s\".\n", path);
		exit(1);
	}

	size_t bytesRead = fread(buffer, sizeof(char), size, file); // 读取文件内容
	if (bytesRead < size) {
		fprintf(stderr, "无法读取文件 \"%s\" 的全部内容.\n", path);
		exit(1);
	}
	buffer[size] = '\0';
	fclose(file);
	return buffer;
}

/**
 * @brief 运行并分析整个文件的内容。
 * @param path 要分析的文件路径。
 */
static void runFile(const char *path) {
	char *source = readFile(path); // 读取文件内容
	run(source);                   // // 调用 run 函数处理源文件生成的字符串
	free(source);                  // 及时释放资源
}

/**
 * @brief 主函数，根据命令行参数决定程序行为。
 * @param argc 命令行参数的数量。
 * @param argv 命令行参数的数组。
 * @return 程序退出码。
 * @note 主函数支持操作系统传递命令行参数，并根据参数决定程序行为。\n
 * 如果没有主动传入参数 (argc = 1), 因为第一个参数总会传入一个当前可执行文件的目录作为命令行参数，此时执行 repl 函数。\n
 * 如果传递了一个参数 (argc = 2), 说明传递了一个参数, 将传递的参数视为某个源代码的路径，然后调用 runFile 函数, 传入该源代码文件的路径, 处理源文件。\n
 * 如果传递多个参数 (argc > 2), 说明传递了多个参数, 进行错误处理。
 */
int main(int argc, const char *argv[]) {
	if (argc == 1) {
		// 交互式的输入源代码字符串，然后词法分析
		repl();
	} else if (argc == 2) {
		// 命令行参数输入一个源文件的路径名，然后词法分析此源文件代码
		runFile(argv[1]);
	} else {
		// 如果主动传入超过一个命令行参数. 即参数传递有误, 错误处理
		// 告诉用户正确的使用函数的方式
		fprintf(stderr, "用法：参数 [路径]\n");
		exit(1);
	}
	return 0;
}
