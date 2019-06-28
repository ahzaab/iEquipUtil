#include "Settings.h"

#include <fileapi.h>  // FindFirstFile, FindNextFile

#include <cstring>  // memset
#include <string>  // string


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
			if (!Json2Settings::Settings::loadSettings(fileName.c_str(), true, a_dumpParse)) {
				result = false;
			}
		} while (result && FindNextFile(handle, &findData));
		FindClose(handle);
	}

	return result;
}


decltype(Settings::spears)			Settings::spears("spears", false);
decltype(Settings::javelins)		Settings::javelins("javelins", false);
decltype(Settings::grenades)		Settings::grenades("grenades", false);
decltype(Settings::throwingAxes)	Settings::throwingAxes("throwingAxes", false);
decltype(Settings::throwingKnives)	Settings::throwingKnives("throwingKnives", false);
decltype(Settings::waxes)			Settings::waxes("waxes", false);
decltype(Settings::oils)			Settings::oils("oils", false);
decltype(Settings::spellWards)		Settings::spellWards("spellWards", false);
decltype(Settings::fire)			Settings::fire("fire", false);
decltype(Settings::ice)				Settings::ice("ice", false);
decltype(Settings::shock)			Settings::shock("shock", false);
decltype(Settings::poison)			Settings::poison("poison", false);
decltype(Settings::salves)			Settings::salves("salves", false);
decltype(Settings::bandages)		Settings::bandages("bandages", false);
