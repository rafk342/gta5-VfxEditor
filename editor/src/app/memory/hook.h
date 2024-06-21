#pragma once

#include "app/common/types.h"
#include "MinHook/MinHook.h"
#include "app/logger.h"
#pragma comment(lib, "vendor/MinHook/libMinHook.x64.lib")

/**
 * \brief Utility helper for MinHook library.
 */
class Hook
{
public:
	/**
	 * \brief Initializes MinHook.
	 */
	static void Init()
	{
		MH_STATUS status = MH_Initialize();
		if (status != MH_OK) { LogInfo("MH_Initialize err"); }
	}

	/**
	 * \brief Removes all existing hooks and un-initializes MinHook.
	 */
	static void Shutdown()
	{
		MH_RemoveHook(MH_ALL_HOOKS);
		MH_Uninitialize();
	}

	/**
	 * \brief Enables all existing hooks up to this moment.
	 */
	static void Seal()
	{
		MH_EnableHook(MH_ALL_HOOKS);
	}

	/**
	 * \brief Hooks and redirects given function.
	 * \param from		Pointer to function to replace.
	 * \param to		Pointer to function that will be called instead.
	 * \param backup	Pointer that will be set to address of replaced function.
	 */
	template<typename TFrom, typename TTo, typename TBackup>
	static void Create(TFrom from, TTo to, TBackup* backup, std::string debug_name = "")
	{
		MH_STATUS create = MH_CreateHook((LPVOID)from, (LPVOID)to, (LPVOID*)backup);
		if (create != MH_OK) { LogInfo("{} create hook err", debug_name); };

		MH_STATUS enable = MH_EnableHook((LPVOID)from);
		if (enable != MH_OK) { LogInfo("{} enable hook err", debug_name); };
	}

	/**
	 * \brief Hooks and redirects given function.
	 * \param from		Pointer to function to replace.
	 * \param to		Pointer to function that will be called instead.
	 */
	template<typename TFrom, typename TTo>
	static void Create(TFrom from, TTo to)
	{
		MH_STATUS create = MH_CreateHook((LPVOID)from, (LPVOID)to, NULL);
		MH_STATUS enable = MH_EnableHook((LPVOID)from);
	}

	/**
	 * \brief Removes already installed hook.
	 * \param from Address of function that was passed in corresponding argument of Create function.
	 */
	template<typename TFrom>
	static void Remove(TFrom from, std::string debug_name = "")
	{
		if (from == 0)
			return;

		MH_STATUS disable = MH_DisableHook((pVoid)from);
		if (disable != MH_OK) { LogInfo("{} DisableHook err" , debug_name); };

		MH_STATUS remove = MH_RemoveHook((pVoid)from);
		if (remove != MH_OK) { LogInfo("{} RemoveHook err ", debug_name); };
	}
};
