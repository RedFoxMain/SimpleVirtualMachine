#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void throw_error(const char* msg, ...);

#endif // !ERROR_H