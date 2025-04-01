#pragma once

#include <string>


namespace photon 
{
	class GameObject
	{
	public:
		static uint64_t s_GameObjectId;


		GameObject()
		{
			GameObjectId = s_GameObjectId++;
		}


		virtual std::string GetGameObjectType() = 0;

		uint64_t GameObjectId;
		std::string GameObjectName = "GameObject";

	};
}