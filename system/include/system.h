#pragma once

#include "Auto_Head.h"

#include <iostream>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include "Log.hpp"

#define DELAY_S(sec)       sleep(sec)
#define DELAY_MS(usec)     usleep(usec * 1000)


#define MEMCLR(data,size)    memset(data,0,size)
typedef enum {
	OK = 0,
	FAIL
} err_t;


