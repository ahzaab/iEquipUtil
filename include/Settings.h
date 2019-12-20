#pragma once

#include <map>
#include <string>

#include "RE/Skyrim.h"

#include "Json2Settings.h"


template<>
class aSetting<RE::TESForm*> :
	public ISetting,
	public std::map<UInt32, RE::TESForm*>
{
private:
	using Container = std::map<UInt32, RE::TESForm*>;

public:
	aSetting() = delete;

	aSetting(std::string a_key, std::initializer_list<Container::value_type> a_list = {}, bool a_consoleOK = false) :
		ISetting(a_key, a_consoleOK),
		Container(a_list)
	{}

	virtual ~aSetting() = default;

	virtual void assign(json& a_val) override
	{
		clear();
		auto dataHandler = RE::TESDataHandler::GetSingleton();
		std::string pluginName;
		std::string formIDStr;
		UInt32 formIDNum;
		RE::TESForm* form = 0;
		for (auto& val : a_val) {
			val.at("pluginName").get_to(pluginName);
			val.at("formID").get_to(formIDStr);
			formIDNum = std::stoi(formIDStr, 0, 16);
			form = dataHandler->LookupForm(formIDNum, pluginName);
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


	static aSetting<RE::TESForm*> bandages;
	static aSetting<RE::TESForm*> fire;
	static aSetting<RE::TESForm*> grenades;
	static aSetting<RE::TESForm*> ice;
	static aSetting<RE::TESForm*> javelins;
	static aSetting<RE::TESForm*> oils;
	static aSetting<RE::TESForm*> poison;
	static aSetting<RE::TESForm*> rangedSpells;
	static aSetting<RE::TESForm*> rangedStaves;
	static aSetting<RE::TESForm*> salves;
	static aSetting<RE::TESForm*> shock;
	static aSetting<RE::TESForm*> spears;
	static aSetting<RE::TESForm*> spellWards;
	static aSetting<RE::TESForm*> throwingAxes;
	static aSetting<RE::TESForm*> throwingKnives;
	static aSetting<RE::TESForm*> waxes;

private:
	static constexpr char FILE_PREFIX[] = "Data\\SKSE\\Plugins\\";
	static constexpr char FILE_PATTERN[] = "iEquip_*.json";
};
