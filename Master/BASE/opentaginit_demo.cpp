/* Auxiliary macros */
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define M_CHECK(...) M_CHECK_(__VA_ARGS__)
#define M_CHECK_(a, b, ...) b

#define M_IS_PAREN(x) M_CHECK(M_IS_PAREN_ x, 0)
#define M_IS_PAREN_(...) 1, 1

#define M_CONCAT(a, b) M_CONCAT_(a, b)
#define M_CONCAT_(a, b) a ## b

/* Conditional definition macros */

#define DEF(x) M_IS_PAREN(x)

#define DEF_IF_0(def) 0
#define DEF_IF_1(def) def

#define VALUE_DEF(x) M_CONCAT(DEF_IF_, DEF(x))(x)

/* Implementation */

#define ID_1 (1)
#define ID_3 (28)
#define ID_4 (29)
#define IsTagOpenInit(name) __IsTagOpenInit(#name,VALUE_DEF(name))
#define SetTagStateInit(name,count,value) __SetTagStateInit(#name,count,value)
static char name_arr[10][50];
static char value_arr[10] = {-1};
static char pos = 0;
char findTag(const char* name)
{
	int i = 0;
	if(strlen(name) >= 50)
	{
		printf("err");//err
	}
	for(i=0;i<pos;i++)
	{
		if( memcmp(name_arr[i],name,strlen(name) ) == 0 )
		{
 		 	break;	
		}			 
	}
	return i;
}
char AddTag(const char* name,char value)
{
	int i = findTag(name);
	if(i < pos)
	{
		printf("err");//err
	}
	if(pos < 10 )
	{
		memcpy(name_arr[pos],name,strlen(name));
		pos ++;	  
	}
	else
	{
		printf("err");//err
	}
	value_arr[pos-1] = value;
	printf("set Tag:%s value:%d\n",name,value);				

	return value_arr[pos-1] ;
}
char __IsTagOpenInit(const char* name,char value)
{
    int i = 0;
    if(value !=0 &&value !=1)
	{
		printf("err");//err
	}
	if(strlen(name) >= 50)
	{
		printf("err");//err
		return 0;
	}
	i = findTag(name);
	if(i < pos)
	{
		return 	value_arr[i];
	}
	else
	{
		if(value != 1 )
        	return 0;
		AddTag(name,value);
		return value;
	}
	
}

char __SetTagStateInit(const char* name,char count,char value)
{
	static char count_arr[10] = {-1};
	int i = 0;
	if(value !=0 &&value !=1)
	{
		printf("err");//err
	}
	if(count < 0 )
	{
		printf("err");//err
	}
	if(strlen(name) >= 50)
	{
		printf("err");//err

	}
	
	i = findTag(name);
	if( count_arr[i] != -2 )
	{
		if(i >= pos)
		{
			printf("Set State of Not Define Tag:%s\n",name);//err
			AddTag(name,0);
		}
		
		if(value_arr[i] == value)
		{
			if(count_arr[i] == -1)
				printf("Tag %s already is %d \n",name,value);
			else
	        	printf("Tag %s Change Value by others to %d \n",name,value);
	        	
	        count_arr[i] = -2;
	        return value;
	        
	    }
		if(i < pos)
		{
	        if(count_arr[i] == -1 )
	            count_arr[i] = count;
	        else if(count_arr[i] < 0)
	        {
	        	printf("err");//err
			}
			count_arr[i] --;
	 		if(count_arr[i] == -1)
			{
				count_arr[i] = -2;
				value_arr[i] = value;
				printf("set tag:%s value:%d\n",name,value_arr[i]);
			}
		}
		else
		{
			printf("err");//err
		}

	}
	return value_arr[i];
}

int main(void)
{
	for(int i = 0 ;i < 10;i++)
	{
		printf("%d\n",IsTagOpenInit(ID_1));
		SetTagStateInit(ID_1,0,0);
	}
}
