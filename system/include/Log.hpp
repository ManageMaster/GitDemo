#pragma once
#include <iostream>

#define LOG_LVL    TRACE
#define LOG_POS    "[",__FUNCTION__,":",__LINE__,"]"


#define LOG_NONE                  "\033[m"
#define LOG_YELLOW                "\033[1;33m"
#define LOG_RED                   "\033[0;32;31m"
#define LOG_GREEN                 "\033[0;32;32m"
#define LOG_BLUE                  "\033[0;32;34m"
#define LOG_PURPLE                "\033[0;35m"

#define LOG_TRACE(args...) \
	Log(TRACE,"TRACE->",LOG_POS, ##args)
#define LOG_INFO(args...) \
	Log(INFO,LOG_GREEN,"INFO->",LOG_POS, ##args,LOG_NONE)
#define LOG_DEBUG(args...) \
	Log(DEBUG,LOG_YELLOW,"DEBUG->",LOG_POS, ##args,LOG_NONE)
#define LOG_WARNING(args...) \
	Log(WARNING,LOG_PURPLE,"WARNING->",LOG_POS, ##args,LOG_NONE)
#define LOG_ERROR(args...) \
	Log(ERROR,LOG_RED,"ERROR->",LOG_POS, ##args,LOG_NONE)


#define LOG_POINT()   std::cout << "TestPoint->" << __FUNCTION__ << ":" << __LINE__ << std::endl

enum LogLevel {
	TRACE = 0,
	INFO,
	DEBUG,
	WARNING,
	ERROR
};


template<typename T>
static void _log(T value) {
	std::cout << value << std::endl;
}

template<typename T, typename ...types>
static void _log(T value, types ... args) {
	std::cout << value;
	_log(args ...);
}

template<typename ...types>
static void Log(LogLevel _level, types ...args) {
	if (_level >= LOG_LVL) {
		_log(args ...);
	}
};