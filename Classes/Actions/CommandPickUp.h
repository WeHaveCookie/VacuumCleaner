#pragma once
#include "Command.h"

class CommandPickUp : public Command
{
public:
	CommandPickUp() : Command("CommandPickup", CommandExeType::JustPressed) {};
	~CommandPickUp();

	virtual void init(Entity* ent, void* data = NULL);
	virtual void execute();
	virtual void undo() {};
	virtual void* makeCopy();
};

