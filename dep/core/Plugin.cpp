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

std::string Plugin::GetPlugName()
{
	return Plugin::plug_name;
}

std::string Plugin::GetPlugDescription()
{
	return Plugin::plug_descr;
}

Parameters Plugin::plug_params;
