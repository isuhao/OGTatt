#pragma once

#include "mastercontrol.h"
#include "tile.h"
#include "sceneobject.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class Deco : public SceneObject
{
    OBJECT(Deco);
public:
    Deco(Context *context, MasterControl* masterControl, Tile *tile, String name);
protected:
};
