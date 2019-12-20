#pragma once

#include "RE/Skyrim.h"


namespace StringExt
{
	UInt32				CalcCRC32Hash(RE::StaticFunctionTag*, RE::BSFixedString a_str, UInt32 a_start);
	RE::BSFixedString	IntToHexString(RE::StaticFunctionTag*, UInt32 a_num);
	RE::BSFixedString	LocalizeString(RE::StaticFunctionTag*, RE::BSFixedString a_str);

	bool RegisterFuncs(RE::BSScript::Internal::VirtualMachine* a_vm);
}
