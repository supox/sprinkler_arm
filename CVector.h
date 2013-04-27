#pragma once
#include <stddef.h>
#include <stdlib.h>

// Same as vector, but uses realloc for copy elements.
// Thus, suit for primitive types rather than classes.

template<typename T>
class CVector
{
	public:
		CVector() : m_size(0), m_limit(0), m_elements(NULL)
		{
			Clear();
		}
		
		~CVector() 
		{
			if(m_elements != NULL)
				free(m_elements);
			m_elements = NULL;
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

		bool Remove(const T& element) {
			for (size_t i = 0; i < m_size ; i++) {
				if(m_elements[i] == element) {
					return RemoveAt(i);
				}
			}
			return false;
		}

		bool RemoveAt(const size_t index) {
			if(index >= m_size)
				return false;			
			if( index != m_size-1 )
				m_elements[index] = m_elements[m_size-1];
			m_size--;
			
			if(m_limit > m_size * 2) // Pack
				return ResizeBuffer(m_size + 1);
			return true;
		}		
		
		inline T& operator[](const unsigned int index)
		{
			return m_elements[index];
		}
		inline unsigned int size(){return m_size;}
		inline bool empty(){return m_size==0;}
		
		CVector& operator=( const CVector& other ) {
      if( this != &other ) {
				if(ResizeBuffer(other.m_size)) {
					memcpy(m_elements, other.m_elements, m_size * sizeof(T));
					m_size = other.m_size;
				}
      }
      return *this;
		}

		bool operator==( const CVector& other ) {
			if(this == &other)
				return true;
			if(m_size != other.m_size)
				return false;
			for (unsigned int i = 0; i < m_size ; i++)
				if(m_elements[i] != other.m_elements[i])
					return false;
			return true;
		}
		
	protected:
		bool ResizeBuffer(unsigned int new_number_of_elements)
		{
			if(new_number_of_elements < 8) new_number_of_elements = 8;
			
			T *new_elements = (T*)realloc(m_elements, new_number_of_elements * sizeof(T));
			if(new_elements == NULL)
				return false;
			
			m_elements = new_elements;
			m_limit = new_number_of_elements;
			return true;
		}
		
		unsigned int m_size;
    unsigned int m_limit;
    T* m_elements;		
};
