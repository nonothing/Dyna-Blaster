#include "Model/NPC.h"

USING_NS_CC;
#define ANIM_TAG 225 

NPC* NPC::create(const NPCData& data, BricksVec vec)
{
	NPC* npc = new NPC();
	if (npc && npc->init(data, vec))
	{
		return (NPC*)npc->autorelease();
	}

	CC_SAFE_DELETE(npc);

	return npc;
}

void NPC::setMapLayer(cocos2d::Layer* layer)
{
	_mapLayer = layer;
}

bool NPC::init(const NPCData& data, BricksVec vec)
{
    if ( !Layer::init() )
    {
        return false;
    }
	_data = data;
	_isChangeAnimation = false;
	_isDead = false;
	_createTime = Director::getInstance()->getTotalFrames();

	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("npc.plist", "npc.png");
	AnimationCache::getInstance()->addAnimationsWithFile("animation/" + _data._name + ".plist");

	schedule(schedule_selector(NPC::update), 0.03f);
	_sprite = Sprite::createWithSpriteFrameName(_data._name + "_1.png");
	addChild(_sprite);
	_bricks = vec;
	_dir = RIGHT;
	animate(_dir);
	return true;
}

const static Point sPoints[] = { Point(0, -74), Point(74, 0), Point(-74, 0), Point(0, 74) };

void NPC::move()
{
	Point point;
	std::vector< std::pair< Point, Direction> > freePoints;
	for (auto p : sPoints)
	{
		point = getPosition() + p;
		if (isCollisionEmpty(point))
		{
			freePoints.push_back(std::make_pair(point, PointToDir(p)));
		}
	}
	if (!freePoints.empty())
	{
		std::random_shuffle(freePoints.begin(), freePoints.end());
		point = freePoints.at(0).first;
		_dir = freePoints.at(0).second;
		if (_isChangeAnimation) animate(_dir);
		runAction(Sequence::create(MoveTo::create(_data._speed, point), CallFunc::create(CC_CALLBACK_0(NPC::nextDir, this)), nullptr));
	}
}

void NPC::nextDir() //todo rewrite
{
	move();
}

void NPC::animate(Direction dir) 
{
	auto animation = AnimationCache::getInstance()->getAnimation(_data.getAnimationNameMove(_dir));
	if (animation)
	{
		_sprite->setFlippedX(_dir == RIGHT);
		_isChangeAnimation = true;
		runAnimate(animation);
	} 
	else
	{
		animation = AnimationCache::getInstance()->getAnimation(_data.getAnimationNameMove());
		if (animation)
		{
			runAnimate(animation);
		}
	}
}

void NPC::dead()
{
	if (!_isDead)
	{
		deadEvent(_data._score);
		_isDead = true;
		stopAllActions();
		_sprite->stopAllActions();
		auto animation = AnimationCache::getInstance()->getAnimation(_data.getAnimationNameDead());
		if (animation)
		{
			auto action = CCSequence::create(Animate::create(animation), CallFunc::create(CC_CALLBACK_0(NPC::destroy, this)), nullptr);
			action->setTag(ANIM_TAG);
			_sprite->runAction(action);
		}
	}
}

void NPC::destroy()
{
	stopAllActions();
	_sprite->stopAllActions();
	_sprite->setOpacity(0);
}

cocos2d::Rect NPC::getRect()
{
	return Rect(0, 0, 60, 60);
}

bool NPC::isDead()
{
	return _isDead;
}

bool NPC::isRemove()
{
	return _sprite->getOpacity() == 0;
}

unsigned int NPC::getCreateTime()
{
	return _createTime;
}

void NPC::runAnimate(cocos2d::Animation* animation)
{
	_sprite->stopActionByTag(ANIM_TAG);
	auto action = RepeatForever::create(Animate::create(animation));
	action->setTag(ANIM_TAG);
	_sprite->runAction(action);
}

bool NPC::isCollisionEmpty(const cocos2d::Point& point)//todo rewrite
{
	Size size = getRect().size;
	Rect obj2Rect = Rect(point.x - size.width / 2, point.y - size.height / 2, size.width, size.height);

	for (auto brick : _bricks)
	{
		if (isMove(brick->getType()))
		{
			Size bSize = brick->getRect().size;
			Point obj1Pos = brick->convertToWorldSpace(brick->getRect().origin) - _mapLayer->getPosition();
			Rect obj1Rect = Rect(obj1Pos.x - bSize.width / 2, obj1Pos.y - bSize.height / 2, bSize.width, bSize.height);

			if (obj1Rect.intersectsRect(obj2Rect))
			{
				return true;
			}
		}
	}

	return false;
}

Direction NPC::PointToDir(const cocos2d::Point& point)
{
	if (point.x != 0)
	{
		return point.x > 0 ? RIGHT : LEFT;
	}
	if (point.y != 0)
	{
		return point.y > 0 ? UP : DOWN;
	}
	return NONE;
}

bool NPC::isMove(BrickType type)
{
	return type == EBACKGROUND || type == EBONUS || ( type == EWALL && _data._type == FLY );
}
