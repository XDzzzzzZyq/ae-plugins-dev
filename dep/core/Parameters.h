#pragma once

#include <iostream>
#include <unordered_map>
#include <any>

class Parameters
{
public:

	using ParaData = std::pair<int32_t, std::any>;

	static std::unordered_map<std::string, ParaData> plug_param_list;
	static int32_t GetParamID(const std::string& name);
	static int32_t GetParamNum();

	template<typename T>
	static T GetParamData(const std::string& name);
	template<typename T>
	static void SetParamData(const std::string& name, const T& data);

};

template<typename T>
T Parameters::GetParamData(const std::string& name)
{
	return std::any_cast<T>(plug_param_list[name].second);
}

template<typename T>
void Parameters::SetParamData(const std::string& name, const T& data)
{
	plug_param_list[name].second = data;
}

