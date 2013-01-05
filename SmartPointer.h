#pragma once
#include <stddef.h>

template < typename T > class SmartPointer
{
public:
	SmartPointer() : m_ptr(0), m_reference_counter(NULL) 
	{
		m_reference_counter = new RC();
		m_reference_counter->AddRef();
	}

	SmartPointer(T* p) : m_ptr(p), m_reference_counter(NULL)
	{
		m_reference_counter = new RC();
		m_reference_counter->AddRef();
	}

	SmartPointer(const SmartPointer<T>& sp) : m_ptr(sp.m_ptr), m_reference_counter(sp.m_reference_counter)
	{
		m_reference_counter->AddRef();
	}

	~SmartPointer()
	{
		Release();
	}

	inline T& operator* ()
	{
		return *m_ptr;
	}

	inline T* operator-> ()
	{
		return m_ptr;
	}

	inline T* get()
	{
		return m_ptr;
	}
	
	SmartPointer<T>& operator = (const SmartPointer<T>& sp)
	{
		// Assignment operator
		if (this != &sp) // Avoid self assignment
		{
			Release();

			m_ptr = sp.m_ptr;
			m_reference_counter = sp.m_reference_counter;
			m_reference_counter->AddRef();
		}
		return *this;
	}
		
private:
	// Reference counter helper
	class RC
	{
	public:
		RC():count(0)
		{
		}
		inline void AddRef()
		{
			count++;
		}
		inline int Release()
		{
			return --count;
		}
	private:
		unsigned int count; // Reference count
	};

	T* m_ptr;
	RC* m_reference_counter;

	inline void Release()
	{
		if(m_reference_counter->Release() == 0)
		{
			delete m_ptr;
			delete m_reference_counter;
		}		
	}
};
