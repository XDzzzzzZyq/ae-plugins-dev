#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>

class Parameters
{
public:

	static std::string plug_name;
	static std::string plug_descr;

	static char* GetPlugName();
	static char* GetPlugDescription();

public:

	static std::unordered_map<std::string, int32_t> plug_param_list;
	static int32_t GetParamID(const std::string& name);
	static int32_t GetParamNum();

public:

	static void InitParameters();

};

