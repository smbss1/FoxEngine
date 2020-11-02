
#include <iostream>

#define Black        30
#define Red          31
#define Green        32
#define Yellow       33
#define Blue         34
#define Magenta      35
#define Cyan         36
#define White        37
#define Stringify(a) #a
#define LaterStringify(a) Stringify(a)
#define Bold(color) "\033[1;" LaterStringify(color) "m"
#define Underline(color) "\033[4;" LaterStringify(color) "m"

#define Log(...) do { std::cout << __VA_ARGS__ << std::endl; } while(0);
#define LogColor(color, ...) do { std::cout << (color); std::cout << __VA_ARGS__ << "\033[0m"; } while(0);
#define LogError(...) do { std::wcerr << __VA_ARGS__ << std::endl; } while(0);