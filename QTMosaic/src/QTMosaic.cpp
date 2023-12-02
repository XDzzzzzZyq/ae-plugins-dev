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

#include "QTMosaic.h"

#define BindInput(dataname, paramname, type, ae_type) dataname = params[Parameters::GetParamID(paramname)]->u.ae_type; \
Parameters::SetParamData<type>(paramname, dataname);

void QTMosaic::ParamData::Update(PF_ParamDef** params)
{
	BindInput(gainF, "Gain", PF_FpLong, fs_d.value);
	BindInput(color, "Color", PF_Pixel, cd.value);
	BindInput(layer, "Layer", PF_LayerDef, ld);
}

static PF_Err 
About (	
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	AEGP_SuiteHandler suites(in_data->pica_basicP);

	static QTMosaic plugin(
		"Quad-Tree Mosaic",
		"An Quad-Tree based Mosaic postprocessing famity effects. \rCopyright XDzZyq"
	);
	
	suites.ANSICallbacksSuite1()->sprintf(	out_data->return_msg,
											"%s v%d.%d\n\r%s",
											QTMosaic::GetPlugName().c_str(),
											MAJOR_VERSION, 
											MINOR_VERSION, 
											QTMosaic::GetPlugDescription().c_str());

	return PF_Err_NONE;
}

static PF_Err 
GlobalSetup (	
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	out_data->my_version = PF_VERSION(	MAJOR_VERSION, 
										MINOR_VERSION,
										BUG_VERSION, 
										STAGE_VERSION, 
										BUILD_VERSION);

	out_data->out_flags =  PF_OutFlag_DEEP_COLOR_AWARE | PF_OutFlag_I_DO_DIALOG;	// just 16bpc, not 32bpc
	out_data->out_flags2 = PF_OutFlag2_SUPPORTS_THREADED_RENDERING;
	
	PushParam("None");
	PushParam("Gain");
	PushParam("Color");
	PushParam("Count");
	PushParam("Select");
	PushParam("Layer");
	
	return PF_Err_NONE;
}

static PF_Err 
ParamsSetup (	
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	PF_Err		err		= PF_Err_NONE;
	PF_ParamDef	def;	

	AEFX_CLR_STRUCT(def);

	PF_ADD_FLOAT_SLIDERX(	"Gain",
							SKELETON_GAIN_MIN, 
							SKELETON_GAIN_MAX, 
							SKELETON_GAIN_MIN, 
							SKELETON_GAIN_MAX, 
							SKELETON_GAIN_DFLT,
							PF_Precision_HUNDREDTHS,
							0,
							PF_ParamFlag_SUPERVISE,
							Parameters::GetParamID("Gain"));

	AEFX_CLR_STRUCT(def);
	
	PF_ADD_COLOR(	"Color",
					PF_HALF_CHAN8,
					PF_MAX_CHAN8,
					PF_MAX_CHAN8,
					Parameters::GetParamID("Color"));

	AEFX_CLR_STRUCT(def);

	PF_ADD_FLOAT_SLIDERX(	"Count",
							SKELETON_GAIN_MIN,
							SKELETON_GAIN_MAX,
							SKELETON_GAIN_MIN,
							SKELETON_GAIN_MAX,
							SKELETON_GAIN_DFLT,
							PF_Precision_HUNDREDTHS,
							0,
							PF_ParamFlag_SUPERVISE,
							Parameters::GetParamID("Count"));

	AEFX_CLR_STRUCT(def);

	PF_ADD_CHECKBOX(	"Select",
						"text",
						false,
						PF_ParamFlag_SUPERVISE,
						Parameters::GetParamID("Select"));

	AEFX_CLR_STRUCT(def);

	PF_ADD_LAYER("Layer", 1, Parameters::GetParamID("Layer"));
	
	out_data->num_params = Parameters::GetParamNum();

	return err;
}

static PF_Err
MySimpleGainFunc16 (
	void		*refcon, 
	A_long		xL, 
	A_long		yL, 
	PF_Pixel16	*inP, 
	PF_Pixel16	*outP)
{
	PF_Err		err = PF_Err_NONE;

	if (refcon == nullptr) return err;

	outP->alpha		=	inP->alpha;
	outP->red		=	inP->red	;
	outP->green		=	inP->green	;
	outP->blue		=	inP->blue	;

	return err;
}

PF_Pixel* sampleIntegral32(PF_EffectWorld& def, int x, int y) {
	return (PF_Pixel*)((char*)def.data +
		(y * def.rowbytes) +
		(x * sizeof(PF_Pixel)));
}

static PF_Err
MySimpleGainFunc8 (
	void		*refcon, 
	A_long		xL, 
	A_long		yL, 
	PF_Pixel8	*inP, 
	PF_Pixel8	*outP)
{
	PF_Err		err = PF_Err_NONE;

	if (refcon == nullptr) return err;

	QTMosaic::ParamData* param = (QTMosaic::ParamData*)refcon;

	PF_Pixel* tar = sampleIntegral32(param->layer, xL, yL);


	outP->alpha		=	inP->alpha;
	outP->red = param->color.red;
	outP->green		=	inP->green;
	outP->blue		=	inP->blue;

	if (param->layer.data == nullptr) {
		outP->red = inP->red;
		//outP->red		=	param->color.red;
	}
	else {
		outP->red = tar->red;
	}

	return err;
}

static PF_Err  // int32_t
Render (
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output,
	QTMosaic*		plugin)
{
	PF_Err				err		= PF_Err_NONE;
	AEGP_SuiteHandler	suites(in_data->pica_basicP);

	/*	Put interesting code here. */

	A_long				linesL	= 0;
	PF_ParamDef			sourceWorld{};

	linesL = output->extent_hint.bottom - output->extent_hint.top;

	PF_CHECKOUT_PARAM(
		in_data,
		Parameters::GetParamID("Layer"), 
		in_data->current_time,
		in_data->time_step,
		in_data->time_scale, 
		&sourceWorld
	);

	plugin->render_param.layer = sourceWorld.u.ld;

	std::cout << &sourceWorld.u.ld << std::endl;

	if (PF_WORLD_IS_DEEP(output)){
		ERR(suites.Iterate16Suite2()->iterate(	in_data,
												0,								// progress base
												linesL,							// progress final
												&params[0]->u.ld,				// src 
												NULL,							// area - null for all pixels
												(void*)plugin,	// refcon - your custom data pointer
												MySimpleGainFunc16,				// pixel function pointer
												output));
	} else {
		ERR(suites.Iterate8Suite2()->iterate(	in_data,
												0,								// progress base
												linesL,							// progress final
												&params[0]->u.ld,				// src 
												NULL,							// area - null for all pixels
												(void*)plugin,	// refcon - your custom data pointer
												MySimpleGainFunc8,				// pixel function pointer
												output));	
	}

	ERR(PF_CHECKIN_PARAM(in_data, &sourceWorld));
	AEFX_CLR_STRUCT(sourceWorld);

	return err;
}


extern "C" DllExport
PF_Err PluginDataEntryFunction2(
	PF_PluginDataPtr inPtr,
	PF_PluginDataCB2 inPluginDataCallBackPtr,
	SPBasicSuite* inSPBasicSuitePtr,
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
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output,
	void			*extra)
{
	PF_Err		err = PF_Err_NONE;

	QTMosaic plugin{};
	
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

				err = GlobalSetup(	in_data,
									out_data,
									params,
									output);
				break;
				
			case PF_Cmd_PARAMS_SETUP:

				err = ParamsSetup(	in_data,
									out_data,
									params,
									output);
				break;
				
			case PF_Cmd_RENDER:

				AEFX_CLR_STRUCT(plugin.render_param);
				plugin.render_param.Update(params);

				err = Render(	in_data,
								out_data,
								params,
								output,
								&plugin);
				break;

			case PF_Cmd_USER_CHANGED_PARAM:
				break;
		}
	}
	catch(PF_Err &thrown_err){
		err = thrown_err;
	}
	return err;
}

