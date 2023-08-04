#include "Parameters.h"

std::unordered_map<std::string, Parameters::ParaData> Parameters::plug_param_list;

int32_t Parameters::GetParamID(const std::string& name)
{
	return Parameters::plug_param_list[name].first;
}

int32_t Parameters::GetParamNum()
{
	return (int32_t)Parameters::plug_param_list.size();
}
