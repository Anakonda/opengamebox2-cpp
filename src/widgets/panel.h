#ifndef PANEL_H
#define PANEL_H

#include "../widget.h"
#include "../renderer.h"

#include <vector>

class Panel : public Widget
{
public:
	Panel(Vector2 location, Vector2 size, std::pair<halign, valign> alignment, Vector2 screenSize, ALLEGRO_COLOR* backgroundColor);
	virtual ~Panel();
	virtual void draw() override;
	virtual bool checkHit(Vector2 clickPos) override;
	std::vector<Widget*> widgets;
	virtual void screenResize(Vector2 newSize, Vector2 offset = Vector2()) override;
	virtual Widget::type getType() override;
private:
	ALLEGRO_COLOR* backgroundColor;
};

#endif // PANEL_H
