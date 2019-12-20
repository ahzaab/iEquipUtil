#include "Settings.h"

#include <fileapi.h>

#include <cstring>
#include <string>


bool Settings::loadSettings(bool a_dumpParse)
{
	bool result = true;

	std::string fileName(FILE_PREFIX);
	fileName += FILE_PATTERN;
	WIN32_FIND_DATA findData;
	std::memset(&findData, 0, sizeof(findData));
	auto handle = FindFirstFile(fileName.c_str(), &findData);
	if (handle != INVALID_HANDLE_VALUE) {
		do {
			fileName = FILE_PREFIX;
			fileName += findData.cFileName;
			if (!Json2Settings::Settings::loadSettings(fileName.c_str(), a_dumpParse, true)) {
				result = false;
			}
		} while (result && FindNextFile(handle, &findData));
		FindClose(handle);
	}

	return result;
}


decltype(Settings::bandages)		Settings::bandages("bandages");
decltype(Settings::fire)			Settings::fire("fire");
decltype(Settings::grenades)		Settings::grenades("grenades");
decltype(Settings::ice)				Settings::ice("ice");
decltype(Settings::javelins)		Settings::javelins("javelins");
decltype(Settings::oils)			Settings::oils("oils");
decltype(Settings::poison)			Settings::poison("poison");
decltype(Settings::rangedSpells)	Settings::rangedSpells("rangedSpells");
decltype(Settings::rangedStaves)	Settings::rangedStaves("rangedStaves");
decltype(Settings::salves)			Settings::salves("salves");
decltype(Settings::shock)			Settings::shock("shock");
decltype(Settings::spears)			Settings::spears("spears");
decltype(Settings::spellWards)		Settings::spellWards("spellWards");
decltype(Settings::throwingAxes)	Settings::throwingAxes("throwingAxes");
decltype(Settings::throwingKnives)	Settings::throwingKnives("throwingKnives");
decltype(Settings::waxes)			Settings::waxes("waxes");
