#include "Script_ItemUseFuncEnabler.h"

#include "util\Memory.h"
#include "util\Logging.h"
#include "util\Hook.h"
#include "util/Util.h"

#include "Script.h"

gSScriptInit & GetScriptInit()
{
	static gSScriptInit s_ScriptInit;
	return s_ScriptInit;
}

mCFunctionHook Hook__AI_UseInventoryItem;
GEBool GE_STDCALL _AI_UseInventoryItem(bTObjStack<gScriptRunTimeSingleState> & a_rRunTimeStack, gCScriptProcessingUnit * a_pSPU)
{
	Entity Player = Entity::GetPlayer();
	Entity SelfEntity = Entity(a_pSPU->GetSelfEntity());
	GEU32 BreakBlock = a_rRunTimeStack.AccessAt(a_rRunTimeStack.GetCount() - 1).m_iBreakBlock;
	gSArgsFor__AI_UseInventoryItem const * pArgs = static_cast<gSArgsFor__AI_UseInventoryItem *>(a_rRunTimeStack.Peek().m_pArguments);

	if (BreakBlock == 7 && SelfEntity == Player)
	{
		bCString ItemFuncs = pArgs->m_Consumer.Inventory.GetTemplateItem(pArgs->m_iIndex).Interaction.UseFunc;
		if (!ItemFuncs.IsEmpty())
		{
			gCScriptAdmin & ScriptAdmin = GetScriptAdmin();
			bCString delimeter = ";";
			for (GEInt i = 0; i < ItemFuncs.CountWords(delimeter); i++)
			{
				bCString ItemFunc; ItemFuncs.GetWord(i, delimeter, ItemFunc, GETrue, GETrue);
				if (!ItemFunc.IsEmpty())
					ScriptAdmin.CallScriptFromScript(ItemFunc, &SelfEntity, &SelfEntity, 0);
			}
		}
	}

	return Hook__AI_UseInventoryItem.GetOriginalFunction(&_AI_UseInventoryItem)(a_rRunTimeStack, a_pSPU);
}

extern "C" __declspec(dllexport)
gSScriptInit const * GE_STDCALL ScriptInit(void)
{
	GetScriptAdmin().LoadScriptDLL("Script_Game.dll");

	Hook__AI_UseInventoryItem.Hook(GetScriptAdminExt().GetScriptAIFunction("_AI_UseInventoryItem")->m_funcScriptAIFunction, &_AI_UseInventoryItem, mCBaseHook::mEHookType_OnlyStack);

	return &GetScriptInit();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		::DisableThreadLibraryCalls(hModule);
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
