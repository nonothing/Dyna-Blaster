﻿#include "Boss/Cyclop.h"

USING_NS_CC;

Cyclop* Cyclop::create(const NPCData& data, BricksVec vec)
{
	Cyclop* npc = new Cyclop();
	if (npc && npc->init(data, vec))
	{
		return (Cyclop*)npc->autorelease();
	}
	CC_SAFE_DELETE(npc);

	return npc;
}

bool Cyclop::init(const NPCData& data, BricksVec vec)
{
	if (!BossBase::init(data, vec))
	{
		return false;
	}
	_sprite = Sprite::createWithSpriteFrameName("cyclop_big_blue_left_3.png");
	_dir = RIGHT;
	NPC::animate(_dir);
	if (_data._id == cyclopeB)
	{
		_sprite->setPositionY(15);
	}
	addChild(_sprite);
	return true;
}

