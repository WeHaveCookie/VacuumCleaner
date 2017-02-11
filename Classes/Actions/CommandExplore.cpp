#include "stdafx.h"
#include "CommandExplore.h"
#include "Entity/Entity.h"

void CommandExplore::init(Entity* ent, void* data)
{
	Command::init(ent);
}

void CommandExplore::execute()
{
	getEntity()->explore();
}

void* CommandExplore::makeCopy()
{
	return (void*)new CommandExplore();
}