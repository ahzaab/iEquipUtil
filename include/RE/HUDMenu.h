#pragma once

#include "GameEvents.h"  // BSTEventSink
#include "GameMenus.h"  // IMenu


namespace RE
{
	class ActorValueMeter;
	class UserEventEnabledEvent;
	struct BSRemoteGamepadEvent;


	class HUDObject
	{
	public:
		virtual ~HUDObject();				// 00

		// add
		virtual void	Update(void) = 0;	// 01
		virtual void	Unk_02(void);		// 02 - { return 0; }
		virtual void	Unk_03(void);		// 03
		virtual void	Unk_04(void);		// 04 - { return; }


		// members
#ifndef _WIN64
		UInt32 unk1;
#endif
		GFxMovieView*	view;
#ifndef _WIN64
		UInt32 unk2;
#endif
		GFxValue		unkValue;
	};


	class HUDMeter : public HUDObject
	{
	public:
		virtual ~HUDMeter();					// 00

		// override (HUDObject)
		virtual void	Update(void) override;	// 01
		virtual void	Unk_02(void) override;	// 02 - { return 0; }

		// add
		virtual float	GetFillPct();			// 05 - { return 100.0; }


		// members
		BSFixedString	setPctName;
		BSFixedString	setBlinkingName;
		BSFixedString	fadeOutName;
		float			fillPct;
	};


	class ShoutMeter : public HUDMeter
	{
	public:
		virtual ~ShoutMeter();					// 00

		// override (HUDMeter)
		virtual void	Unk_02(void) override;	// 02
		virtual float	GetFillPct() override;	// 05


		// members
		float cooldown;
	};


	class HUDMenu :
		public IMenu,
		public BSTEventSink<UserEventEnabledEvent>
#if _WIN64
		, public BSTEventSink<BSRemoteGamepadEvent>
#endif

	{
	public:
		virtual ~HUDMenu();	// 00


		// members
		tArray<HUDObject*>	unk40;
		ActorValueMeter*	health;
		ActorValueMeter*	stamina;
		ActorValueMeter*	magicka;
		ShoutMeter*			shout;
	};
}
