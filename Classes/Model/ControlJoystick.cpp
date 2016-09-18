#include "Model/ControlJoystick.h"
#include "Model/GameSettings.h"
#include "utils/Utils.h"

USING_NS_CC;

ControlJoystick* ControlJoystick::create()
{
	ControlJoystick* control = new ControlJoystick();
	if (control && control->init())
	{
		return (ControlJoystick*)control->autorelease();
	}

	CC_SAFE_DELETE(control);

	return control;
}

bool ControlJoystick::init()
{
	if (!IControl::initKeyBoard() || !IControl::initTouch()) return false;

	float scale = GameSettings::Instance().getScaleButtons();
	float opacity = GameSettings::Instance().getOpacityButtons();

	for (int i = 0; i < 4; i++)
	{
		auto border = Sprite::create("joystick_border.png");
		border->setTag(7 + i * 10);
		border->setVisible(false);
		setButtonParameters(border, scale, opacity);
		_borders.push_back(border);

		auto joystick = Sprite::create("joystick_" + myUtils::to_string(i + 1) + ".png");
		joystick->setTag(8 + i * 10);
		joystick->setVisible(false);
		joystick->setScale(scale);
		joystick->setOpacity(opacity);
		joystick->setPosition(_borders.at(i)->getPosition());
		addChild(joystick);
		_joysticks.push_back(joystick);
		_directions.push_back(NONE);
		_oldDirs.push_back(NONE);

		auto createBombButton = Sprite::create("bomb_key_" + myUtils::to_string(i + 1) + ".png");
		createBombButton->setTag(5 + i * 10);
		createBombButton->setVisible(false);
		setButtonParameters(createBombButton, scale, opacity);
		_createBombButtons.push_back(createBombButton);

		auto radioButton = Sprite::create("bomb_radio_key.png");
		radioButton->setTag(6);
		radioButton->setVisible(false);
		setButtonParameters(radioButton, scale, opacity);
		_radioButtons.push_back(radioButton);
	}

	return true;
}

void ControlJoystick::setButtonParameters(cocos2d::Sprite* button, float scale, float opacity)
{
	button->setScale(scale);
	button->setOpacity(opacity);
	button->setPosition(GameSettings::Instance().getPosition(button->getTag()));
	addChild(button);
}

bool ControlJoystick::TouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
	Point point = convertToNodeSpace(touch->getLocation());
//	_direction = NONE;
	for (size_t i = 0; i < _borders.size(); i++)
	{
		auto border = _borders.at(i);
		float angle = atan2(border->getPositionX() - point.x, border->getPositionY() - point.y);
		if (touchButton(border, point))
		{
			setJoystickPosition(angle, i);
			findDirection(angle, i);
			if (_directions[i] != NONE)
			{
				_eventMoveDirection(_directions[i], i);
			}
		}
	}
	return true;
}

void ControlJoystick::TouchMoved(cocos2d::Touch* touch, cocos2d::Event* event)
{
	Point point = convertToNodeSpace(touch->getLocation());
	for (size_t i = 0; i < _borders.size(); i++)
	{
		auto border = _borders.at(i);
		float angle = atan2(border->getPositionX() - point.x, border->getPositionY() - point.y);
		if (touchButton(border, point))
		{
			setJoystickPosition(angle, i);
			findDirection(angle, i);
			if (_oldDirs[i] != _directions[i])
			{
				_oldDirs[i] = _directions[i];
				_eventMoveDirection(_directions[i], i);
			}
		}
		else
		{
			if (_oldDirs[i] != NONE)
			{
				_oldDirs[i] = NONE;
				_joysticks.at(i)->setPosition(border->getPosition());
				_eventStopDirection(_directions[i], i);
			}
		}
	}

}

void ControlJoystick::setJoystickPosition(float angle, int index)
{
	auto border = _borders.at(index);
	float distance = border->getPosition().getDistance(border->getPosition() + (border->getContentSize() / 2));
	_joysticks[index]->setPosition(border->getPosition() - Point(sin(angle) * distance, cos(angle) * distance));
}

void ControlJoystick::TouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
	Point point = convertToNodeSpace(touch->getLocation());
	for (size_t i = 0; i < _borders.size(); i++)
	{
		auto border = _borders.at(i);
		if (touchButton(border, point))
		{
			_joysticks.at(i)->setPosition(border->getPosition());
			_oldDirs[i] = NONE;
			_eventStopDirection(_directions[i], i);
		}
		if (touchButton(_createBombButtons[i], point))
		{
			_eventCustom(ECREATEBOMB, i);
		}
		if (touchButton(_radioButtons[i], point))
		{
			_eventCustom(EEXPLODE, i);
		}
	}
}

void ControlJoystick::showRadioButton(PlayerColor color, bool var)
{
	for (auto button : _radioButtons)
	{
		if (button->getTag() == 6 + color * 10)
		{
			button->setVisible(var);
		}
	}
}

void ControlJoystick::showControlPlayer(PlayerColor color, bool isVisisble)
{
	for (size_t i = 0; i < _borders.size(); i++)
	{
		if (_createBombButtons[i]->getTag() == 5 + color * 10)
		{
			_createBombButtons[i]->setVisible(isVisisble);
		}
		if (_borders[i]->getTag() == 7 + color * 10)
		{
			_borders[i]->setVisible(isVisisble);
		}
		if (_joysticks[i]->getTag() == 8 + color * 10)
		{
			_joysticks[i]->setVisible(isVisisble);
		}
	}
}

bool ControlJoystick::touchButton(cocos2d::Sprite* button, const cocos2d::Point& point)
{
	return button->isVisible() ? button->getBoundingBox().containsPoint(point) : false;
}

void ControlJoystick::findDirection(float angle, int i)
{
	angle = (angle * 180.f / M_PI) + 180;
	if (angle > 45 && angle < 135)
	{
		_directions[i] = RIGHT;
	}
	else if (angle > 135 && angle < 225)
	{
		_directions[i] = DOWN;
	}
	else if (angle > 225 && angle < 315)
	{
		_directions[i] = LEFT;
	}
	else
	{
		_directions[i] = UP;
	}
}

void ControlJoystick::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{ // do nothing
}

void ControlJoystick::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
	{
		_eventCustom(EQUIT, 0);
	}
}
