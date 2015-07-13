#pragma once

#include "sceneobject.h"
#include "mastercontrol.h"
#include "level.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class WallCollider: public Object
{
    OBJECT(WallCollider);
public:
    WallCollider(Context *context, Level* dungeon, IntVector2 coords);
};
