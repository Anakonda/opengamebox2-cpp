#ifndef INTERFACE_H
#define INTERFACE_H

#include <vector>

#include <allegro5/allegro_font.h>

#include "widget.h"

class Interface
{
protected:
	std::vector<Widget*> widgets;
public:
	Interface();
	virtual ~Interface();
	virtual bool handleEvent(ALLEGRO_EVENT &event) = 0;
	virtual void draw();
	virtual void screenResize(Vector2 newSize);
};

#endif
