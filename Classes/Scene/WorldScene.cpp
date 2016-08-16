#include "WorldScene.h"
#include "Model/Brick.h"
#include "cocostudio/CocoStudio.h"
#include "Model/Bomb.h"

USING_NS_CC;

Scene* WorldScene::createScene()
{
    auto scene = Scene::create();
    auto layer = WorldScene::create();
    scene->addChild(layer);

    return scene;
}

bool WorldScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

	_keyboardListener = EventListenerKeyboard::create();
	_keyboardListener->onKeyPressed = CC_CALLBACK_2(WorldScene::onKeyPressed, this);
	_keyboardListener->onKeyReleased = CC_CALLBACK_2(WorldScene::onKeyReleased, this);
	getEventDispatcher()->addEventListenerWithFixedPriority(_keyboardListener, 100);

	schedule(schedule_selector(WorldScene::update), 0.01f);

	auto rootNode = CSLoader::createNode("WorldScene.csb");
	BricksVec bricks;
	int size = 74; // todo delete magic nubmer
	Point position;
	for (int i = 0; i <= 12; i++)
	{
		for (int j = 0; j <= 10; j++)
		{
			auto brick = Brick::create(1, i, j);
			brick->setPosition(Point(i * size + 148, j * size + 112));
			if (brick->getType() == EBACKGROUND) position = brick->getPosition();
			bricks.push_back(brick);
			addChild(brick, 1);
		}
	}

	for (int i = 0; i < 60; i++)
	{
		int randomNumber = rand() % bricks.size();
		bricks.at(randomNumber)->createWall();
	}

	addChild(rootNode, 0);

	_player = Player::create();
	_player->setPosition(position);
	addChild(_player, 3);
    
    return true;
}

void WorldScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	auto dir = KeyCodeToDiretion(keyCode);
	if (dir != NONE)
	{
		_player->setDirection(dir);
	}
	if (keyCode == EventKeyboard::KeyCode::KEY_SPACE && _player->hasBomb())
	{
		createBomb(_player->getPosition());
	}
	if (keyCode == EventKeyboard::KeyCode::KEY_CTRL && _player->isRemote())
	{
		if (!_bombs.empty())
		{
			_bombs.at(0)->explode();
		}
	}
}

void WorldScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (isMoveKey(keyCode))
	{
		_player->setDirection(NONE);
	}
}

Direction WorldScene::KeyCodeToDiretion(EventKeyboard::KeyCode keyCode)
{
	switch (keyCode)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_W: case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW: return UP;
	case cocos2d::EventKeyboard::KeyCode::KEY_S: case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:return DOWN;
	case cocos2d::EventKeyboard::KeyCode::KEY_A: case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:return LEFT;
	case cocos2d::EventKeyboard::KeyCode::KEY_D: case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:return RIGHT;
	default:	return NONE;
	}
}

void WorldScene::createBomb(const Point& point)
{
	auto bomb = Bomb::create(_player->isRemote());
	bomb->setPosition(point);
	addChild(bomb, 2);
	_bombs.push_back(bomb);
}

bool WorldScene::isMoveKey(cocos2d::EventKeyboard::KeyCode keyCode)
{
	return keyCode == EventKeyboard::KeyCode::KEY_W || keyCode == cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW ||
		keyCode == EventKeyboard::KeyCode::KEY_S || keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW ||
		keyCode == EventKeyboard::KeyCode::KEY_A || keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW ||
		keyCode == EventKeyboard::KeyCode::KEY_D || keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW;
}

void WorldScene::update(float dt)
{
	checkCollisionBombs();
}

void WorldScene::checkCollisionBombs()
{
	//todo need optimization?
	for (auto it = _bombs.begin(); it != _bombs.end();)
	{
		auto bomb = *it;
		if (bomb->isFire())
		{
			_expBomb = bomb;
			it = _bombs.erase(it);
		}
		else
		{
			++it;
		}
	}

	if (_expBomb)
	{
		for (auto bomb : _bombs)
		{
			Point bombPos = bomb->convertToWorldSpace(bomb->getBoundingBox().origin);
			Rect bombRect = Rect(bombPos.x, bombPos.y, bomb->getRect().size.width, bomb->getRect().size.height);

			Point firePos = _expBomb->convertToWorldSpace(_expBomb->getBoundingBox().origin);
			Rect fireRect = Rect(firePos.x, firePos.y, _expBomb->getRect().size.width, _expBomb->getRect().size.height);

			if (isCollisionFire(fireRect, bombRect))
			{
				bomb->explode();
			}
		}
	}
}

bool WorldScene::isCollisionFire(const cocos2d::Rect& rectFire, const cocos2d::Rect& rect)
{
	float w = rectFire.size.width;
	float h = rectFire.size.height;
	float x = rectFire.origin.x - w / 2;
	float y = rectFire.origin.y - h / 2;
	float size = _expBomb->getSize();

	Rect rectFireHorizontal = Rect(x - size * w, y, (size * 2 + 1) * w, h);
	Rect rectFireVertical =   Rect(x, y - size * h, w, (size * 2 + 1) * h);

	return rectFireHorizontal.intersectsRect(rect) || rectFireVertical.intersectsRect(rect);
}

