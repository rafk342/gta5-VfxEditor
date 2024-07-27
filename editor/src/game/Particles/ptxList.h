#pragma once
#include "ptxEmitterRule.h"
#include "ptxEffectRule.h"
#include "ptxParticleRule.h"
#include "game/rage/paging/Dictionary.h"


namespace rage
{
	struct ptxFxList : public pgBase
	{
		const char* name;
		u32 RefCount;

		void* ptxTextureDictionary;
		u32 ptxTextureDictionaryPusher;

		void* ptxModelDictionary;
		datOwner<pgDictionary<ptxParticleRule>>	ptxParticleRuleDictionary;
		//void* ptxParticleRuleDictionary;
		u32 ptxTextureDictionaryPopper;

		datOwner<pgDictionary<ptxEffectRule>>	ptxEffectRuleDictionary;
		datOwner<pgDictionary<ptxEmitterRule>>	ptxEmitterRuleDictionary;
	};

	struct ptxFxListInfo
	{
		ptxFxList* m_pFxList;
		u32 m_flags;
		u8 pad[4];
	};

}