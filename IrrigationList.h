#pragma once
#include "Irrigation.h"
#include <stddef.h>

class IrrigationNode
{
public:
	IrrigationNode(const Irrigation &_irrigation) 
		: irrigation(_irrigation), Next(NULL)
	{
	}
	
	Irrigation irrigation;
	IrrigationNode* Next;
};

class IrrigationList
{
public:
	IrrigationList();
	~IrrigationList();
	
	void Add(const Irrigation& irrigation);
	bool Remove(const Irrigation& irrigation);
	void Clear();
	bool Empty();

	Irrigation& GetFirst();
	void RemoveFirst();
	
private:
	IrrigationNode root;

};
