#pragma once

#include <stdlib.h>

template<typename T>
class Vector
{
	public:
		Vector() : m_size(0), m_limit(0), m_elements(NULL)
		{
			Clear();
		}
		
		~Vector() 
		{
			delete[](m_elements);
		}
		
		void Clear() {
			ResizeBuffer(8);
			m_size = 0;
		}
		
		bool Add(const T& element) {
			if (m_size == m_limit)
			{
        if(!ResizeBuffer(m_limit * 2))
            return false;
			}
			m_elements[m_size] = element;
			++m_size;
    
			return true;
		}
		
		T& operator[](const unsigned int index)
		{
			return m_elements[index];
		}
		unsigned int size(){return m_size;}
		bool empty(){return m_size==0;}
		
	protected:
		bool ResizeBuffer(unsigned int new_number_of_elements)
		{
			if(m_elements == 0)
			{
					m_size = 0;
					m_limit = 0;
			}
			
			T *new_elements = new T [new_number_of_elements];
			if(new_elements == NULL)
				return false;
			
			unsigned int size_to_copy = new_number_of_elements < m_size ? new_number_of_elements : m_size;
			for (unsigned int i = 0; i < size_to_copy ; i++)
				new_elements[i] = m_elements[i];
			
			delete[] m_elements;
			m_elements = new_elements;

			m_limit = new_number_of_elements;
			return true;
		}
		
		unsigned int m_size;
    unsigned int m_limit;
    T* m_elements;
};
