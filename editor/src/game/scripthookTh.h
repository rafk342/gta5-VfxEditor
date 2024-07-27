#pragma once

#include <iostream>
#include <functional>
#include <thread>
#include <vector>

#include "memory/address.h"
#include "memory/hook.h"
#include "common/types.h"

#include "scripthook/inc/natives.h"
#include "logger.h"

#include "compiler/compiler.h"

using ScriptHookDelegate = std::function<void()>;

class ScriptThreadDispatcher
{
	std::mutex m_Mutex;
	
	std::vector<ScriptHookDelegate> m_Subscribers;
	std::vector<ScriptHookDelegate> m_FrameTasks;

public:
	void Subscribe(const ScriptHookDelegate& delegate)
	{
		std::unique_lock lock(m_Mutex);
		m_Subscribers.push_back(delegate);
	}

	void Invoke(const ScriptHookDelegate& delegate)
	{
		std::unique_lock lock(m_Mutex);
		m_FrameTasks.push_back(delegate);
	}

	void Update()
	{
		std::unique_lock lock(m_Mutex);
		
		if (m_FrameTasks.empty() && m_Subscribers.empty()) 
			return;

		for (const ScriptHookDelegate& delegate : m_Subscribers)
			delegate();

		for (const ScriptHookDelegate& frameTask : m_FrameTasks)
			frameTask();

		m_FrameTasks.clear();
	}
};


class ScriptHook
{
	static inline std::unique_ptr<ScriptThreadDispatcher> sm_Dispatcher;

	static inline int				sm_ScriptID = 0;
	static inline char				sm_ScriptName[] = "audiotest";
	static inline bool				sm_Running = false;
	static inline std::atomic_bool	sm_KillRequested = false;
	static inline u64				sm_CurrentThreadID = 0;
	static inline u64				sm_gtaThread = 0;

	static inline bool* CTheScripts_sm_bUpdatingScriptThreads = nullptr;


	static inline u64(*scrThread_Run_gImpl)(u64);
	static inline u64(*scrThread_Execute_gImpl)(u64, u64, u64, u64);
	static u64 scrThread_Execute_aImpl(u64 a1, u64 a2, u64 a3, u64 a4)
	{
		if (sm_CurrentThreadID != sm_ScriptID)
		{
			return scrThread_Execute_gImpl(a1, a2, a3, a4);
		}

		if (!sm_KillRequested)
		{
			if (sm_Dispatcher) {
				sm_Dispatcher->Update();
			}
		}
		else
		{
			SCRIPT::TERMINATE_THIS_THREAD();
			sm_KillRequested = false;
		}

		return 0;
	}
	static u64 scrThread_Run_aImpl(u64 thread)
	{
		sm_CurrentThreadID = *(u32*)(thread + 8);
		if (sm_gtaThread == 0 && sm_CurrentThreadID == sm_ScriptID)
		{
			sm_gtaThread = thread;
		}

		return scrThread_Run_gImpl(thread);
	}

public:
	static ScriptThreadDispatcher* GetThread() { return sm_Dispatcher.get(); }

	static void Init()
	{
		sm_Dispatcher = std::make_unique<ScriptThreadDispatcher>();

		CTheScripts_sm_bUpdatingScriptThreads = gmAddress::Scan("C6 05 ?? ?? ?? ?? ?? F3 0F 10 05 ?? ?? ?? ?? F3 0F 11 05 ?? ?? ?? ?? E8")
			.GetRef(2)
			.To<bool*>() + 1;
	}

	static void Begin()
	{
		u64 tls = *(u64*)((u64)NtCurrentTeb() + 0x58);
		u64	v5 = *((u64*)tls);
		*(u8*)(v5 + 0x850) = 1;
		*(u64*)(v5 + 0x848) = sm_gtaThread;

		*CTheScripts_sm_bUpdatingScriptThreads = true;
	}

	static void End()
	{
		u64 tls = *(u64*)((u64)NtCurrentTeb() + 0x58);
		u64	v5 = *((u64*)tls);
		*(u8*)(v5 + 0x850) = 0;
		*(u64*)(v5 + 0x848) = 0;

		*CTheScripts_sm_bUpdatingScriptThreads = false;
	}

	// Must be called from some gta thread because accesses rage allocators in tls
	static void Start()
	{

		static gmAddress scrThread_Run_Addr = gmAddress::Scan(

#if game_version == gameVer3095
			"48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 56 41 57 48 83 EC 20 48 8D 81 D4 00 00 00 48 8B E9"
#elif game_version == gameVer2060
			"48 89 6c 24 ?? 48 89 74 24 ?? 57 41 56 41 57 48 83 ec ?? 48 8d 81"
#endif
		);
		Hook::Create(scrThread_Run_Addr, scrThread_Run_aImpl, &scrThread_Run_gImpl, "scrThread_Run_Addr");



		static gmAddress scrThread_Execute_Addr = gmAddress::Scan(
#if game_version == gameVer3095
			"48 8B C4 4C 89 40 ?? 48 89 50 ?? 48 89 48 ?? 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 68 ?? 48 81 EC B8 00 00 00" /*"48 8B C4 4C 89 40 18 48 89 50 10 48 89 48 08 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 68 A1 48 81 EC B8"*/
#elif game_version == gameVer2060	
			"48 8b c4 4c 89 40 ?? 48 89 50 ?? 48 89 48 ?? 55 53 56 57 41 54 41 55 41 56 41 57 48 8d 68 ?? 48 81 ec ?? ?? ?? ?? 4d 8b f9"
#endif
		);
		Hook::Create(scrThread_Execute_Addr, scrThread_Execute_aImpl, &scrThread_Execute_gImpl, "scrThread_Execute_Addr");


		sm_ScriptID = SYSTEM::START_NEW_SCRIPT(sm_ScriptName, 0);
		
		if (sm_ScriptID == 0) {
			LogInfo("Failed to start script thread");
			sm_Running = false;
			return;
		}
		
		sm_Running = true;
	}

	static void Shutdown()
	{
		if (sm_Running)
		{
			sm_KillRequested = true;
			while (sm_KillRequested)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				// Wait until closed, condition variable would make more sense here
			}
			sm_ScriptID = 0;
			sm_Running = false;
		}
		sm_Dispatcher = nullptr;
	}
};


inline void scrBegin() { ScriptHook::Begin(); }
inline void scrEnd() { ScriptHook::End(); }
inline void scrInvoke(const ScriptHookDelegate& delegate)
{
	ScriptHook::GetThread()->Invoke(delegate);
}
