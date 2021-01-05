#pragma once

#include <exception>
#include <unordered_map>
#include <regex>
#include <string>

#include "Json2Settings.h"


template <class T> using aSetting = Json2Settings::aSetting<T>;


namespace Json2Settings
{
	template<>
	class aSetting<RE::TESForm*> : public ISetting
	{
	public:
		using key_type = RE::FormID;
		using mapped_type = RE::TESForm*;
		using container_type = std::unordered_map<key_type, mapped_type>;
		using value_type = container_type::value_type;

		aSetting() = delete;

		aSetting(std::string a_key, std::initializer_list<value_type> a_init = {}) :
			ISetting(std::move(a_key)),
			_container(std::move(a_init))
		{}

		virtual ~aSetting() = default;

		[[nodiscard]] container_type& operator*()
		{
			return _container;
		}

		[[nodiscard]] const container_type& operator*() const
		{
			return _container;
		}

		[[nodiscard]] container_type* operator->()
		{
			return std::addressof(_container);
		}

		[[nodiscard]] const container_type* operator->() const
		{
			return std::addressof(_container);
		}

	protected:
		// ignore exceptions, let the loader catch them so we can get the guilty file name
		virtual void assign_impl(const json& a_val) override
		{
			auto dataHandler = RE::TESDataHandler::GetSingleton();
			std::string pluginName;
			std::string formIDStr;
			RE::FormID formIDNum;
			RE::TESForm* form = 0;
			for (auto& val : a_val) {
				pluginName = val.at("pluginName").get<std::string>();
				formIDStr = val.at("formID").get<std::string>();
				formIDNum = std::stoi(formIDStr, 0, 16);
				form = dataHandler->LookupForm(formIDNum, pluginName);
				if (form) {
					_container.insert(std::make_pair(form->formID, form));
				}
			}
		}

		[[nodiscard]] virtual string_t dump_impl() const override
		{
			auto dmp = key();
			dmp += ":";
			for (auto& elem : _container) {
				//dmp += SKSE::VArgFormatter("\n\t[0x%08X]", elem.first).str();
			}
			return dmp;
		}

		[[nodiscard]] virtual string_t to_string_impl() const override
		{
			return "<INVALID>";
		}

	private:
		container_type _container;
	};
}


class Settings
{
public:
	Settings() = delete;

	static bool LoadSettings(bool a_dumpParse = false);


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
	static constexpr char FILE_PATH[] = "Data\\SKSE\\Plugins\\";
	static constexpr wchar_t REGEX_PATTERN[] = L"iEquip_.*\\.json";
	static constexpr auto REGEX_FLAGS = std::regex_constants::grep | std::regex_constants::icase;
};
