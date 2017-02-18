#ifndef _STRTOK_S_H
#define _STRTOK_S_H

/***
*tcstok_s.inl - implementation of strtok_s
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file contains the algorithm for strtok_s.
*
****/

#ifdef _UNICODE
wchar_t * wcstok_s(wchar_t *_String, const wchar_t *_Control, wchar_t **_Context);
#define _tcstok_s wcstok_s
#else
char * strtok_s(char *_String, const char *_Control, char **_Context);
#define _tcstok_s strtok_s
#endif


#endif