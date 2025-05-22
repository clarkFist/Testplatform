/**
* @file BaseMacro.h
* @brief 
* @author DuYuesheng
* @date 2009.05.17
*/

#pragma once

#define TO_STR(x) #x

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&);             \
	void operator=(const TypeName&)

template <typename T, size_t N>
char (&ArrayCountHelper(T (&array)[N]))[N];

#define ARRAY_COUNT(array) (sizeof(ArrayCountHelper(array)))

#define SELECT_ANY __declspec(selectany)

