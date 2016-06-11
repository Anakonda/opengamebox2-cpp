#include "widget.h"

Widget::Widget(Vector2 pos, Vector2 size, std::pair<halign, valign> align, Vector2 screenSize)
{
	this->location = pos;
	this->size = size;
	this->alignment = align;
	this->screenResize(screenSize);
	this->focused = false;
}

Widget::~Widget()
{

}

bool Widget::checkHit(Vector2 clickPos)
{
	return this->realLocation.x <= clickPos.x && clickPos.x <= this->realLocation.x + this->size.x
		&& this->realLocation.y <= clickPos.y && clickPos.y <= this->realLocation.y + this->size.y;
}

void Widget::screenResize(Vector2 newSize, Vector2 offset)
{
	Vector2 newLocation;
	switch(this->alignment.first)
	{
		case halign::LEFT:
		{
			newLocation.x = this->location.x;
			break;
		}
		case halign::CENTER:
		{
			newLocation.x = this->location.x + newSize.x / 2;
			break;
		}
		case halign::RIGHT:
		{
			newLocation.x = this->location.x + newSize.x;
			break;
		}
	}
	switch(this->alignment.second)
	{
		case valign::TOP:
		{
			newLocation.y = this->location.y;
			break;
		}
		case valign::CENTER:
		{
			newLocation.y = this->location.y + newSize.y / 2;
			break;
		}
		case valign::BOTTOM:
		{
			newLocation.y = this->location.y + newSize.y;
			break;
		}
	}
	this->realLocation = newLocation + offset;
}
