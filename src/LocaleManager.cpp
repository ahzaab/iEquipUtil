#include "pch.h"

#include "LocaleManager.h"

#include <string>
#include <fstream>
#include <map>
#include <queue>
#include <stack>
#include <utility>
#include <codecvt>
#include <windows.h>

LocaleManager* LocaleManager::GetSingleton()
{
	static LocaleManager singleton;
	return &singleton;
}


std::wstring LocaleManager::ConvertStringToWString(const std::string& a_str)
{
	if (a_str.empty()) {
		return std::wstring();
	}

	try {
		auto strTo = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(a_str);
		return strTo;
	}
	catch (const std::range_error& e)
	{
		_ERROR("MultiByteToWideChar failed with error code");
	}

	return std::wstring();
}


std::string LocaleManager::ConvertWStringToString(const std::wstring& a_str)
{
	if (a_str.empty()) {
		return std::string();
	}

	auto size = WideCharToMultiByte(CP_UTF8, 0, a_str.c_str(), static_cast<int>(a_str.length()), NULL, 0, NULL, NULL);
	bool err = size == 0;
	if (!err) {
		std::string strTo;
		strTo.resize(size);
		err = WideCharToMultiByte(CP_UTF8, 0, a_str.c_str(), static_cast<int>(a_str.length()), strTo.data(), size, NULL, NULL) == 0;
		if (!err) {
			return strTo;
		}
	}

	if (err) {
		_ERROR("WideCharToMultiByte failed with error code (%i)", GetLastError());
	}

	return std::string();
}


void LocaleManager::Dump()
{
	std::string key;
	std::string value;
	for (auto& pair : GetLocalizationMap()) {
		key = ConvertWStringToString(pair.first);
		value = ConvertWStringToString(pair.second);
		_DMESSAGE("%s: %s", key.c_str(), value.c_str());
	}
}


void LocaleManager::LoadLocalizationStrings()
{
	constexpr wchar_t REGEX_PREFIX[] = L"iEquip_";
	constexpr wchar_t ENGLISH[] = L"ENGLISH";
	constexpr wchar_t REGEX_POSTFIX[] = L"\\.txt$";
	constexpr auto REGEX_FLAGS = std::regex_constants::grep | std::regex_constants::icase;

	std::filesystem::path path("data/interface/translations");

	std::wstring pattern(REGEX_PREFIX);
	std::wstring wLanguage(ENGLISH);
	auto setting = RE::GetINISetting("sLanguage:General");
	if (setting) {
		auto u8Language = setting->GetString();
		wLanguage = ConvertStringToWString(u8Language);
	}
	pattern += wLanguage;
	pattern += REGEX_POSTFIX;
	std::wregex regex(pattern, REGEX_FLAGS);

	bool english = _wcsicmp(ENGLISH, wLanguage.c_str()) == 0;

	FindFiles(path, regex, english);
	if (!english) {
		pattern = REGEX_PREFIX;
		pattern += ENGLISH;
		pattern += REGEX_POSTFIX;
		regex.assign(pattern, REGEX_FLAGS);
		FindFiles(path, regex, true);
	}
}


std::wstring LocaleManager::GetLocalization(std::wstring a_key)
{
	return GetLocalizationInternal(a_key);
}


std::string LocaleManager::GetLocalization(std::string a_key)
{
	auto str = ConvertStringToWString(a_key);
	str = GetLocalization(str);
	return ConvertWStringToString(str);
}


bool LocaleManager::ci_wstring_compare::operator()(const std::wstring& a_lhs, const std::wstring& a_rhs) const
{
	return _wcsicmp(a_lhs.c_str(), a_rhs.c_str()) < 0;
}


void LocaleManager::FindFiles(const std::filesystem::path& a_path, const std::wregex& a_pattern, bool a_english)
{
	std::error_code err;
	if (!std::filesystem::exists(a_path, err)) {
		return;
	}

	std::filesystem::path fileName;
	for (auto& dirEntry : std::filesystem::directory_iterator(a_path)) {
		fileName = dirEntry.path().filename();
		if (std::regex_match(fileName.native(), a_pattern)) {
			ReadFromFile(dirEntry.path(), a_english);
		}
	}
}


#pragma warning(push)
#pragma warning(disable : 4996)  // codecvt deprecated in c++17
void LocaleManager::ReadFromFile(const std::filesystem::path& a_path, bool a_english)
{
	constexpr auto CVT_MODE = std::codecvt_mode(std::little_endian | std::consume_header);
	constexpr auto NPOS = std::wstring::npos;

	auto& localizations = a_english ? _localizations_ENG : _localizations_LOC;
	std::wifstream inFile(a_path);
	inFile.imbue(std::locale(inFile.getloc(), new std::codecvt_utf16<wchar_t, 0x10FFFF, CVT_MODE>));  // UCS-2 LE w/ BOM
	std::wstring line;
	std::wstring key;
	std::wstring value;
	if (!inFile.is_open()) {
		_ERROR("Failed to open file \"%s\"!\n", a_path.string().c_str());
		return;
	}

	while (std::getline(inFile, line)) {
		if (!line.empty() && line.back() == L'\r') {
			line.pop_back();  // discard carriage return
		}
		if (line.empty() || line.front() != L'$') {
			continue;
		}
		key.clear();
		value.clear();

		auto pos = line.find_first_of(L'\t');
		if (pos != NPOS) {
			key = line.substr(0, pos);
			value = line.substr(pos + 1);
		}

		if (!key.empty() && !value.empty()) {
			auto sanitizedKey = SanitizeKey(key);
			if (sanitizedKey) {
				key = std::move(*sanitizedKey);
			}
			localizations.insert(std::make_pair(std::move(key), std::move(value)));
		}
	}
}
#pragma warning(pop)


LocaleManager::LocalizationMap& LocaleManager::GetLocalizationMap()
{
	return _localizations_LOC.empty() ? _localizations_ENG : _localizations_LOC;
}


std::wstring LocaleManager::GetLocalizationInternal(const std::wstring& a_key)
{
	if (a_key.empty() || a_key[0] != L'$') {
		return a_key;
	}

	auto sanitizedKey = SanitizeKey(a_key);
	if (!sanitizedKey) {
		return a_key;
	}

	auto localization = FindLocalization(*sanitizedKey);
	if (!localization) {
		return a_key;
	}

	std::stack<size_type> stack;
	std::queue<std::wstring> queue;
	if (!GetNestedLocalizations(a_key, stack, queue)) {
		return *localization;
	}

	while (!stack.empty()) {
		stack.pop();
	}
	if (InsertLocalizations(*localization, stack, queue))
	{
		for (size_type i = 0, j = 1; j < localization->length(); ++i, ++j)
		{
			if (localization->at(i) == L'\\' && localization->at(j) == L'n')
			{
				localization->replace(i, 2, L"\n");
			}
		}
	}
	return *localization;
}


std::optional<std::wstring> LocaleManager::SanitizeKey(std::wstring a_key)
{
	std::stack<size_type> stack;
	for (size_type pos = 0; pos < a_key.size(); ++pos) {
		switch (a_key[pos]) {
		case L'{':
			stack.push(pos);
			break;
		case L'}':
			{
				switch (stack.size()) {
				case 0:
					return std::nullopt;
				case 1:
					{
						size_type last = stack.top();
						stack.pop();
						auto off = last + 1;
						auto count = pos - last - 1;
						if (count > 0) {
							a_key.replace(off, count, L"");
						}
						pos = off;
					}
					break;
				default:
					stack.pop();
				}
				break;
			}
		}
	}

	if (!a_key.empty() && a_key.back() == L'\r') {
		a_key.pop_back();
	}

	return std::make_optional(a_key);
}


bool LocaleManager::GetNestedLocalizations(const std::wstring& a_key, std::stack<size_type>& a_stack, std::queue<std::wstring>& a_queue)
{
	for (size_type pos = 0; pos < a_key.size(); ++pos) {
		switch (a_key[pos]) {
		case L'{':
			a_stack.push(pos);
			break;
		case L'}':
			{
				switch (a_stack.size()) {
				case 0:
					return false;
				case 1:
					{
						size_type last = a_stack.top();
						a_stack.pop();
						auto off = last + 1;
						auto count = pos - last - 1;
						if (count == 0) {
							return false;	// nothing to replace {} with
						}
						auto subStr = a_key.substr(off, count);
						a_queue.push(GetLocalizationInternal(subStr));
					}
					break;
				default:
					a_stack.pop();
				}
				break;
			}
		}
	}

	return true;
}


std::optional<std::wstring> LocaleManager::FindLocalization(const std::wstring& a_key)
{
	auto& localizations = GetLocalizationMap();
	auto it = localizations.find(a_key);
	if (it == localizations.end()) {
		if (&localizations != &_localizations_ENG) {
			it = _localizations_ENG.find(a_key);
			if (it == _localizations_ENG.end()) {
				return std::nullopt;
			}
		} else {
			return std::nullopt;
		}
	}

	return std::make_optional(it->second);
}


bool LocaleManager::InsertLocalizations(std::wstring& a_localization, std::stack<size_type>& a_stack, std::queue<std::wstring>& a_queue)
{
	for (size_type pos = 0; pos < a_localization.size(); ++pos) {
		switch (a_localization[pos]) {
		case L'{':
			a_stack.push(pos);
			break;
		case L'}':
			{
				if (a_stack.empty() || a_queue.empty()) {
					return false;
				}

				size_type beg = a_stack.top();
				a_stack.pop();
				auto subStr = a_queue.front();
				a_queue.pop();

				a_localization.replace(beg, pos - beg + 1, subStr);
				pos = beg;
			}
			break;
		}
	}

	return true;
}
