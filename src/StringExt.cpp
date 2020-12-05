#include "pch.h"
#include "StringExt.h"

#include "skse64/HashUtil.h"

#include <cstdio>
#include <cstring>

#include "LocaleManager.h"


namespace StringExt
{
	uint32_t CalcCRC32Hash(RE::StaticFunctionTag*, RE::BSFixedString a_str, uint32_t a_start)
	{
		return HashUtil::CRC32(a_str.c_str(), a_start);
	}


	RE::BSFixedString IntToHexString(RE::StaticFunctionTag*, uint32_t a_num)
	{
		char buf[] = "DEADBEEF";
		std::snprintf(buf, sizeof(buf), "%08X", a_num);
		return buf;
	}


	RE::BSFixedString LocalizeString(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::BSFixedString a_str)
	{
		if (a_str.empty()) {
			a_vm->TraceStack("String was empty!", a_stackID, Severity::kError);
		}

		auto loc = LocaleManager::GetSingleton();
		return loc->GetLocalization(a_str.c_str()).c_str();
	}


	bool RegisterFuncs(VM* a_vm)
	{
		a_vm->RegisterFunction("CalcCRC32Hash", "iEquip_StringExt", CalcCRC32Hash, true);
		a_vm->RegisterFunction("IntToHexString", "iEquip_StringExt", IntToHexString, true);
		a_vm->RegisterFunction("LocalizeString", "iEquip_StringExt", LocalizeString, true);

		return true;
	}
}
