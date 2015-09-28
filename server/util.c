#include "system.h"


int utilRemoveBlankFromString(char *string)
{
	int i;
	int srcindex = 0;
	int destindex = 0;
	char removechar[] = {" \r\n\t"};
	while (string[srcindex] != '\0')
	{
		for (i = 0; i < sizeof(removechar);i++)
		{
			if (string[srcindex] == removechar[i])
			{
				break;
			}
		}
		if (i >= sizeof(removechar))
		{
			string[destindex++] = string[srcindex];
		}
		srcindex++;
	}
	string[destindex] = '\0';
	return destindex;
}

BOOL utilGetDelimiterString(char *src, char *delim, int index, char *dest, int size)
{
	int delim_len;
	char *next;

	*dest = '\0';
	delim_len = strlen(delim);
	if (delim_len <= 0)
	{
		char string[256];
		sprintf(string, "分割符号发生问题([%s],[%s],[%d]).\n", src, delim, size);
		LogWrite(LT_SYSTEM, string);
		return FALSE;
	}
	if (size <= 0)
	{
		char string[256];
		sprintf(string, "缓冲区被设定成不合理的尺寸([%s],[%s],[%d]).\n", src, delim, size);
		LogWrite(LT_SYSTEM, string);
	}
	while (--index > 0)
	{
		next = strstr(src, delim);
		if (next == NULL)
		{
			return FALSE;
		}
		src = next + delim_len;
	}
	size--;

	next = strstr(src, delim);
	if (next)
	{
		int len;
		len = next - src;
		if (len < size)
			size = len;
	}
	strncpy(dest, src, size);
	dest[size] = '\0';
	return TRUE;
}
