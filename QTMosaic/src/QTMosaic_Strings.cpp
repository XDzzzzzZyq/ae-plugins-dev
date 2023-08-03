/*******************************************************************/
/*                                                                 */
/*                      ADOBE CONFIDENTIAL                         */
/*                   _ _ _ _ _ _ _ _ _ _ _ _ _                     */
/*                                                                 */
/* Copyright 2007-2023 Adobe Inc.                                  */
/* All Rights Reserved.                                            */
/*                                                                 */
/* NOTICE:  All information contained herein is, and remains the   */
/* property of Adobe Inc. and its suppliers, if                    */
/* any.  The intellectual and technical concepts contained         */
/* herein are proprietary to Adobe Inc. and its                    */
/* suppliers and may be covered by U.S. and Foreign Patents,       */
/* patents in process, and are protected by trade secret or        */
/* copyright law.  Dissemination of this information or            */
/* reproduction of this material is strictly forbidden unless      */
/* prior written permission is obtained from Adobe Inc.            */
/* Incorporated.                                                   */
/*                                                                 */
/*******************************************************************/

#include "QTMosaic.h"
#include <unordered_map>

std::unordered_map<A_u_long, std::string> g_strs = {
	{StrID_NONE,					""},
	{StrID_Name,					"Quad-Tree Mosaic"},
	{StrID_Description,				"An Quad-Tree based Mosaic postprocessing famity effects. \rCopyright XDzZyq"},
	{StrID_Gain_Param_Name,			"Gain"},
	{StrID_Color_Param_Name,		"Color"},
	{StrID_Count_Name,              "count"}
};


char *GetStringPtr(int strNum)
{
	return (char*)g_strs[strNum].c_str();
}
	