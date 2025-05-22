#ifndef _TB_RSSP1_H
#define _TB_RSSP1_H

#include "GlueData.h"
#include "../RSSP1/GM_Define.h"

#ifdef WIN32
#define TB_RSSP1_FILE_PATH "."
#else
#define TB_RSSP1_FILE_PATH "/tffs0"
#endif

#ifdef __cplusplus
extern"C"
{
#endif /**< __cplusplus */

    /**
    * @brief TB_RSSP1_Init
    *
    * Initial RSSP1 protocol.
    * @return true if successfully initialized.
    */
    bool TB_RSSP1_Init(char* fileName);

	/**
    * @brief TB_RSSP1_Recv
    *
    * RSSP1 receive function.
    * @return void
    */
	void TB_RSSP1_Recv();

	/**
    * @brief TB_RSSP1_Send
    *
    * RSSP1 send function.
    * @return void
    */
	void TB_RSSP1_Send();

	/**
    * @brief TB_RSSP1_Close
    *
    * RSSP1 close function.
    * @return void
    */
	void TB_RSSP1_Close();


#ifdef __cplusplus
}

#endif /**< __cplusplus */

#endif /**< _TB_RSSP1_H */

