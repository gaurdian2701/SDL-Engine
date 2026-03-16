#pragma once
#include <memory>

#include "EventSystem/EventSystem.h"
#include "InputSystem.h"

namespace Core
{
	inline std::vector<CoreSystem*> MakeCoreSystemsVector()
	{
		return std::vector<CoreSystem*>{
		&Events::EventSystem::GetInstance(),
		&Input::InputSystem::GetInstance()};
	}

	inline std::vector<CoreSystem*> GetCoreSystems()
	{
		static std::vector<CoreSystem*> coreSystems = MakeCoreSystemsVector();
		return coreSystems;
	}
}
