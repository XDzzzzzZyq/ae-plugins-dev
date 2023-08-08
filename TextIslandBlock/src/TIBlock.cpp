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

void TIBlock::Reset()
{
	AEFX_CLR_STRUCT(TIBlock::render_param);
	//TIBlock::render_param.blocks.clear();
}

#define BindInput(dataname, paramname, type, ae_type) this->dataname = (type)params[Parameters::GetParamID(paramname)]->u.ae_type; \
Parameters::SetParamData<type>(paramname, dataname);

void TIBlock::ParamData::UpdateParam(PF_InData* in_data, PF_ParamDef** params)
{

	BindInput(color, "Block Color", PF_Pixel, cd.value);
	BindInput(block_only, "Block Only", A_long, bd.value);
	BindInput(hole_exclude, "Exclude Inv", A_long, bd.value);

	BindInput(x_exr, "x_exr", PF_FpLong, fs_d.value);
	BindInput(y_exr, "y_exr", PF_FpLong, fs_d.value);
	BindInput(rand_exr, "randomize_exr", double, fd.value/65536.0);
	BindInput(seed_exr, "seed_exr", long, sd.value);

	BindInput(x_off, "x_off", PF_FpLong, fs_d.value);
	BindInput(y_off, "y_off", PF_FpLong, fs_d.value);
	BindInput(rand_off, "randomize_off", double, fd.value / 65536.0);
	BindInput(seed_off, "seed_off", long, sd.value);

	BindInput(color2, "color2", PF_Pixel, cd.value);
	BindInput(rand_col, "randomize_col", double, fd.value / 65536.0);
	BindInput(seed_col, "seed_col", long, sd.value);

	BindInput(begin_xcl, "begin_xcl", double, fd.value / 65536.0);
	BindInput(end_xcl, "end_xcl", double, fd.value / 65536.0);
	BindInput(rand_xcl, "randomize_xcl", double, fd.value / 65536.0);
	BindInput(seed_xcl, "seed_xcl", long, sd.value);
	
}

void TIBlock::ParamData::UpdateBlock(PF_InData* in_data, PF_ParamDef** params)
{
	AEGP_SuiteHandler suites(in_data->pica_basicP);

	AEGP_LayerH layerPH{};
	AEGP_TextOutlinesH outlinesPH{};

	suites.PFInterfaceSuite1()->AEGP_GetEffectLayer(
		in_data->effect_ref,
		&layerPH);

	A_Time time = { in_data->current_time , in_data->time_scale };
	suites.TextLayerSuite1()->AEGP_GetNewTextOutlines(
		layerPH,
		&time,
		&outlinesPH);

	A_long count{};
	suites.TextLayerSuite1()->AEGP_GetNumTextOutlines(
		outlinesPH,
		&count
	);

	this->blocks.resize(count);

	for (A_long i = 0; i < count; i++) {

		this->blocks.at(i).Reset();

		PF_PathOutlinePtr pathPP{};
		suites.TextLayerSuite1()->AEGP_GetIndexedTextOutline(
			outlinesPH,
			i,
			&pathPP);

		A_long nums{};
		suites.PathDataSuite1()->PF_PathNumSegments(
			in_data->effect_ref,
			pathPP,
			&nums);

		std::vector<glm::vec2> verts(nums);
		for (A_long j = 0; j < nums; j++) {

			PF_PathVertex vert{};
			suites.PathDataSuite1()->PF_PathVertexInfo(
				in_data->effect_ref,
				pathPP,
				j,
				&vert);

			const glm::vec2 b_vert{ vert.x, vert.y };

			this->blocks.at(i).UpdateMin(b_vert);
			this->blocks.at(i).UpdateMax(b_vert);

			verts.at(j) = b_vert;
			//std::cout << vert.x << ", " << vert.y << "\n";
		}
		
		if(this->hole_exclude)
			this->blocks.at(i).b_invert = is_invert(verts);
		//std::cout << TIBlock::render_param.blocks.at(i) << "\n";
	}
	//std::cout << "end\n";
}

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
	PushParam("Exclude Inv");

	PushTopic("Extrude",
		PushParam("x_exr");
		PushParam("y_exr");
		PushParam("randomize_exr");
		PushParam("seed_exr");
	);

	PushTopic("Offset",
		PushParam("x_off");
		PushParam("y_off");
		PushParam("randomize_off");
		PushParam("seed_off");
	);

	PushTopic("ReColor",
		PushParam("color2");
		PushParam("randomize_col");
		PushParam("seed_col");
	);

	PushTopic("Exclude",
		PushParam("begin_xcl");
		PushParam("end_xcl");
		PushParam("randomize_xcl");
		PushParam("seed_xcl");
	);
	std::cout << "init\n";
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


	std::cout << "param_setup\n";


	AEFX_CLR_STRUCT(def);

	PF_ADD_COLOR("main color",
		PF_HALF_CHAN8,
		PF_MAX_CHAN8,
		PF_MAX_CHAN8,
		Parameters::GetParamID("Block Color"));


	AEFX_CLR_STRUCT(def);

	PF_ADD_CHECKBOX("block only",
		"only display blocks",
		false,
		0,
		Parameters::GetParamID("Exclude Inv"));


	AEFX_CLR_STRUCT(def);

	PF_ADD_CHECKBOX("inner exclude",
		"exclude inner holes",
		true,
		0,
		Parameters::GetParamID("Exclude Inv"));


	AEFX_CLR_STRUCT(def);

	PF_ADD_TOPIC("Extrude", Parameters::GetParamID("Extrude"));

	AEFX_CLR_STRUCT(def);

	PF_ADD_FLOAT_SLIDERX("x", -1000, 1000, -100, 100, 0, PF_Precision_TENTHS, 0, 0, Parameters::GetParamID("x_exr"));
	PF_ADD_FLOAT_SLIDERX("y", -1000, 1000, -100, 100, 0, PF_Precision_TENTHS, 0, 0, Parameters::GetParamID("y_exr"));
	PF_ADD_PERCENT("randomize", 0.0, Parameters::GetParamID("randomize_exr"));
	PF_ADD_SLIDER("seed", -1000, 1000, -1000, 1000, 0, Parameters::GetParamID("seed_exr"));

	PF_END_TOPIC(Parameters::GetParamID("Extrude_End"));


	AEFX_CLR_STRUCT(def);

	PF_ADD_TOPIC("Offset", Parameters::GetParamID("Offset"));

	AEFX_CLR_STRUCT(def);

	PF_ADD_FLOAT_SLIDERX("x", -1000, 1000, -100, 100, 0, PF_Precision_TENTHS, 0, 0, Parameters::GetParamID("x_off"));
	PF_ADD_FLOAT_SLIDERX("y", -1000, 1000, -100, 100, 0, PF_Precision_TENTHS, 0, 0, Parameters::GetParamID("y_off"));
	PF_ADD_PERCENT("randomize", 0.0, Parameters::GetParamID("randomize_off"));
	PF_ADD_SLIDER("seed", -1000, 1000, -1000, 1000, 0, Parameters::GetParamID("seed_off"));

	PF_END_TOPIC(Parameters::GetParamID("Offset_End"));


	AEFX_CLR_STRUCT(def);

	PF_ADD_TOPIC("Random Colorize (future)", Parameters::GetParamID("ReColor"));

	AEFX_CLR_STRUCT(def);

	PF_ADD_COLOR("color2", PF_HALF_CHAN8, PF_MAX_CHAN8, PF_MAX_CHAN8, Parameters::GetParamID("color2"));
	PF_ADD_PERCENT("randomize", 0.0, Parameters::GetParamID("randomize_col"));
	PF_ADD_SLIDER("seed", -1000, 1000, -1000, 1000, 0, Parameters::GetParamID("seed_col"));

	PF_END_TOPIC(Parameters::GetParamID("ReColor_End"));


	AEFX_CLR_STRUCT(def);

	PF_ADD_TOPIC("Exclude", Parameters::GetParamID("Exclude"));

	AEFX_CLR_STRUCT(def);

	PF_ADD_PERCENT("begin", 0.0, Parameters::GetParamID("begin_xcl"));
	PF_ADD_PERCENT("end", 100, Parameters::GetParamID("end_xcl"));
	PF_ADD_PERCENT("randomize", 0.0, Parameters::GetParamID("randomize_xcl"));
	PF_ADD_SLIDER("seed", -1000, 1000, -1000, 1000, 0, Parameters::GetParamID("seed_xcl"));

	PF_END_TOPIC(Parameters::GetParamID("Exclude_End"));


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

	const double frac = (double)inP->alpha / (double)255;

	outP->alpha =	(A_u_char)(255*(1-(255-inP->alpha)/255.0 * (255 - outP->alpha) / 255.0));
	outP->red =		(A_u_char)(outP->red * (1.0 - frac) + inP->red * frac);
	outP->green =	(A_u_char)(outP->green * (1.0 - frac) + inP->green * frac);
	outP->blue =	(A_u_char)(outP->blue * (1.0 - frac) + inP->blue * frac);

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
	TIBlock*	plugin = (TIBlock*)refcon;

	outP->alpha = 255;
	outP->red = plugin->render_param.color.red;
	outP->green = plugin->render_param.color.green;
	outP->blue = plugin->render_param.color.blue;

	return err;
}

static PF_Err
Exclude8(
	void* refcon,
	A_long		xL,
	A_long		yL,
	PF_Pixel8* inP,
	PF_Pixel8* outP)
{
	PF_Err		err = PF_Err_NONE;

	outP->alpha = 0;

	return err;
}

PF_Rect CalcBlockArea(TIBlock* plugin, Block& block, int index) {
	glm::ivec2 center = glm::ivec2{ block.b_max.x + 1 + block.b_min.x , block.b_max.y + 1 + block.b_min.y } / 2;
	glm::ivec2 corner = glm::ivec2{ block.b_max.x + 1 - block.b_min.x , block.b_max.y + 1 - block.b_min.y } / 2;

	const double rand = random01(index);

	const double begin	= (double)plugin->render_param.begin_xcl / 100.;
	const double end	= (double)plugin->render_param.end_xcl   / 100.;
	const int max_index = (int)plugin->render_param.blocks.size() - 1;
	//std::cout << index << "\n";
	if (index < max_index * begin || index > max_index * end)
		return { 0, 0, 0, 0 };

	if(end == 0. || begin == 1.)
		return { 0, 0, 0, 0 };

	const double rand_xcl = (double)plugin->render_param.rand_xcl / 100.;
	if (rand_xcl != 0) {
		const auto seed_xcl = plugin->render_param.seed_xcl + 1.3;
		if(hash01(rand, rand * seed_xcl) < rand_xcl)
			return { 0, 0, 0, 0 };
	}

	const float rand_exr = (float)plugin->render_param.rand_exr / 100.f;
	glm::vec2 exr = { plugin->render_param.x_exr, plugin->render_param.y_exr };
	if (rand_exr != 0) {
		const auto seed_exr = plugin->render_param.seed_exr + 1.3;
		const glm::vec2 rand_off_exr = 2.0f * glm::vec2(hash01(rand, rand + seed_exr), hash01(rand, rand * seed_exr + seed_exr)) - 1.0f;

		exr = glm::mix(exr, rand_off_exr*exr, rand_exr);
	}
	corner += exr;

	const float rand_off = (float)plugin->render_param.rand_off / 100.f;
	glm::vec2 off = { plugin->render_param.x_off, plugin->render_param.y_off };
	if (rand_off != 0) {
		const auto seed_off = plugin->render_param.seed_off + 1.3;
		const glm::vec2 rand_off_off = 2.0f * glm::vec2(hash01(rand, rand * seed_off), hash01(rand, rand * seed_off + seed_off)) - 1.0f;

		off = glm::mix(off, rand_off_off * off, rand_off);
	}
	center += off;

	corner = glm::max(corner, 0);
	return { center.x - corner.x, center.y - corner.y , center.x + corner.x, center.y + corner.y };
}

static PF_Err  // int32_t
Render(
	PF_InData* in_data,
	PF_OutData* out_data,
	PF_ParamDef* params[],
	TIBlock*	plugin,
	PF_LayerDef* output)
{
	PF_Err				err = PF_Err_NONE;
	AEGP_SuiteHandler	suites(in_data->pica_basicP);

	/*	Put interesting code here. */

	A_long				linesL = 0;

	linesL = output->extent_hint.bottom - output->extent_hint.top;

	PF_ParamDef sourceWorld{};

	ERR(PF_CHECKOUT_PARAM(
		in_data,
		0,
		in_data->current_time,
		in_data->time_step,
		in_data->time_scale,
		&sourceWorld
	));

	// fill the blocks

	for (int i = 0; auto & block : plugin->render_param.blocks) {
	
		if (block.b_invert && plugin->render_param.hole_exclude) {
			i++;
			continue;
		}

		const PF_Rect area = CalcBlockArea(plugin, block, i++);

		if(area.right == 0 && area.bottom == 0) continue;

		ERR(suites.Iterate8Suite2()->iterate(
			
			in_data,
			0,								// progress base
			linesL,							// progress final
			output,							// src 
			&area,							// area - null for all pixels
			plugin,							// refcon - your custom data pointer
			BlockFill,						// pixel function pointer
			output));
	}

	// exclude the blocks

	for (int i = 0; auto & block : plugin->render_param.blocks) {

		if (!block.b_invert || !plugin->render_param.hole_exclude) {
			i++;
			continue;
		}

		const PF_Rect area = CalcBlockArea(plugin, block, i++);

		if (area.right == 0 && area.bottom == 0) continue;

		ERR(suites.Iterate8Suite2()->iterate(

			in_data,
			0,								// progress base
			linesL,							// progress final
			output,							// src 
			&area,							// area - null for all pixels
			nullptr,						// refcon - your custom data pointer
			Exclude8,						// pixel function pointer
			output));
	}

	// copy_text

	if (plugin->render_param.block_only == 0) {

		ERR(suites.Iterate8Suite2()->iterate(

			in_data,
			0,								// progress base
			linesL,							// progress final
			&sourceWorld.u.ld,				// src 
			NULL,							// area - null for all pixels
			nullptr,						// refcon - your custom data pointer
			Copy8,							// pixel function pointer
			output));

	}

	ERR(PF_CHECKIN_PARAM(in_data, &sourceWorld));
	AEFX_CLR_STRUCT(sourceWorld);

	// copy previous layer

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
	TIBlock plugin{};

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

			plugin.Reset();
			plugin.render_param.UpdateParam(in_data, params);
			plugin.render_param.UpdateBlock(in_data, params);

			err = Render(in_data,
				out_data,
				params,
				&plugin,
				output);
			break;

		case PF_Cmd_USER_CHANGED_PARAM:
			break;
		}
	}
	catch (PF_Err& thrown_err) {
		err = thrown_err;
	}

	plugin.Reset();
	return err;
}

