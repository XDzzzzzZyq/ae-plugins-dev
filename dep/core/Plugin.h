#pragma once

#include "Parameters.h"

#define PushParam(name) Plugin::plug_params.plug_param_list[name] = {(int32_t)Plugin::plug_params.plug_param_list.size(), NULL}

class Plugin
{

public:

	Plugin();
	Plugin(std::string name, std::string descr);

public:

	static std::string plug_name;
	static std::string plug_descr;

	static std::string GetPlugName();
	static std::string GetPlugDescription();

public:

	static Parameters plug_params;

};

