#pragma once

#include <Urho3D/Scene/SceneEvents.h>

#include "mastercontrol.h"
#include "deco.h"
#include "tile.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class FirePit : public Deco
{
    OBJECT(FirePit);
public:
    FirePit(Context* context, MasterControl *masterControl, Tile *tile);
    void HandleSceneUpdate(StringHash eventType, VariantMap &eventData);
private:
    Node* lightNode_;
    Light* light_;
};
