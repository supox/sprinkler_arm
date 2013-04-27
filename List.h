#pragma once
#include <stddef.h>

template<typename T>
	class List
{
public:
		List() : m_root(NULL)
		{
		}
		
		~List() 
		{
			Clear();
		}
		
		void Clear() {
			Leaf* leaf = m_root.Next;
			while(leaf != NULL) {
				Leaf* leaf_temp = leaf;
				leaf = leaf->Next;
				delete(leaf_temp);
			}
			m_root.Next = NULL;
		}
		
		void Add(T* element) {
			Leaf* leaf = &m_root;
			while(leaf->Next != NULL){
				leaf = leaf->Next;
			}
			leaf->Next = new Leaf(element);
		}
		
		bool Remove(T* element) {
			Leaf* leaf = &m_root;
			while(leaf->Next != NULL && leaf->Next->Element != element){
				leaf = leaf->Next;
			}
			if(leaf->Next == NULL)
				return false;
			Leaf* leaf_temp = leaf->Next;
			leaf->Next = leaf->Next->Next;
			delete(leaf_temp);
			return true;
		}

		inline bool Empty(){return m_root.Next == NULL;}
		
		inline T* Peak() {
			if(Empty())
				return NULL;
			return m_root.Next->Element;
		}
		
		inline T* Dequeue() {
			T* ret = Peak();
			if(ret != NULL)
				Remove(ret);

			return ret;
		}		
		
	protected:
		class Leaf
		{
		public:
			Leaf(T* element = NULL) : Next(NULL), Element(element)
			{
			}
			Leaf* Next;
			T* Element;
		};
		
		Leaf m_root;
};
