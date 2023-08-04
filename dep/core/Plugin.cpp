#include "Plugin.h"


Plugin::Plugin()
	:Plugin("Test", "Plugin Test.")
{}

Plugin::Plugin(std::string name, std::string descr)
{
	Plugin::plug_name = name;
	Plugin::plug_descr = descr;
}

std::string Plugin::plug_name;
std::string Plugin::plug_descr;

char* Plugin::GetPlugName()
{
	return (char*)Plugin::plug_name.c_str();
}

char* Plugin::GetPlugDescription()
{
	return (char*)Plugin::plug_descr.c_str();
}

Parameters Plugin::plug_params;
