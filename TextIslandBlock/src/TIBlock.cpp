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

/*	Skeleton.cpp

	This is a compiling husk of a project. Fill it in with interesting
	pixel processing code.

	Revision History

	Version		Change													Engineer	Date
	=======		======													========	======
	1.0			(seemed like a good idea at the time)					bbb			6/1/2002

	1.0			Okay, I'm leaving the version at 1.0,					bbb			2/15/2006
				for obvious reasons; you're going to
				copy these files directly! This is the
				first XCode version, though.

	1.0			Let's simplify this barebones sample					zal			11/11/2010

	1.0			Added new entry point									zal			9/18/2017
	1.1			Added 'Support URL' to PiPL and entry point				cjr			3/31/2023

*/

#include "TIBlock.h"

#define BindInput(dataname, paramname, type, ae_type) dataname = params[Parameters::GetParamID(paramname)]->u.ae_type; \
Parameters::SetParamData<type>(paramname, dataname);

void TIBlock::ParamData::UpdateParam(PF_InData* in_data, PF_ParamDef** params)
{
	AEFX_CLR_STRUCT(TIBlock::render_param);
	BindInput(color, "Block Color", PF_Pixel, cd.value);
	BindInput(block_only, "Block Only", A_long, bd.value);
}

void TIBlock::ParamData::UpdateBlock(PF_InData* in_data, PF_ParamDef** params)
{
	AEGP_SuiteHandler suites(in_data->pica_basicP);

	suites.PFInterfaceSuite1()->AEGP_GetEffectLayer(
		in_data->effect_ref,
		&TIBlock::layerPH);

	suites.TextLayerSuite1()->AEGP_GetNewTextOutlines(
		TIBlock::layerPH,
		(const A_Time*)&in_data->time_step,
		&TIBlock::outlinesPH);

	A_long count{};
	suites.TextLayerSuite1()->AEGP_GetNumTextOutlines(
		TIBlock::outlinesPH,
		&count
	);

	TIBlock::render_param.blocks.resize(count);

	for (A_long i = 0; i < count; i++) {
		PF_PathOutlinePtr pathPP{};
		suites.TextLayerSuite1()->AEGP_GetIndexedTextOutline(
			TIBlock::outlinesPH,
			i,
			&pathPP);

		A_long nums{};
		suites.PathDataSuite1()->PF_PathNumSegments(
			in_data->effect_ref,
			pathPP,
			&nums);

		for (A_long j = 0; j < nums; j++) {
			PF_PathVertex vert{};
			suites.PathDataSuite1()->PF_PathVertexInfo(
				in_data->effect_ref,
				pathPP,
				j,
				&vert);

			const glm::vec2 b_vert{ vert.x, vert.y };
			TIBlock::render_param.blocks.at(i).UpdateMin(b_vert);
			TIBlock::render_param.blocks.at(i).UpdateMax(b_vert);

			//std::cout << vert.x << ", " << vert.y << "\n";
		}

		//std::cout << TIBlock::render_param.blocks.at(i) << "\n";
	}
	//std::cout << "end\n";
}

TIBlock::ParamData TIBlock::render_param;

AEGP_LayerH TIBlock::layerPH = nullptr;

AEGP_TextOutlinesH TIBlock::outlinesPH = nullptr;

static PF_Err
About(
	PF_InData* in_data,
	PF_OutData* out_data,
	PF_ParamDef* params[],
	PF_LayerDef* output)
{
	AEGP_SuiteHandler suites(in_data->pica_basicP);

	static TIBlock plugin(
		"Text Island Block",
		"Quick blocks generator for text islands. \rCopyright XDzZyq"
	);

	suites.ANSICallbacksSuite1()->sprintf(out_data->return_msg,
		"%s v%d.%d\n\r%s",
		TIBlock::GetPlugName().c_str(),
		MAJOR_VERSION,
		MINOR_VERSION,
		TIBlock::GetPlugDescription().c_str());

	return PF_Err_NONE;
}

static PF_Err
GlobalSetup(
	PF_InData* in_data,
	PF_OutData* out_data,
	PF_ParamDef* params[],
	PF_LayerDef* output)
{
	out_data->my_version = PF_VERSION(MAJOR_VERSION,
		MINOR_VERSION,
		BUG_VERSION,
		STAGE_VERSION,
		BUILD_VERSION);

	out_data->out_flags = PF_OutFlag_DEEP_COLOR_AWARE | PF_OutFlag_I_DO_DIALOG;	// just 16bpc, not 32bpc
	out_data->out_flags2 = PF_OutFlag2_SUPPORTS_THREADED_RENDERING;

	PushParam("None");
	PushParam("Block Color");
	PushParam("Block Only");

	return PF_Err_NONE;
}

static PF_Err
ParamsSetup(
	PF_InData* in_data,
	PF_OutData* out_data,
	PF_ParamDef* params[],
	PF_LayerDef* output)
{
	PF_Err		err = PF_Err_NONE;
	PF_ParamDef	def;

	AEFX_CLR_STRUCT(def);

	PF_ADD_COLOR("Color",
		PF_HALF_CHAN8,
		PF_MAX_CHAN8,
		PF_MAX_CHAN8,
		Parameters::GetParamID("Block Color"));

	AEFX_CLR_STRUCT(def);

	PF_ADD_CHECKBOX("Block Only",
		"only display blocks",
		false,
		PF_ParamFlag_SUPERVISE,
		Parameters::GetParamID("Block Only"));

	out_data->num_params = Parameters::GetParamNum();

	return err;
}

static PF_Err
Copy8(
	void* refcon,
	A_long		xL,
	A_long		yL,
	PF_Pixel8* inP,
	PF_Pixel8* outP)
{
	PF_Err		err = PF_Err_NONE;

	outP->alpha = inP->alpha;
	outP->red	= inP->red;
	outP->green = inP->green;
	outP->blue	= inP->blue;

	return err;
}

static PF_Err
BlockFill(
	void* refcon,
	A_long		xL,
	A_long		yL,
	PF_Pixel8* inP,
	PF_Pixel8* outP)
{
	PF_Err		err = PF_Err_NONE;

	const double frac = (double)inP->alpha / (double)255;

	if (TIBlock::render_param.block_only == 0) {

		outP->alpha = 255;
		outP->red = (A_u_char)(TIBlock::render_param.color.red * (1.0 - frac) + inP->red * frac);
		outP->green = (A_u_char)(TIBlock::render_param.color.green * (1.0 - frac) + inP->green * frac);
		outP->blue = (A_u_char)(TIBlock::render_param.color.blue * (1.0 - frac) + inP->blue * frac);
	
	}
	else {
	
		outP->alpha = 255;
		outP->red = TIBlock::render_param.color.red;
		outP->green = TIBlock::render_param.color.green;
		outP->blue = TIBlock::render_param.color.blue;
	
	}


	return err;
}

static PF_Err  // int32_t
Render(
	PF_InData* in_data,
	PF_OutData* out_data,
	PF_ParamDef* params[],
	PF_LayerDef* output)
{
	PF_Err				err = PF_Err_NONE;
	AEGP_SuiteHandler	suites(in_data->pica_basicP);

	/*	Put interesting code here. */

	A_long				linesL = 0;

	linesL = output->extent_hint.bottom - output->extent_hint.top;

	// copy

	if (TIBlock::render_param.block_only == 0) {

		ERR(suites.Iterate8Suite2()->iterate(
			
			in_data,
			0,								// progress base
			linesL,							// progress final
			&params[0]->u.ld,				// src 
			NULL,							// area - null for all pixels
			nullptr,						// refcon - your custom data pointer
			Copy8,							// pixel function pointer
			output));

	}

	// fill the blocks

	for (auto& block : TIBlock::render_param.blocks) {
	
		const PF_Rect area{ (A_long)block.b_min.x, (A_long)block.b_min.y, (A_long)block.b_max.x + 1, (A_long)block.b_max.y + 1 };

		ERR(suites.Iterate8Suite2()->iterate(
			
			in_data,
			0,								// progress base
			linesL,							// progress final
			output,							// src 
			&area,							// area - null for all pixels
			nullptr,						// refcon - your custom data pointer
			BlockFill,						// pixel function pointer
			output));

	}

	return err;
}


extern "C" DllExport
PF_Err PluginDataEntryFunction2(
	PF_PluginDataPtr inPtr,
	PF_PluginDataCB2 inPluginDataCallBackPtr,
	SPBasicSuite * inSPBasicSuitePtr,
	const char* inHostName,
	const char* inHostVersion)
{
	PF_Err result = PF_Err_INVALID_CALLBACK;

	result = PF_REGISTER_EFFECT_EXT2(
		inPtr,
		inPluginDataCallBackPtr,
		"Skeleton", // Name
		"ADBE Skeleton", // Match Name
		"Sample Plug-ins", // Category
		AE_RESERVED_INFO, // Reserved Info
		"EffectMain",	// Entry point
		"https://www.adobe.com");	// support URL

	return result;
}


PF_Err
EffectMain(
	PF_Cmd			cmd,
	PF_InData* in_data,
	PF_OutData* out_data,
	PF_ParamDef* params[],
	PF_LayerDef* output,
	void* extra)
{
	PF_Err		err = PF_Err_NONE;

	try {
		switch (cmd) {
		case PF_Cmd_ABOUT:

			err = About(in_data,
				out_data,
				params,
				output);
			break;

		case PF_Cmd_DO_DIALOG:

			err = About(in_data,
				out_data,
				params,
				output);
			break;

		case PF_Cmd_GLOBAL_SETUP:

			err = GlobalSetup(in_data,
				out_data,
				params,
				output);
			break;

		case PF_Cmd_PARAMS_SETUP:

			err = ParamsSetup(in_data,
				out_data,
				params,
				output);
			break;

		case PF_Cmd_RENDER:

			TIBlock::render_param.UpdateParam(in_data, params);
			TIBlock::render_param.UpdateBlock(in_data, params);

			err = Render(in_data,
				out_data,
				params,
				output);
			break;

		case PF_Cmd_USER_CHANGED_PARAM:
			break;
		}
	}
	catch (PF_Err& thrown_err) {
		err = thrown_err;
	}
	return err;
}

