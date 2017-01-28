#pragma once
#include "Command.h"

class CommandClean : public Command
{
public:
	CommandClean() : Command("CommandClean", CommandExeType::JustPressed) {};
	~CommandClean();

	virtual void init(Entity* ent, void* data = NULL);
	virtual void execute();
	virtual void undo() {};
	virtual void* makeCopy();
};

