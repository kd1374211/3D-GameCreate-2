#include "CharaManager.h"
#include "../../Scene/SceneManager.h"

void CharaManager::ResetPlayerPos()
{
	//無ければリターン
	if (m_wpPlayer.expired())return;
}
