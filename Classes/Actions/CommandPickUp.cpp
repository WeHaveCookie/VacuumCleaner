#include "stdafx.h"
#include "CommandPickUp.h"
#include "Entity/Entity.h"
#include "Manager/Level/LevelMgr.h"
#include "Manager/Sound/SoundMgr.h"

CommandPickUp::~CommandPickUp()
{
}

void CommandPickUp::init(Entity* ent, void* data)
{
	Command::init(ent);
	if (data != nullptr)
	{
		setExeType(*static_cast<CommandExeType::Enum*>(data));
	}
}

void CommandPickUp::execute()
{
	Entity* entity = getEntity();
	if (!entity->hasTarget())
	{
		LevelMgr::getSingleton()->pickCase(entity);
		SoundMgr::getSingleton()->addSound("Data/Sound/FX/cleanJewels.ogg");
	}
}

void* CommandPickUp::makeCopy()
{
	return (void*)new CommandPickUp();
}
