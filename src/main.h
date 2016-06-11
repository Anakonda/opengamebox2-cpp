#include "renderer.h"
#include "entity.h"
#include "interface.h"

#include <sio_client.h>

#include <map>
#include <mutex>
#include <unordered_set>

bool running = true;

sio::client connection;
Interface* interface;

std::map<std::string, Player> players;

std::map<size_t, Entity> entities;
std::vector<size_t> entityOrder;
std::mutex entityMutex;
std::mutex interfaceMutex;

std::string username;

std::string myID;

ALLEGRO_CONFIG *config;

std::vector<size_t> selectedEntities;

bool movingScreen = false;
bool movingObjects = false;
bool boxSelect = false;
std::pair<Vector2, Vector2> moveScreenStart;
std::pair<Vector2, Vector2> moveObjectsStart;
Vector2 boxSelectStart;

std::unordered_set<size_t> lastMovedEntities;
