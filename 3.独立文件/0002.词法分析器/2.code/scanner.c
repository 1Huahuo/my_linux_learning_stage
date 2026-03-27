#include "scanner.h"

// 检查字符c是否是字母或下划线
bool isAlphaUnderline(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

// 检查字符c是否是数字
bool isDigit(char c) {
	return c >= '0' && c <= '9';
}

// 判断词法分析器当前正在处理的字符是不是空字符(即判断是否处理完了)，curr不动
bool isAtEnd() {
	return *scanner.current == '\0';
}

// curr指针前进一个字符,并返回之前curr指针指向的元素
char advance() {
	//char t = *scanner.current;
	//scanner.current++;
	//return t;
	// 其实有更简单的方法，即：
	return *scanner.current++;
}

// 查看当前正在处理的字符是什么,curr不动
char peek() {
	return *scanner.current;
}

// 如果当前正在处理的字符不是空字符,那就返回下一个要处理的字符,但curr不动
char peekNext() { // 注意：这里不要推进curr指针
	if (*scanner.current != '\0')
		//return *(++scanner.current);
		return *(scanner.current + 1);
	return '\0';
}

// 检查词法分析器当前正在处理的字符是不是符合预期,如果符合预期,curr前进一位
bool match(char expected) {
	if (isAtEnd())
		return false;
	if (peek() != expected)
		return false;
	scanner.current++; // 即符合预期，并且在返回true之前别忘记将curr往前推进一位
	return true;
}

// 根据传入的TokenType类型来制造返回一个Token
Token makeToken(TokenType type) {
	Token token;
	token.type = type;
	token.start = scanner.start;
	token.length = (int)(scanner.current - scanner.start);
	token.line = scanner.line;
	return token;
}

// 当遇到不可识别的字符时,就返回一个TOKEN_ERROR类型的Token
// 比如遇到@，$等不在处理范围内的符号时，比如处理字符串，字符没有对应的右引号时。
Token errorToken(const char* message) {
	Token token;
	token.type = TOKEN_ERROR;
	token.start = message;
	token.length = (int)strlen(message);
	token.line = scanner.line;
	return token;
}

// 跳过空白字符，注意不是空字符
// 跳过空白字符: ' ', '\r', '\t', '\n'和注释
	/*
	空格,制表,回车：就advance()前进
	换行：那就sc.line++再前进
	注释：如果下一个字符是/,那就再瞥一眼下下个字符,如果还是/,那就是注释,此时要把一整行都要跳过,用while循环
	结束：只要不是下一个字符不是上面提出来的几个,就结束死循环
	*/
void skipWhitespace() {
	//for (;;)
	while (1) {
		char c = peek();
		switch (c) {
		case ' ':
		case '\r': // 回车
		case '\t': // 制表符Tab
			advance();
			break;
		case '\n':
			scanner.line++;
			advance();
			break;
		case '/':
			if (peekNext() == '/')// 当前是'/'下一个也是'/'即遇到了单行注释
				while (!isAtEnd() && peek() != '\n') // 跳过一整行注释，遇到空字符或换行符停
					advance();
			else
				return; // 遇到了'/'运算符即非空白字符，则退出当前函数，转为正常处理
			break;
			// 凡break都代表跳出当前switch从而读取下一个字符继续进行while循环
		default:
			return;
		}
	}
}


/*
		start: 待检查序列的起始字符下标
			比如要检查关键字break，那么在case b的前提下，需要传入reak来进行检查
			这里start就等于1，scanner.start[1]
		length: 待检查序列的长度，如果检查的是break，就是检查剩余的reak
			需要传入4
		rest指针，待检查的剩余序列字符串，这里直接传入一个字面值字符串就行了
			比如检查break，传入"reak"就好了
		type：你要检查的关键字Token的类型，比如检查break，那就传入Token_BREAK

		检查从start位置开始，长度为length的字符串是否与给定的rest字符串相匹配
		先判断长度是否一致 再判断内容
*/
// 检查关键字类型的辅助函数
// 判断类型是不是指定的type 如果是就返回type
TokenType checkKeyword(int start, int length, const char* rest, TokenType type) {
	// memcmp库函数
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
	if (start + length == scanner.current - scanner.start && memcmp(scanner.start + start, rest, length) == 0)
		return type;
	return TOKEN_IDENTIFIER;
}

/*
确定identifier类型主要有两种方式：
	1.	将所有的关键字放入哈希表中，然后查表确认
		Key-Value 就是"关键字-TokenType" ,但需要额外内存，且效率不如法2
	2.	将所有的关键字放入Trie树(读踹，字典查找树)中，然后查表确认
		Trie树的方式不管是空间上还是时间上都优于哈希表的方式——trie树：字典树/前缀树
*/
// 检查是标识符还是关键字的主函数，若是关键字并确定它的类型-通过辅助函数checkKeyword
TokenType identifierType() {
	char c = scanner.start[0];
	// 用switch...switch...if组合构建逻辑上的trie树
	switch (c) {
		//keyword
	case 'b':
		return checkKeyword(1, 4, "reak", TOKEN_BREAK);
	case 'c': { // case后加大括号单纯是因为case标签后不能定义变量，变量定义要在语句块内
		int len = scanner.current - scanner.start; // 当前识别的标识符的长度，此时cur在当前标识符的结尾的下一个字节
		if (len > 1) { // 不可改成len > 3，因为会丧失可扩展性，关键是不符合trie树的匹配逻辑
			switch (scanner.start[1]) {
			case 'a':
				return checkKeyword(2, 2, "se", TOKEN_CASE);
			case 'h':
				return checkKeyword(2, 2, "ar", TOKEN_CHAR);
			case 'o':
				if (len > 3 && scanner.start[2] == 'n') {
					switch (scanner.start[3]) {
					case 's': // 也可延续上面的if判断，但是没有必要，因为只有满足前缀的只有1个了
						return checkKeyword(4, 1, "t", TOKEN_CONST);
					case 't':
						return checkKeyword(4, 4, "inue", TOKEN_CONTINUE);
					}
				}
			}
		}
		break; // 跳出开头为'c'的switch匹配，进入开头为'd'的总的switch匹配
	}  // case 'c':
	case 'd': {
		int len = scanner.current - scanner.start;
		if (len > 1) {
			switch (scanner.start[1]) {
			case 'e':
				return checkKeyword(2, 5, "fault", TOKEN_DEFAULT);
			case 'o':
				if (len == 2)
					return TOKEN_DO;
				else
					return checkKeyword(2, 4, "uble", TOKEN_DOUBLE);
			}
		}
		break;
	} // case 'd':
	case 'e': {
		int len = scanner.current - scanner.start;
		if (len > 1) {
			switch (scanner.start[1]) {
			case 'l':
				return checkKeyword(2, 2, "se", TOKEN_ELSE);
			case 'n':
				return checkKeyword(2, 2, "um", TOKEN_ENUM);
			}
		}
		break;
	} // case 'e':
	// 下面同理，就不一一手敲了————————————————————————————————
	case 'f': {
		int len = scanner.current - scanner.start;
		if (len > 1) {
			switch (scanner.start[1]) {
			case 'l': return checkKeyword(2, 3, "oat", TOKEN_FLOAT);
			case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR);
			}
		}
		break;
	} // case 'f':
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
	} // case 'i':
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
	} // case 's':
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
	} // case 'u':
	case 'v': return checkKeyword(1, 3, "oid", TOKEN_VOID);
	case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE);
	} // 总switch

	return TOKEN_IDENTIFIER; // 没有在上面switch中返回，那肯定不是关键字，而是标识符
} // 函数到此为止

// 进入char型单字符(但''之间可能存在多个字符，属于错误)处理模式
Token character() {
	// 字符'开头，以'结尾，而且不能跨行，不支持转义字符，''中间必须只有一个字符
	// 如果下一个字符不是末尾也不是单引号，全部跳过(curr可以记录长度，不用担心)

	// 1.判断当前curr指向的是不是空字符,如果是空字符,那就返回一个errToken
	if (isAtEnd()) { // 此时没有右'来跟左'配对就结束了，即出现错误
		return errorToken("此字符不完整,缺少右单引号!");
	}
	// 2.让curr指针继续走,找到右单引号
	while (!isAtEnd() && peek() != '\'') {
		// 如果当前正在处理的字符既不是空字符,也不是右单引号,那就继续走
		if (peek() == '\n') {
			return errorToken("不支持多行字符!"); // 即规定不允许换行字符,注:可能不为单字符
		}
		advance();
	} // while循环结束时,要么处理到了空字符,要么处理到了右单引号
	if (isAtEnd()) {
		return errorToken("此字符不完整,缺少右单引号!");
	} // 跳出if循环就代表成功处理到了右单引号
	// （重点）结束一个Token处理时，要保证curr指针移动向此Token的下一个位置！！！！
	advance();
	// 接下来要判断一下此字符是不是单个长度,单引号中出现多个字符是不允许的
	int charLen = scanner.current - scanner.start - 2;
	if (charLen == 1 || charLen == 0) {
		// ！注：关于len=0的情况即源文件中的''中间没有任何东西，在这里算作源文件中的空字符，而不是
		// 源文件转换成的字符串之中的空字符，但是在任何标准C语法中都是非法的，只在小众编译器中合法
		return makeToken(TOKEN_CHARACTER);
	}
	// 需求: 现在单引号中间的字符序列长度超长了,于是要打印这个超长的序列
	// 输出效果是: 非单字符Token: xxx(单引号中间的序列)
	char* charStart = scanner.start + 1;
	// 我们需要格式化的向目标字符数组中输出一个字符串
	sprintf(message, "非单字符Token: %.*s", charLen, charStart);
	return errorToken(message);
}

// 当前Token的开头是下划线或字母判断它是不是标识符Token，此时只确定是疑似标识符，还可能是关键字
// 这是判断identifierType即到底是标识符还是关键字的函数的前置函数
Token identifier() {
	// 判断curr指针当前正在处理的字符是不是 字母 下划线 数字
	while (isAlphaUnderline(peek()) || isDigit(peek())) {
		advance();  // 继续前进看下一个字符 直到碰到下一个字符不是字母 下划线 以及数字 结束Token
	}
	// 当while循环结束时，scanner.curr指针指向的是该Token字符串的下一个字符
	// 这个函数的意思是: 只要读到字母或下划线开头的Token我们就进入标识符模式
	// 然后一直找到此Token的末尾
	// 但代码运行到这里还不确定Token是标识符还是关键字, 因为它可能是break, var, goto, max_val...
	// 于是执行identifierType()函数，它是用来确定Token类型的
	return makeToken(identifierType());
	// makeToken中是一个函数，通过嵌套来返回一个标识符或者关键字
}

// 到这步已经确定是数字了，只是处理即生成一个数字Token而已，而不是判断，所以没有非数字的错误处理
Token number() {
	// 简单起见，我们将NUMBER的规则定义如下:
	// 1. NUMBER可以包含数字和最多一个'.'号
	// 2. '.'号前面要有数字
	// 3. '.'号后面也要有数字
	// 这些都是合法的NUMBER: 123, 3.14
	// 这些都是不合法的NUMBER: 123., .14(虽然在C语言中合法)
	// 这个过程要不断的前进跳过所有的数字,
	// 比如数字123.456 最终要保证sc.start指向1,curr指向6后面第一个非数字字符
	while (isDigit(peek()))
		advance();
	// 查找有无小数部分
	if (peek() == '.' && isDigit(peekNext())) {
		// 跳过小数点
		advance();
		while (isDigit(peek()))
			advance();
	}
	return makeToken(TOKEN_NUMBER);
}

// 与上面数字同理，已经确实是字符串了，只是生成一个字符串Token而已
Token string() { // 注：进来前已经检测到了左"，此时处于字符串内部的第一个字符
	// 字符串以"开头，以"结尾，而且不能跨行
	// 为了简化工作量
	// 如果下一个字符不是末尾也不是双引号，全部跳过(curr可以记录长度，不用担心)
	while (!isAtEnd() && peek() != '"') {
		if (peek() == '\n') {
			return errorToken("Not support multi-line string."); // 不支持换行字符串
		}
		advance();
	}
	if ( isAtEnd() )
		return errorToken("Unterminated string.");
/*
注意：这个isAtEnd函数检测的只是源文件转换的字符串中的空字符，而不是字符串中的空字符，因为若是
	字符串中的空字符在这里是用4个字符来代替的即'\0'，并且这个是字符串的内容而不代表空字符，因为转
	换过程中是读取不到空字符的（因为空字符是隐藏的）
	举例：
		main.c中，一行语句是char * str = "hello"; 读取到字符串变为"char *str = "hello";"，
		这样的方式是读取不到空字符的，所以下面才会打印错误信息：这是一个不完整的字符串，
		此时所以源文件对应的字符串的内容已经被全部读完，还未读到右"就已经戛然而止
*/			
	advance(); // 使current一直处于当前已处理完的字符的下一个字符即新的待处理字符处
	return makeToken(TOKEN_STRING);
}

// 处理无法识别的字符
Token errorTokenWithChar(char c) {
	// 将无法识别的字符是什么输出
	sprintf(message, "Unexpected character: %c", c);
	return errorToken(message);
}

// Scanner核心逻辑,用于返回制作好的Token对象
Token scanToken() {
	// 跳过前置空白字符和注释
	skipWhitespace();
	/*
		curr指针在处理完一个Token后,就会指向该Token的下一个字符位置
		而且由于skipWhitespace的存在会跳过前面的空白字符
		所以curr指针此时一定指向下一个Token的开始字符
		于是记录下一个Token的起始位置
		此时scanner.start就指向这一个要处理Token的首字符
	*/
	scanner.start = scanner.current;

	// 如果当前正在处理的字符就是空字符,那就返回TOKEN_EOF,表示处理结束了
	if (isAtEnd()) {
		return makeToken(TOKEN_EOF);
	}

	char c = advance(); // 将当前Token的第一个字符传给c，且current移动一位

	// 如果Token的第一个字符是字母和下划线就进入标识符的处理模式
	if (isAlphaUnderline(c))
		return identifier();

	// 如果Token的第一个字符是数字,那就进入数字的处理模式
	if (isDigit(c))
		return number();

	// 如果Token的第一个字符既不是数字也不是字母和下划线,那么就switch处理它
	switch (c) {
		// 第一部分，处理单字符Token———————————————————————————————
	case '(':
		return makeToken(TOKEN_LEFT_PAREN);
	case ')':
		return makeToken(TOKEN_RIGHT_PAREN);
	case '[':
		return makeToken(TOKEN_LEFT_BRACKET);
	case ']':
		return makeToken(TOKEN_RIGHT_BRACKET);
	case '{':
		return makeToken(TOKEN_LEFT_BRACE);
	case '}':
		return makeToken(TOKEN_RIGHT_BRACE);
	case ',':
		return makeToken(TOKEN_COMMA);
	case '.':
		return makeToken(TOKEN_DOT);
	case ';':
		return makeToken(TOKEN_SEMICOLON);
	case '~':
		return makeToken(TOKEN_TILDE);
		// 第一部分，处理单或双字符Token——————————————————————————————
	case '+': // 即开头是+且开始先默认有第二个字符，若能进行到最后则为单字符+
		if (match('+')) // 注意match判断的是第二个字符是否为+，因为上面那个+是c的，
			//									  而c存的是curren没移动之前的字符
			return makeToken(TOKEN_PLUS_PLUS);
		else if (match('='))
			return makeToken(TOKEN_PLUS_EQUAL);
		else // 如果都不是，那就是单字符+
			return makeToken(TOKEN_PLUS);
	case '-':
		if (match('-'))
			return makeToken(TOKEN_MINUS_MINUS);
		else if (match('='))
			return makeToken(TOKEN_MINUS_EQUAL);
		else if (match('>'))
			return makeToken(TOKEN_MINUS_GREATER);
		else
			return makeToken(TOKEN_MINUS);
	case '*': // 总共只有两种情况那就用三目运算符更简单
		return makeToken(match('=') ? TOKEN_STAR_EQUAL : TOKEN_STAR);
	case '/':
		return makeToken(match('=') ? TOKEN_SLASH_EQUAL : TOKEN_SLASH);
	case '%':
		return makeToken(match('=') ? TOKEN_PERCENT_EQUAL : TOKEN_PERCENT);
	case '&':
		if (match('='))
			return makeToken(TOKEN_AMPER_EQUAL);
		else if (match('&'))
			return makeToken(TOKEN_AMPER_AMPER);
		else
			return makeToken(TOKEN_AMPER);
	case '|':
		if (match('='))
			return makeToken(TOKEN_PIPE_EQUAL);
		else if (match('|'))
			return makeToken(TOKEN_PIPE_PIPE);
		else
			return makeToken(TOKEN_PIPE);
	case '^':
		return makeToken(match('=') ? TOKEN_HAT_EQUAL : TOKEN_HAT);
	case '=':
		return makeToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
	case '!':
		return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
	case '<':
		if (match('='))
			return makeToken(TOKEN_LESS_EQUAL);
		else if (match('<'))
			return makeToken(TOKEN_LESS_LESS);
		else
			return makeToken(TOKEN_LESS);
	case '>':
		if (match('='))
			return makeToken(TOKEN_GREATER_EQUAL);
		else if (match('>'))
			return makeToken(TOKEN_GREATER_GREATER);
		else
			return makeToken(TOKEN_GREATER);
		// 第三部分，多字符Token处理,注：上面已经处理过标识符和数字了————————————————
	case '"': // 如果Token的第一个字符是双引号,那就进入字符串处理模式
		return string();
	case '\'':	// 如果Token的第一个字符是单引号,那就进入字符处理模式 
		return character();
	} // 用来识别非标识符非数字的总的switch
	// 如果读到的字符不在上面的所有之列,那就无法识别,于是制造一个errorToken返回
	// 比如代码中的#(预处理阶段就没了) $(C语言不用该字符) 注释中的中文
	return errorTokenWithChar(c);
}