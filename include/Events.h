#pragma once

#include "RE/Skyrim.h"


namespace Events
{
	class EquipEventHandler : public RE::BSTEventSink<RE::TESEquipEvent>
	{
	public:
		using EventResult = RE::EventResult;

		static EquipEventHandler* GetSingleton();

		virtual EventResult ReceiveEvent(RE::TESEquipEvent* a_event, RE::BSTEventSource<RE::TESEquipEvent>* a_dispatcher) override;

	private:
		EquipEventHandler() = default;
		EquipEventHandler(const EquipEventHandler&) = delete;
		EquipEventHandler(EquipEventHandler&&) = delete;
		virtual ~EquipEventHandler() = default;

		EquipEventHandler&	operator=(const EquipEventHandler&) = delete;
		EquipEventHandler&	operator=(EquipEventHandler&&) = delete;
	};
}
