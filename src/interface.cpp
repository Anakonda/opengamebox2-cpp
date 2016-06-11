#include "interface.h"

Interface::Interface()
{

}

Interface::~Interface()
{
	for(auto &widget : this->widgets)
	{
		delete widget;
	}
}

void Interface::draw()
{
	for(auto &widget : this->widgets)
	{
		widget->draw();
	}
}

void Interface::screenResize(Vector2 newSize)
{
	for(auto &widget : this->widgets)
	{
		widget->screenResize(newSize);
	}
}
