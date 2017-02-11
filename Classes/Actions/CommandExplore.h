#pragma once
#include "Command.h"

class CommandExplore : public Command
{
public:
	CommandExplore() : Command("CommandExplore", CommandExeType::AtOnce) {};
	~CommandExplore() {};

	virtual void init(Entity* ent, void* data = NULL);
	virtual void execute();
	virtual void undo() {};
	virtual void* makeCopy();
};

