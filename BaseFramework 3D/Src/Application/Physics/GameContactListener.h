#pragma once

class GameContactListener : public JPH::ContactListener
{
public:

	// 2つの物体が接触した瞬間に Jolt から自動で呼ばれる
	virtual void OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override;
	
	// 2つの物体が接触し続けている間に Jolt から自動で呼ばれる
	virtual void OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override;
};