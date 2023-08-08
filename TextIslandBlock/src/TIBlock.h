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

/*
	Skeleton.h
*/

#pragma once

#ifndef SKELETON_H
#define SKELETON_H

typedef unsigned char		u_char;
typedef unsigned short		u_short;
typedef unsigned short		u_int16;
typedef unsigned long		u_long;
typedef short int			int16;
#define PF_TABLE_BITS	12
#define PF_TABLE_SZ_16	4096

#define PF_DEEP_COLOR_AWARE 1	// make sure we get 16bpc pixels; 
								// AE_Effect.h checks for this.

#include "AEConfig.h"

#ifdef AE_OS_WIN
	typedef unsigned short PixelType;
	#include <Windows.h>
#endif

#include "entry.h"
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AE_Macros.h"
#include "Param_Utils.h"
#include "AE_EffectCBSuites.h"
#include "String_Utils.h"
#include "AE_GeneralPlug.h"
#include "AEFX_ChannelDepthTpl.h"
#include "AEGP_SuiteHandler.h"
#include "Smart_Utils.h"

/* Versioning information */

#define	MAJOR_VERSION	1
#define	MINOR_VERSION	1
#define	BUG_VERSION		0
#define	STAGE_VERSION	PF_Stage_DEVELOP
#define	BUILD_VERSION	1


/* Parameter defaults */

#define	SKELETON_GAIN_MIN		0
#define	SKELETON_GAIN_MAX		100
#define	SKELETON_GAIN_DFLT		10

#include <vector>

#include "Plugin.h"
#include "Block.h"

class TIBlock : public Plugin {

public:

	typedef class ParamData {
	public:
		
		// paramaters
		PF_Pixel color;
		A_long block_only;
		A_long hole_exclude;
		
		PF_FpLong x_exr, y_exr;
		double rand_exr;
		long seed_exr;

		PF_FpLong x_off, y_off;
		double rand_off;
		long seed_off;

		PF_Pixel color2;
		double rand_col;
		long seed_col;

		double begin_xcl, end_xcl;
		double rand_xcl;
		long seed_xcl;

		// text info
		std::vector<Block> blocks;

	public:

		void UpdateParam(PF_InData* in_data, PF_ParamDef** params);
		void UpdateBlock(PF_InData* in_data, PF_ParamDef** params);

	} ParamData, * ParamDataP, ** ParamDataH;

	ParamData render_param{};

public:

	TIBlock(std::string name, std::string descr) : Plugin(name, descr) {};
	TIBlock() {};
	void Reset();

};


extern "C" {

	DllExport
	PF_Err
	EffectMain(
		PF_Cmd			cmd,
		PF_InData		*in_data,
		PF_OutData		*out_data,
		PF_ParamDef		*params[],
		PF_LayerDef		*output,
		void			*extra);

}

#endif