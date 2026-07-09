#pragma once
#include "TerrainInclude.h"

class TerrainManager
{
public:

	//地形生成
	void GenerateTerrain();

private:

	//いつもの
	TerrainManager() { Init(); }
	~TerrainManager() { Release(); }

	void Init();
	void Release();

	//地形のリスト管理
	std::list<std::weak_ptr<TerrainBase>> m_wpTerrains;

public:

	static TerrainManager& Instance()
	{
		static TerrainManager instance;
		return instance;
	}

};

#define TERRAINMGR TerrainManager::Instance()