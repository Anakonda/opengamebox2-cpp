#include "../widget.h"
#include "../renderer.h"

class Button : public Widget
{
public:
	Button(Vector2 location, Vector2 size, std::pair<halign, valign> alignment, Vector2 screenSize, ALLEGRO_COLOR* backgroundColor, ALLEGRO_COLOR* textColor, std::string text, void (*callback)());
	virtual ~Button();
	virtual void draw() override;
	virtual bool checkHit(Vector2 clickPos) override;
	virtual Widget::type getType() override;
private:
	ALLEGRO_COLOR* backgroundColor;
	ALLEGRO_COLOR* textColor;
	std::string text;

	void (*callback)();
};
