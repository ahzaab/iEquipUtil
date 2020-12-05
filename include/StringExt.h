#pragma once

#include "RE/Skyrim.h"


namespace StringExt
{
	using VM = RE::BSScript::IVirtualMachine;
	using StackID = RE::VMStackID;
	using Severity = RE::BSScript::ErrorLogger::Severity;


	uint32_t				CalcCRC32Hash(RE::StaticFunctionTag*, RE::BSFixedString a_str, uint32_t a_start);
	RE::BSFixedString	IntToHexString(RE::StaticFunctionTag*, uint32_t a_num);
	RE::BSFixedString	LocalizeString(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_str);

	bool RegisterFuncs(VM* a_vm);
}
