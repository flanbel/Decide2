#pragma once

/**
 * アサート.
 *
 * @param flag   True to flag.
 * @param format Describes the format to use.
 * @param file   The file.
 * @param line   The line.
 * @param ...    Variable arguments providing additional information.
 */
static inline void Assert(bool flag, const char* format, const char*file, long line, ...)
{
	if (!flag) {
		//可変長引数
		va_list va;
		//可変長引数の初期化
		va_start(va, flag);
		vprintf(format, va);
		char fileLineInfo[256];
		sprintf(fileLineInfo, "%s, %d行目", file, line);
		char assertMessage[256];
		vsprintf(assertMessage, format, va);
		strcat(assertMessage, fileLineInfo);
		//メッセージボックスで警告を出す
		MessageBoxA(nullptr, assertMessage, "アサート", MB_OK);
		//可変個引数リストの終了
		va_end(va);
		abort();
	}
}

#ifdef _DEBUG
#define 	FB_ASSERT( flg, format, ... )	Assert( flg, format, __FILE__, __LINE__, __VA_ARGS__)
#else
#define 	FB_ASSERT( flg, format, ... )
#endif // !_DEBUG