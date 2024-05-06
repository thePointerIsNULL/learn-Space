#pragma once

#ifndef MVector_h 
#define MVector_h
namespace MContainer
{	
	template<typename T >
	class Vector final
	{
	public:
		Vector();
		~Vector();

	private:
		T* m_element;
		unsigned int m_size;



		static unsigned int m_maxSize;
	};



}

#endif // !MVectorContainer 

