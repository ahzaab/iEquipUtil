#pragma once

#include "GameForms.h"  // TESForm

#include <map>  // map
#include <string>  // string, stoi

#include "Forms.h"  // GetForm
#include "Json2Settings.h"  // Json2Settings


template<>
class aSetting<TESForm*> :
	public ISetting,
	public std::map<UInt32, TESForm*>
{
private:
	using Container = std::map<UInt32, TESForm*>;

public:
	aSetting() = delete;

	aSetting(std::string a_key, bool a_consoleOK, std::initializer_list<Container::value_type> a_list = {}) :
		ISetting(a_key, a_consoleOK),
		Container(a_list)
	{}

	virtual ~aSetting()
	{}

	virtual void assign(json& a_val) override
	{
		clear();
		std::string pluginName;
		std::string formIDStr;
		UInt32 formIDNum;
		TESForm* form = 0;
		for (auto& val : a_val) {
			val.at("pluginName").get_to(pluginName);
			val.at("formID").get_to(formIDStr);
			formIDNum = std::stoi(formIDStr, 0, 16);
			form = GetForm(formIDNum, pluginName.c_str());
			if (form) {
				Container::insert({ form->formID, form });
			}
		}
	}

	virtual void dump() override
	{
		_DMESSAGE("== %s ==", _key.c_str());
		for (auto& elem : *this) {
			_DMESSAGE("[0x%08X]", elem.first);
		}
	}

	virtual std::string	getValueAsString() const override
	{
		return "";
	}
};


class Settings : public Json2Settings::Settings
{
public:
	Settings() = delete;
	static bool loadSettings(bool a_dumpParse = false);


	static aSetting<TESForm*> spears;
	static aSetting<TESForm*> javelins;
	static aSetting<TESForm*> grenades;
	static aSetting<TESForm*> throwingAxes;
	static aSetting<TESForm*> throwingKnives;
	static aSetting<TESForm*> waxes;
	static aSetting<TESForm*> oils;
	static aSetting<TESForm*> spellWards;
	static aSetting<TESForm*> fire;
	static aSetting<TESForm*> ice;
	static aSetting<TESForm*> shock;
	static aSetting<TESForm*> poison;
	static aSetting<TESForm*> salves;
	static aSetting<TESForm*> bandages;

private:
	static constexpr char FILE_PREFIX[] = "Data\\SKSE\\Plugins\\";
	static constexpr char FILE_PATTERN[] = "iEquip_*.json";
};
