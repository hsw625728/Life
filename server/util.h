#ifndef __TUIL_H__
#define __TUIL_H__
#include "type.h"
#define utilArraySizeof(x) (sizeof(x)/sizeof(x[0]))
int utilRemoveBlankFromString(char *string);
BOOL utilGetDelimiterString(char *src, char *delim, int index, char *dest, int size);

#endif
