#include "../interface.h"
#include "../widgets/textbox.h"

class GameInterface : public Interface
{
public:
	GameInterface(Vector2 screenSize);
	virtual ~GameInterface();
	virtual void draw() override;
	virtual bool handleEvent(ALLEGRO_EVENT &event);
private:
	Textbox* activeTextBox;
};
