#include "PinHit.h"

void PinHit::Init()
{
	// マネージャー経由でデータ取得
	m_data = EFFECTMGR.GetEffectData(EffectType::HitImpact);

	// 本体のInitを呼ぶ
	EffectBase::Init();
}
