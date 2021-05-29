#pragma once

#include <iostream>
#include <string>

#define LOG_INF  "LOG_INFO"
#define LOG_WARN "LOG_WARNING"
#define LOG_ERR  "LOG_ERROR"

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#ifdef _DEBUG
	#define LOG(LOG_TYPE, MESSAGE)                               \
		do                                                       \
		{                                                        \
			std::cout <<                                         \
				LOG_TYPE << ": [" << __FILENAME__ << " : " <<    \
				__FUNCTION__ << "() : " << __LINE__ << "] : " << \
				MESSAGE <<                                       \
			std::endl;                                           \
		} while(0);                                              \

#else
	#define LOG(LOG_TYPE, MESSAGE)                               \
		do                                                       \
		{                                                        \
			std::cout <<                                         \
				LOG_TYPE << ": " << MESSAGE <<                   \
			std::endl;                                           \
		} while(0);                                              \

#endif
