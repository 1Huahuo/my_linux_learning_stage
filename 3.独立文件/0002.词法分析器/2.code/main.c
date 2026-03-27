#define _CRT_SECURE_NO_WARNINGS
#include "scanner.h"

// 全局变量结构体对象,后续所有函数的操作都基于同一个scanner结构体对象
Scanner scanner; // 注意不要使用静态定义，不然无法在scanner.c里使用
char message[50]; // 该全局变量字符数组用于存储打印errToken时,存放错误信息

// 对词法分析器Scanner 进行初始化 
void initScanner(const char* source) { // 这个source就是处理一条语句或者是.c源文件转换而来的字符串
	scanner.start = source; // 初始化全局变量scanner
	scanner.current = source; // start和current一开始都可以指向源代码字符串的起始位置
	scanner.line = 1;
}

static void run(const char* source) {
	initScanner(source); // 此source代表着一行语句的字符串或者是一个.c源文件的字符串
	int line = -1; // 记录当前处理的token所处的行的行号 词元(词法单元)，-1代表还未开始处理
	
	while (1) {
		Token token = scanToken(); // 获取下一个token
		if (token.line != line) { // 即代表换行
			printf("%4d ", token.line);  // 格式
			line = token.line;
		}
		else { // 注意此else不可省略，因为换行了是要将 | 替换成新行号
			printf("   | ");
		}
		// "."在非浮点数的情况下, 其实表示的是打印的宽度
		// "*"是宽度占位符，也要对应一个参数并且是整型
		printf("%2d '%.*s'\n", token.type, token.length, token.start);
		
		if (token.type == TOKEN_EOF)
			break; // 读取到TOKEN_EOF即代表文件结束，这里EOF代表空字符，因为此时文件与语句
				   // 都转换成了字符串了，自然结尾也就没有EOF而是\0了
	}
}

static void repl() { // 循环：读取、求值、打印
	char line[1024]; // 存一行用1024字节，存一个文件用4096字节
	while (1) {
		printf("> "); // >之后即此次输入的一条语句，格式
		if (fgets(line, 1024, stdin) == NULL) { // 从标准输入流读取一行字符串即我们输入的语句
			printf("\n"); // 如果没有读取成功或者输入为空则换行并退出循环
			break;
		}
		run(line); // 开始分析这行语句，run为核心函数，用来处理代表源文件的字符串
	}
}

// 被runFile调用
static char* readFile(const char* path) {
	// 用户输入文件路径，将整个文件的内容读入内存，并在末尾添加'\0'
	// 注意: 这里应该使用动态内存分配，因此应该事先确定文件的大小
	FILE* file = fopen(path, "rb");
	if (file == NULL) {
		fprintf(stderr, "Could not open file \"%s\".\n", path); // \"代表非语句格式中的"
		exit(1);
	}
	// 获取文件的大小并倒带
	fseek(file, 0L, SEEK_END);
	long file_size = ftell(file);
	rewind(file);
	char* buffer = (char*)malloc(file_size + 1); // 不要忘记结尾要添加一个'\0'代表字符串
	if (buffer == NULL) {
		printf("error:Not enough memory to read \"%s\".\n", path);
		exit(1);
	}
	// 一次性将文件读入到字符串数组，别忘记最后手动置0，因为文件中是没有空字符的
	size_t read_size = fread(buffer, 1, file_size, file);
	if (read_size != file_size) {
		fprintf(stderr, "Could not read file \"%s\".\n");
		exit(1);
	}
	buffer[file_size] = '\0';

	fclose(file);
	return buffer; // 返回源文件转换成的字符串
}

static void runFile(const char* path) { // 处理一整个.c源文件
	char* source = readFile(path); // 生成一个包含源文件所有内容的字符串
	run(source); // 调用核心函数处理源文件
	free(source); // 不要忘记释放source指向的malloc空间(在readFile中申请)
}

int main(int argc,const char* argv[]) {
	if (argc == 1) {
		repl(); // 此时只有一个可执行文件目录的参数，无文件路径参数，即处理用户输入的语句
	}
	else if (argc == 2) {
		runFile(argv[1]); // argv[1]即额外的参数就是要传入的文件的路径，即处理一整个源文件
	}
	else {
		fprintf(stderr, "usage：scanner [path]\n "); // 即多输入了参数，告诉用户正确用法
		exit(1);
	}
	return 0;
}
