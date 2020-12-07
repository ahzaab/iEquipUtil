#pragma once
#include <filesystem>
#include <map>
#include <optional>
#include <queue>
#include <string>
#include <stack>
#include <utility>


class LocaleManager
{
public:
    static LocaleManager* GetSingleton();
    static std::wstring   ConvertStringToWString(const std::string& a_str);
    static std::string    ConvertWStringToString(const std::wstring& a_str);

    void         Dump();
    void         LoadLocalizationStrings();
    std::wstring GetLocalization(std::wstring a_key);
    std::string  GetLocalization(std::string a_key);

private:
    class ci_wstring_compare
    {
    public:
        bool operator()(const std::wstring& a_lhs, const std::wstring& a_rhs) const;
    };


    typedef std::map<std::wstring, std::wstring, ci_wstring_compare> LocalizationMap;
    typedef std::wstring::size_type                                  size_type;


    LocaleManager() = default;
    LocaleManager(const LocaleManager&) = delete;
    LocaleManager(LocaleManager&&) = delete;
    ~LocaleManager() = default;

    LocaleManager& operator=(const LocaleManager&) = delete;
    LocaleManager& operator=(LocaleManager&&) = delete;

    void                        FindFiles(const std::filesystem::path& a_path, const std::wregex& a_pattern, bool a_english);
    void                        ReadFromFile(const std::filesystem::path& a_path, bool a_english);
    LocalizationMap&            GetLocalizationMap();
    std::wstring                GetLocalizationInternal(const std::wstring& a_key);
    std::optional<std::wstring> SanitizeKey(std::wstring a_key);
    bool                        GetNestedLocalizations(const std::wstring& a_key, std::stack<size_type>& a_stack, std::queue<std::wstring>& a_queue);
    std::optional<std::wstring> FindLocalization(const std::wstring& a_key);
    bool                        InsertLocalizations(std::wstring& a_localization, std::stack<size_type>& a_stack, std::queue<std::wstring>& a_queue);


    LocalizationMap _localizations_ENG;
    LocalizationMap _localizations_LOC;
};
