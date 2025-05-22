/**
* @file Singleton.h
* @brief A slight implement for GOF singleton pattern.
* @author DuYuesheng
* @date 2009.05.16
* 
* If you want to make sure that only the singleton instance of
* <TYPE> is created, and that users cannot create their own
* instances of <TYPE>, do the following to class <TYPE>:
* (a) Make the constructor of <TYPE> private (or protected)
* (b) Make Singleton a friend of <TYPE>
* Here is an example:
* @verbatim
* class foo
* {
*	SINGLETON(foo);
*	private:
*	foo () { cout << "foo constructed" << endl; }
*	~foo () { cout << "foo destroyed" << endl; }
* };
* typedef TSingleton<foo> FOO;
* @endverbatim
*
* @note Not thread-safe!!! When you call Get or Free <B>at first time</B>, 
* please make sure only the one thread do that at the same time, otherwise
* maybe more than one object will be created.
*/

#pragma once

template <typename TYPE>
class TSingleton
{
public:
	static TYPE* Get()
	{
		if (sm_pObj == NULL)
			sm_pObj = new TSingleton<TYPE>();
		return &sm_pObj->m_instane;
	}

	static void Free()
	{
		delete sm_pObj;
		sm_pObj = NULL;
	}

private:
	TSingleton() {};

	static TSingleton* sm_pObj;
	TYPE m_instane;
};

template<typename TYPE>
__declspec(selectany) TSingleton<TYPE>* TSingleton<TYPE>::sm_pObj = NULL;

#define SINGLETON(CLASS) friend TSingleton<CLASS>
