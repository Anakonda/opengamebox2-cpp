#include "vector2.h"
#include "player.h"

#include <map>

extern std::string myID;
extern std::map<std::string, Player> players;

class Entity
{
public:
	Entity(std::string hash, Vector2 location, std::string ownerID, size_t id);
	bool checkHit(Vector2 location);
	bool checkCollision(Entity other);
	Vector2 getSize();
	void move(Vector2 newPos);
	void draw();
	std::string ownerID;
	size_t depth;
	Vector2 location;
	Vector2 dragPos;
	size_t id;
private:
	std::string hash;
	Vector2 destination;
};
