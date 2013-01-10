#include "IrrigationList.h"

IrrigationList::IrrigationList() :
	root(Irrigation())
{
}

IrrigationList::~IrrigationList()
{
	Clear();
}
	
void IrrigationList::Add(const Irrigation& irrigation)
{
	// Create node
	IrrigationNode *node = new IrrigationNode(irrigation);
	
	// find the right place for the new node, ordered by time
	IrrigationNode *current ;
	for(
		current = &root;
		current->Next != NULL && current->Next->irrigation.start_time < node->irrigation.start_time;
		current = current->Next
	);
	
	// insert the node
	node->Next = current->Next;
	current->Next = node;
}

bool IrrigationList::Remove(const Irrigation& irrigation)
{
	// search the node
	IrrigationNode* current ;
	for(
		current = &root;
		current->Next != NULL && current->Next->irrigation.start_time <= irrigation.start_time;
		current = current->Next)
	{
		if(current->Next->irrigation == irrigation )
		{
			// Found
			IrrigationNode *node = current->Next;
			current->Next = node->Next;
			delete(node);
			return true;
		}
	}
	
	return false;	
}

void IrrigationList::Clear()
{
	// delete list
	IrrigationNode* current = root.Next;
	IrrigationNode* next;
	while(current != NULL) 
	{
		next = current->Next;
		delete(current);
		current = next;
	}
	root.Next = NULL;
}

bool IrrigationList::Empty()
{
	return (root.Next==NULL);
}

Irrigation& IrrigationList::GetFirst()
{
	if(Empty())
	{
		// ERROR...
		return root.irrigation;
	}
	
	return root.Next->irrigation;
}

void IrrigationList::RemoveFirst()
{
	IrrigationNode *node = root.Next;
	if(node != NULL)
	{
		root.Next = node->Next;
		delete(node);
	}
}
