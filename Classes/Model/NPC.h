#ifndef __NPC_H__
#define __NPC_H__

#include "cocos2d.h"
#include "enumerate/Direction.h"
#include "Model/Brick.h"
#include "Model/WorldObject.h"
#include "Model/Data/NPCData.h"
#include "Model/Bomb.h"
#include "utils/Events.h"

class NPC;
typedef EventTempl<NPC*>					NPCEvent;

const static cocos2d::Point sPoints[] = { cocos2d::Point(0, -74), cocos2d::Point(74, 0), cocos2d::Point(-74, 0), cocos2d::Point(0, 74) };

class NPC : public WorldObject
{
	protected:
	
	NPCData			_data;
	bool			_isDead;
	bool			_isChangeAnimation;
	unsigned int	_createTime;
	cocos2d::Layer* _mapLayer;
	bool			_isFree;
	bool			_isBackMove;
	std::vector<Bomb*>* _bombs;
	cocos2d::Point			_prevPos;

	Direction _dir;
	BricksVec _bricks;
	void runAnimate(cocos2d::Animation* animation);
	bool isCollisionEmpty(const cocos2d::Point& point);
	Direction PointToDir(const cocos2d::Point& point);
	bool	isMove(BrickType type);
	int		getLife();
	virtual std::string	getAnimationName(Direction dir = NONE);
	virtual float getSpeed();
	void moveBack();
public:

	NPCEvent	deadEvent;
    virtual bool init(const NPCData& data);
	void setMapLayer(cocos2d::Layer* layer);
	virtual void move();
	void nextDir();
	void setBricks(BricksVec vec);
	virtual void update(float dt) override;
	virtual void moveUpdate(float dt);
	virtual void animate(Direction dir);
	virtual bool isThroughBomb(Brick* brick);
	virtual void dead();
	void destroy();
	void setBombs(std::vector<Bomb*>* bombs);
	virtual cocos2d::Rect getRect() override;
	bool isDead();
	bool isRemove();
	unsigned int getCreateTime();
	int getScore();
};

typedef std::vector<NPC*> NPCVec;

#endif // __NPC_H__
