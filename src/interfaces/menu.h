#include "../interface.h"
#include "../widgets/textbox.h"

class Menu : public Interface
{
public:
	Menu(Vector2 screenSize, void (*callback)());
	virtual ~Menu();
	virtual bool handleEvent(ALLEGRO_EVENT &event);
	Textbox* urlTextbox;
	Textbox* nameTextbox;
private:
	Textbox* activeTextBox;
};
