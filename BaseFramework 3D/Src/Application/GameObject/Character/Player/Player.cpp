#include "Player.h"

void Player::Update()
{}

void Player::Init()
{
	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData("Asset/Models/Chara/Player/Player.gltf");
}
