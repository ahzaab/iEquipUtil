#include "pch.h"
#include "Settings.h"

#include <cstring>
#include <filesystem>
#include <regex>
#include <string>


bool Settings::LoadSettings(bool a_dumpParse)
{
	std::filesystem::path dir(FILE_PATH);
	std::error_code ec;
	if (!std::filesystem::exists(dir, ec)) {
		_ERROR("\"%s\" does not exist!", FILE_PATH);
		return false;
	}

	std::wregex pattern(REGEX_PATTERN, REGEX_FLAGS);
	for (auto& entry : std::filesystem::directory_iterator(dir)) {
		if (std::regex_match(entry.path().filename().native(), pattern)) {
			auto [log, success] = Json2Settings::load_settings(entry.path(), a_dumpParse, true);
			if (!log.empty()) {
				_ERROR("%s", log.c_str());
			}
			if (!success) {
				return false;
			}
		}
	}

	return true;
}


decltype(Settings::bandages) Settings::bandages("bandages");
decltype(Settings::fire) Settings::fire("fire");
decltype(Settings::grenades) Settings::grenades("grenades");
decltype(Settings::ice) Settings::ice("ice");
decltype(Settings::javelins) Settings::javelins("javelins");
decltype(Settings::oils) Settings::oils("oils");
decltype(Settings::poison) Settings::poison("poison");
decltype(Settings::rangedSpells) Settings::rangedSpells("rangedSpells");
decltype(Settings::rangedStaves) Settings::rangedStaves("rangedStaves");
decltype(Settings::salves) Settings::salves("salves");
decltype(Settings::shock) Settings::shock("shock");
decltype(Settings::spears) Settings::spears("spears");
decltype(Settings::spellWards) Settings::spellWards("spellWards");
decltype(Settings::throwingAxes) Settings::throwingAxes("throwingAxes");
decltype(Settings::throwingKnives) Settings::throwingKnives("throwingKnives");
decltype(Settings::waxes) Settings::waxes("waxes");
