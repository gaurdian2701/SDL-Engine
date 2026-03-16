#pragma once
#include "Application/Application.h"

namespace Core
{
	inline int RunEngine()
	{
		GetApplicationInstance()->Init();
		GetApplicationInstance()->Run();
		return 0;
	}
}

