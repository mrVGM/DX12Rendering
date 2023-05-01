#pragma once

#include "BaseObject.h"
#include "ColladaEntities.h"

#include <string>

namespace collada
{
	class ColladaScene : public BaseObject
	{
	public:
		enum State
		{
			NotLoaded,
			Loaded,
			FailedToLoad,
		};
	private:
		State m_state = ColladaScene::State::NotLoaded;
		Scene m_scene;
	public:
		
		ColladaScene();
		bool Load(const std::string& filePath);

		virtual ~ColladaScene();

		Scene& GetScene();
		State GetState();
	};
}