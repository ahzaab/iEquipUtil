#include "StringExt.h"

#include "skse64/HashUtil.h"

#include <cstdio>
#include <cstring>

#include "LocaleManager.h"


namespace StringExt
{
	UInt32 CalcCRC32Hash(RE::StaticFunctionTag*, RE::BSFixedString a_str, UInt32 a_start)
	{
		return HashUtil::CRC32(a_str.c_str(), a_start);
	}


	RE::BSFixedString IntToHexString(RE::StaticFunctionTag*, UInt32 a_num)
	{
		char buf[] = "DEADBEEF";
		std::snprintf(buf, sizeof(buf), "%08X", a_num);
		return buf;
	}


	RE::BSFixedString LocalizeString(RE::StaticFunctionTag*, RE::BSFixedString a_str)
	{
		if (a_str.empty()) {
			_ERROR("String was empty!\n");
		}

		auto loc = LocaleManager::GetSingleton();
		return loc->GetLocalization(a_str.c_str()).c_str();
	}


	bool RegisterFuncs(RE::BSScript::Internal::VirtualMachine* a_vm)
	{
		a_vm->RegisterFunction("CalcCRC32Hash", "iEquip_StringExt", CalcCRC32Hash);
		a_vm->RegisterFunction("IntToHexString", "iEquip_StringExt", IntToHexString);
		a_vm->RegisterFunction("LocalizeString", "iEquip_StringExt", LocalizeString);

		return true;
	}
}
