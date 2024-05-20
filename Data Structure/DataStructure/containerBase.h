#pragma once
#ifndef ContainerBase_h 
#define ContainerBase_h
#include "container.h"
namespace MContainer
{
	class ContainerBase
	{
	public:
		using uint = unsigned int;
	protected:
		ContainerBase() = default;
		~ContainerBase() = default;
		static const uint m_maxSize = 0xFF000000;
		static constexpr int m_npos = -1;
	};
}


#endif