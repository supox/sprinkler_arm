#include "TimeListenersList.h"

TimeListenersList::TimeListenersList()
{
	root = new TimeListenerData(-1,NULL);
}

TimeListenersList::~TimeListenersList()
{
	Clear();
	delete(root);
}

void TimeListenersList::Clear()
{
	// delete list
	TimeListenerData* current = root->Next;
	TimeListenerData* next;
	while(current != NULL) 
	{
		next = current->Next;
		delete(current);
		current = next;
	}
	root->Next = NULL;
}

void TimeListenersList::Add(int Time, ITimeListener* listener)
{
	// Create node
	TimeListenerData *node = new TimeListenerData(Time, listener);
	
	// find the right place for the new node, ordered by time
	TimeListenerData* current ;
	for(
		current = root;
		current->Next != NULL && current->Next->Time < node->Time;
		current = current->Next
	);
	
	// insert the node
	node->Next = current->Next;
	current->Next = node;
}

bool TimeListenersList::Remove(int Time, ITimeListener* listener)
{
	// search the node
	TimeListenerData* current ;
	for(
		current = root;
		current->Next != NULL && current->Next->Time <= Time;
		current = current->Next)
	{
		if(current->Next->listener == listener && current->Next->Time == Time)
		{
			// Found
			TimeListenerData *node = current->Next;
			current->Next = node->Next;
			delete(node);
			return true;
		}
	}
	
	return false;
}

bool TimeListenersList::Remove(ITimeListener* listener)
{
	bool ret = false;
	// search the node
	TimeListenerData* current ;
	for(
		current = root;
		current->Next != NULL;
		current = current->Next)
	{
		if(current->Next->listener == listener)
		{
			// Found
			TimeListenerData *node = current->Next;
			current->Next = node->Next;
			delete(node);
			ret = true;
			if(current->Next == NULL)
				break;
		}
	}
	
	return ret;
}


TimeListenerData* TimeListenersList::GetFirst()
{
	return root->Next;
}

void TimeListenersList::RemoveFirst()
{
	TimeListenerData *node = root->Next;
	if(node != NULL)
	{
		root->Next = node->Next;
		delete(node);
	}
}
