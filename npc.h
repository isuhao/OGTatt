#pragma once

#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>

#include "mastercontrol.h"
#include "sceneobject.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class NPC : public SceneObject
{
    OBJECT(NPC);
public:
    NPC(Context *context, MasterControl* masterControl, Vector3 pos);
protected:
    RigidBody* rigidBody_;
    CollisionShape* collisionShape_;
    float maxHealth_;
    float health_;
};
