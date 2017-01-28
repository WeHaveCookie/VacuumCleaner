#include "stdafx.h"
#include "CommandClean.h"
#include "Entity/Entity.h"
#include "Manager/Level/LevelMgr.h"
#include "Manager/Sound/SoundMgr.h"

CommandClean::~CommandClean()
{
}

void CommandClean::init(Entity* ent, void* data)
{
	Command::init(ent);
}

void CommandClean::execute()
{
	Entity* entity = getEntity();
	if (!entity->hasTarget())
	{
		LevelMgr::getSingleton()->cleanCase(entity);
		SoundMgr::getSingleton()->addSound("Data/Sound/FX/workDone.ogg");
	}
}

void* CommandClean::makeCopy()
{
	return (void*)new CommandClean();
}
