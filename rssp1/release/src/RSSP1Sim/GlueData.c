#include "GlueData.h"

UINT g_nNumcdw = 8000;
UINT g_nNumcdwRecv = 8000;
UINT g_nNumcdwSend = 8000;

void* g_hFormView = NULL;

SND_MSG		g_sendMsg;
UINT m_SendLength;
BUG_INSERT	g_bugInsert;
BOOL	g_LongtimeRun = 0;
