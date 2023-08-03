#include "Parameters.h"

std::string Parameters::plug_name = "Quad-Tree Mosaic";

std::string Parameters::plug_descr = "An Quad-Tree based Mosaic postprocessing famity effects. \rCopyright XDzZyq";

char* Parameters::GetPlugName()
{
	return (char*)Parameters::plug_name.c_str();
}

char* Parameters::GetPlugDescription()
{
	return (char*)Parameters::plug_descr.c_str();
}

std::unordered_map<std::string, int> Parameters::plug_param_list;

int32_t Parameters::GetParamID(const std::string& name)
{
	return Parameters::plug_param_list[name];
}

int32_t Parameters::GetParamNum()
{
	return (int32_t)Parameters::plug_param_list.size();
}

#define PushParam(name, id) Parameters::plug_param_list[name] = id;
void Parameters::InitParameters()
{
	PushParam("None",	0)
	PushParam("Gain",	1);
	PushParam("Color",	2);
	PushParam("Count",	3);
	PushParam("Select", 4);
}
