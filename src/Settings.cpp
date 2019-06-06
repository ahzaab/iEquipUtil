#include "Settings.h"


bool Settings::loadSettings(bool a_dumpParse)
{
	Json2Settings::Settings::setFileName(FILE_NAME);
	return Json2Settings::Settings::loadSettings(a_dumpParse);
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
