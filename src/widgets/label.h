#include "../widget.h"
#include "../renderer.h"

class Label : public Widget
{
public:
	Label(Vector2 location, std::pair<halign, valign> alignment, Vector2 screenSize, ALLEGRO_COLOR textColor, std::string text);
	virtual ~Label();
	virtual void draw() override;
	virtual bool checkHit(Vector2 clickPos) override;
	virtual Widget::type getType() override;
private:
	ALLEGRO_COLOR textColor;
	std::string text;
};
