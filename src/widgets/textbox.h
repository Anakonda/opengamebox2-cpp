#ifndef TEXTBOX_H
#define TEXTBOX_H

#include "../widget.h"

class Textbox : public Widget
{
public:
	Textbox(Vector2 pos, Vector2 size, std::pair<halign, valign> align, Vector2 screenSize, std::string text);

	virtual ~Textbox();

	virtual void draw() override;

	void setText(std::string text);
	std::string getText();

	void move(Vector2 location);
	Vector2 getLocation();

	virtual Widget::type getType() override;
private:
	std::string text;
};

#endif // TEXTBOX_H
