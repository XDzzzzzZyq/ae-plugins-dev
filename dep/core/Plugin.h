#pragma once

#include "Parameters.h"

#define PushParam(name, id) Plugin::plug_params.plug_param_list[name] = {id, NULL}

class Plugin
{

public:

	Plugin();
	Plugin(std::string name, std::string descr);

public:

	static std::string plug_name;
	static std::string plug_descr;

	static char* GetPlugName();
	static char* GetPlugDescription();

public:

	static Parameters plug_params;

};

