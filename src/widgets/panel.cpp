#include "panel.h"

Panel::Panel(Vector2 location, Vector2 size, std::pair<halign, valign> alignment, Vector2 screenSize, ALLEGRO_COLOR* backgroundColor) :
	Widget(location, size, alignment, screenSize)
{
	this->backgroundColor = backgroundColor;
}

Panel::~Panel()
{
	for(auto &widget : this->widgets)
	{
		delete widget;
	}
}

void Panel::draw()
{
	if(this->focused)
	{
		al_draw_filled_rectangle(this->realLocation.x, this->realLocation.y, this->realLocation.x + this->size.x, this->realLocation.y + this->size.y, *backgroundColor);
		al_draw_rectangle(this->realLocation.x, this->realLocation.y, this->realLocation.x + this->size.x, this->realLocation.y + this->size.y, Renderer::white, 2);
		for(auto &widget : this->widgets)
		{
			widget->draw();
		}
	}
}

bool Panel::checkHit(Vector2 clickPos)
{
	if(!this->focused)
	{
		return false;
	}
	bool hit = Widget::checkHit(clickPos);
	bool childHit = false;
	if(hit)
	{
		for(auto &widget : this->widgets)
		{
			if(widget->checkHit(clickPos))
			{
				childHit = true;
			}
		}
	}
	return childHit;
}

void Panel::screenResize(Vector2 newSize, Vector2 offset)
{
	Widget::screenResize(newSize, offset);
	for(auto &widget : this->widgets)
	{
		widget->screenResize(this->size, this->realLocation);
	}
}

Widget::type Panel::getType()
{
	return Widget::type::PANEL;
}
