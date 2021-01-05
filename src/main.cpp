#include "pch.h"

#include <clocale>
#include <cstdlib>
#include <string>

#include "Events.h"
#include "Hooks.h"
#include "LocaleManager.h"
#include "Papyrus.h"
#include "RefHandleManager.h"
#include "Registration.h"
#include "Settings.h"
#include "version.h"

#include "SKSE/API.h"


namespace
{
    enum : uint32_t
    {
        kSerializationVersion = 2,
        kOnBoundWeaponEquipped = 'OBWE',
        kOnBoundWeaponUnequipped = 'OBWU',
        kOnRefHandleActive = 'ORHA',
        kOnRefHandleInvalidated = 'ORHI',
        kRefHandleManager = 'RFHM'
    };


    std::string DecodeTypeCode(uint32_t a_typeCode)
    {
        constexpr std::size_t SIZE = sizeof(uint32_t);

        std::string sig;
        sig.resize(SIZE);
        char* iter = reinterpret_cast<char*>(&a_typeCode);
        for (std::size_t i = 0, j = SIZE - 2; i < SIZE - 1; ++i, --j) {
            sig[j] = iter[i];
        }
        return sig;
    }


    void SaveCallback(SKSE::SerializationInterface* a_intfc)
    {
        if (!OnBoundWeaponEquippedRegSet::GetSingleton()->Save(a_intfc, kOnBoundWeaponEquipped, kSerializationVersion)) {
            logger::error("Failed to save OnBoundWeaponEquipped regs!\n");
        }

        if (!OnBoundWeaponUnequippedRegSet::GetSingleton()->Save(a_intfc, kOnBoundWeaponUnequipped, kSerializationVersion)) {
            logger::error("Failed to save OnBoundWeaponUnequipped regs!\n");
        }

        if (!OnRefHandleActiveRegSet::GetSingleton()->Save(a_intfc, kOnRefHandleActive, kSerializationVersion)) {
            logger::error("Failed to save OnRefHandleActive regs!\n");
        }

        if (!OnRefHandleInvalidatedRegSet::GetSingleton()->Save(a_intfc, kOnRefHandleInvalidated, kSerializationVersion)) {
            logger::error("Failed to save OnRefHandleInvalidated regs!\n");
        }

        if (!RefHandleManager::GetSingleton()->Save(a_intfc, kRefHandleManager, kSerializationVersion)) {
            logger::error("Failed to save ref handles!\n");
        }

        logger::info("Finished saving data");
    }


    void LoadCallback(SKSE::SerializationInterface* a_intfc)
    {
        uint32_t type;
        uint32_t version;
        uint32_t length;
        while (a_intfc->GetNextRecordInfo(type, version, length)) {
            if (version != kSerializationVersion) {
                logger::error("Loaded data is out of date! Read ({}), expected ({}) for type code ({})"sv, version, kSerializationVersion, DecodeTypeCode(type).c_str());
                continue;
            }

            switch (type) {
            case kOnBoundWeaponEquipped:
            {
                auto regs = OnBoundWeaponEquippedRegSet::GetSingleton();
                regs->Clear();
                if (!regs->Load(a_intfc)) {
                    logger::error("Failed to load OnBoundWeaponEquipped regs!\n"sv);
                }
            } break;
            case kOnBoundWeaponUnequipped:
            {
                auto regs = OnBoundWeaponUnequippedRegSet::GetSingleton();
                regs->Clear();
                if (!regs->Load(a_intfc)) {
                    logger::error("Failed to load OnBoundWeaponUnequipped regs!\n"sv);
                }
            } break;
            case kOnRefHandleActive:
            {
                auto regs = OnRefHandleActiveRegSet::GetSingleton();
                regs->Clear();
                if (!regs->Load(a_intfc)) {
                    logger::error("Failed to load OnRefHandleActive regs!\n"sv);
                }
            } break;
            case kOnRefHandleInvalidated:
            {
                auto regs = OnRefHandleInvalidatedRegSet::GetSingleton();
                regs->Clear();
                if (!regs->Load(a_intfc)) {
                    logger::error("Failed to load OnRefHandleInvalidated regs!\n"sv);
                }
            } break;
            case kRefHandleManager:
            {
                auto regs = RefHandleManager::GetSingleton();
                regs->Clear();
                if (!regs->Load(a_intfc, version)) {
                    logger::error("Failed to load ref handles!\n"sv);
                }
            } break;
            default:
                logger::error("Unrecognized record type ({})!"sv, DecodeTypeCode(type).c_str());
                break;
            }
        }

        logger::info("Finished loading data");
    }


    void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
    {
        switch (a_msg->type) {
        case SKSE::MessagingInterface::kDataLoaded:
        {
            if (Settings::LoadSettings()) {
                logger::info("Settings loaded successfully"sv);
            } else {
                logger::error("Encountered error while loading settings!\n"sv);
            }

            auto events = RE::ScriptEventSourceHolder::GetSingleton();
            events->AddEventSink(Events::EquipEventHandler::GetSingleton());

            auto refManager = RefHandleManager::GetSingleton();
            refManager->Register();

            auto locManager = LocaleManager::GetSingleton();
            locManager->LoadLocalizationStrings();
        } break;
        }
    }
}


extern "C"
{
    bool SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
    {
        //std::setlocale(LC_ALL, "");

        try {
#ifndef NDEBUG
            auto                    msvc_sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
            auto                    console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            spdlog::sinks_init_list sink_list = { msvc_sink, console_sink };
            auto                    log = std::make_shared<spdlog::logger>("multi_sink", sink_list.begin(), sink_list.end());
            log->set_level(spdlog::level::trace);
            spdlog::flush_every(std::chrono::seconds(3));
            spdlog::register_logger(log);
#else
            auto path = logger::log_directory();
            if (!path) {
                return false;
            }

            *path /= "iEquipUtil.log"sv;

            auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
            auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
            log->set_level(spdlog::level::info);
            log->flush_on(spdlog::level::warn);
            spdlog::flush_every(std::chrono::seconds(3));
#endif

            spdlog::set_default_logger(std::move(log));
            spdlog::set_pattern("%g(%#): [%l] %v");


            logger::info("iEquipUtil v{}"sv, IEQP_VERSION_VERSTRING);

            a_info->infoVersion = SKSE::PluginInfo::kVersion;
            a_info->name = "iEquipUtil";
            a_info->version = IEQP_VERSION_MAJOR;

            if (a_skse->IsEditor()) {
                logger::critical("Loaded in editor, marking as incompatible!\n");
                return false;
            }

            auto ver = a_skse->RuntimeVersion();
            if (ver <= SKSE::RUNTIME_1_5_39) {
                logger::critical("Unsupported runtime version {}!"sv, ver.string().c_str());
                return false;
            }
        } catch (const std::exception& e) {
            logger::critical(e.what());
            return false;
        } catch (...) {
            logger::critical("caught unknown exception"sv);
            return false;
        }

        return true;
    }


    bool SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
    {
        //while (!IsDebuggerPresent()) {
        //    Sleep(10);
        //}

        //Sleep(1000 * 2);


        try {
            logger::info("iEquipUtil loaded");

            SKSE::Init(a_skse);

            auto messaging = SKSE::GetMessagingInterface();
            if (!messaging->RegisterListener("SKSE", MessageHandler)) {
                logger::critical("Could not register MessageHandler"sv);
                return false;
            }

            auto serialization = SKSE::GetSerializationInterface();
            serialization->SetUniqueID('IEQP');
            serialization->SetSaveCallback(SaveCallback);
            serialization->SetLoadCallback(LoadCallback);
            logger::info("Registered serialization callbacks");

            Papyrus::Register();
            Hooks::Install();
        } catch (const std::exception& e) {
            logger::critical(e.what());
            return false;
        } catch (...) {
            logger::critical("caught unknown exception"sv);
            return false;
        }
        return true;
    }
};
