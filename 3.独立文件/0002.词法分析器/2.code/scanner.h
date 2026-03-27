#ifndef SCANNER_H
#define SCANNER_H
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>



// 定义一个TokenType枚举，用于标记不同种类的Token
typedef enum {
    /* 单字符 Token */
    TOKEN_LEFT_PAREN,        // '(' 左小括号
    TOKEN_RIGHT_PAREN,       // ')' 右小括号
    TOKEN_LEFT_BRACKET,      // '[' 左中括号
    TOKEN_RIGHT_BRACKET,     // ']' 右中括号
    TOKEN_LEFT_BRACE,        // '{' 左大括号
    TOKEN_RIGHT_BRACE,       // '}' 右大括号
    TOKEN_COMMA,             // ',' 逗号
    TOKEN_DOT,               // '.' 点
    TOKEN_SEMICOLON,         // ';' 分号
    TOKEN_TILDE,             // '~' 波浪号

    /* 可能是单字符或双字符的Token */
    TOKEN_PLUS,                  // '+' 加号
    TOKEN_PLUS_PLUS,             // '++' 自增运算符
    TOKEN_PLUS_EQUAL,            // '+=' 加赋运算符
    TOKEN_MINUS,                 // '-' 减号或负号
    TOKEN_MINUS_MINUS,           // '--' 自减运算符
    TOKEN_MINUS_EQUAL,           // '-=' 减赋运算符
    TOKEN_MINUS_GREATER,         // '->' 结构体指针访问
    TOKEN_STAR,                  // '*' 乘号
    TOKEN_STAR_EQUAL,            // '*=' 乘赋运算符
    TOKEN_SLASH,                 // '/' 除号
    TOKEN_SLASH_EQUAL,           // '/=' 除赋运算符
    TOKEN_PERCENT,               // '%' 取模运算符
    TOKEN_PERCENT_EQUAL,         // '%=' 取模赋运算符
    TOKEN_AMPER,                 // '&' 按位与运算符
    TOKEN_AMPER_EQUAL,           // '&=' 按位与赋运算符
    TOKEN_AMPER_AMPER,           // '&&' 逻辑与运算符
    TOKEN_PIPE,                  // '|' 按位或运算符
    TOKEN_PIPE_EQUAL,            // '|=' 按位或赋运算符
    TOKEN_PIPE_PIPE,             // '||' 逻辑或运算符
    TOKEN_HAT,                   // '^' 按位异或运算符
    TOKEN_HAT_EQUAL,             // '^=' 按位异或赋运算符
    TOKEN_EQUAL,                 // '=' 赋值运算符
    TOKEN_EQUAL_EQUAL,           // '==' 等于比较运算符
    TOKEN_BANG,                  // '!' 逻辑非运算符
    TOKEN_BANG_EQUAL,            // '!=' 不等于比较运算符
    TOKEN_LESS,                  // '<' 小于比较运算符
    TOKEN_LESS_EQUAL,            // '<=' 小于等于比较运算符
    TOKEN_LESS_LESS,             // '<<' 左移运算符
    TOKEN_GREATER,               // '>' 大于比较运算符
    TOKEN_GREATER_EQUAL,         // '>=' 大于等于比较运算符
    TOKEN_GREATER_GREATER,       // '>>' 右移运算符


    /*
        所有的三字符Token都去掉了, 比如:">>= <<="等
        实现它们也没什么特殊的，但会很无聊繁琐，所以就都去掉了
        以下是多字符的Token: 标识符、字符、字符串、数字
    */
    TOKEN_IDENTIFIER,            // 标识符
    TOKEN_CHARACTER,             // 字符
    TOKEN_STRING,                // 字符串
    TOKEN_NUMBER,                // 数字，包含整数和浮点数

    /* 关键字Token 涉及C99所有关键字 */
    TOKEN_SIGNED, TOKEN_UNSIGNED,
    TOKEN_CHAR, TOKEN_SHORT, TOKEN_INT, TOKEN_LONG,
    TOKEN_FLOAT, TOKEN_DOUBLE,
    TOKEN_STRUCT, TOKEN_UNION, TOKEN_ENUM, TOKEN_VOID,
    TOKEN_IF, TOKEN_ELSE, TOKEN_SWITCH, TOKEN_CASE, TOKEN_DEFAULT,
    TOKEN_WHILE, TOKEN_DO, TOKEN_FOR,
    TOKEN_BREAK, TOKEN_CONTINUE, TOKEN_RETURN, TOKEN_GOTO,
    TOKEN_CONST, TOKEN_SIZEOF, TOKEN_TYPEDEF,

    // 注意：#define #include这样的预处理指令 不是关键字
    // 辅助Token
    // 词法分析阶段也是可以检测出一些错误的 比如$只能在字符和字符串中 比如字符串"acb 缺少右边双引号
    // 词法分析阶段不进行错误处理，只是将错误的Token信息抛出，以待后续统一进行处理
    // 流水线架构每个阶段都可能出错，如果每个阶段都进行错误处理，那代码的可维护性就太差了
    TOKEN_ERROR,                 // 错误Token 词法分析时遇到无法识别的文本
    TOKEN_EOF                    // 文件结束Token 表示源代码已经分析完毕
} TokenType;

// 词法分析器的目的就是生产一个一个的Token对象 
typedef struct {
    // Token的类型, 取任一枚举值
    TokenType type;		
    // Token的起始字符指针
    const char* start;	// start指向source中的字符，source为读入的源代码。
    int length;		    // length表示这个Token的长度
    int line;		    // line表示这个Token在源代码的哪一行, 方便后面的报错和描述Token
}Token; // 这个Token只涉及一个字符指针指向源代码的字符信息,没有在内部保存字符数据

typedef struct {
    // start一开始应该指向源代码字符串的起始位置
    // 当开始扫描一个新的Token时，这个指针指向Token的第一个字符。
    const char* start;
    // 词法分析器当前正在扫描处理的字符,会从某个Token的开头字符开始,一直移到Token结束,指向此Token的下一个字符
    const char* current;
    // 当前读取的行
    int line;
} Scanner;

// 重中之重，若是想将工具函数都封装在scanner.h与scanner.c中则需要引用一下main.c里定义的全局变量
// 因为工具函数里大多都会用到这两个全局变量
extern Scanner scanner;
extern char message[50];

// 对词法分析器Scanner 进行初始化 
void initScanner(const char* source); // 源代码字符串(这里涉及一个将源码转换成字符串的函数)

// 核心API, 调用scanToken(), 就生产一个Token, 也就是源代码中下一段字符数据的Token
Token scanToken();	// 当Token返回的是TOKEN_EOF时，源文件被消耗完毕，词法分析结束
// 以下均为核心函数所用到的工具
//-------------------------------------处理字符的工具-------------------------------------------------
// 检查字符c是否是字母或下划线
bool isAlphaUnderline(char c);

// 检查字符c是否是数字
bool isDigit(char c);

// 判断词法分析器当前正在处理的字符是不是空字符(即判断是否处理完了)，curr不动
bool isAtEnd();

// curr指针前进一个字符,并返回之前curr指针指向的元素
char advance();

// 查看当前正在处理的字符是什么,curr不动
char peek();

// 如果当前正在处理的字符不是空字符,那就返回下一个要处理的字符,但curr不动
char peekNext();

// 检查词法分析器当前正在处理的字符是不是符合预期,如果符合预期,curr前进一位
bool match(char expected);

// 根据传入的TokenType类型来制造返回一个Token
Token makeToken(TokenType type);

// 当遇到不可识别的字符时,就返回一个TOKEN_ERROR类型的Token
// 比如遇到@，$等不在处理范围内的符号时，比如处理字符串，字符没有对应的右引号时。
Token errorToken(const char* message);

// 跳过空白字符，注意不是空字符
void skipWhitespace();

TokenType checkKeyword(int start, int length, const char* rest, TokenType type);

TokenType identifierType();

// 当前Token的开头是下划线或字母判断它是不是标识符Token
Token identifier();

// 处理数字
Token number();

// 处理字符串
Token string();

// 进入字符处理模式
Token character();

// 处理无法识别的字符
Token errorTokenWithChar(char character);

#endif // !SCANNER_H
