#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Physics/CollisionShape.h>

#include "wallcollider.h"

WallCollider::WallCollider(Context* context, Level* dungeon, IntVector2 coords):
    Object(context)
{
    CollisionShape* collider = dungeon->rootNode_->CreateComponent<CollisionShape>();
    Vector3 position = Vector3(coords.x_, 0.0f, -coords.y_);
    collider->SetBox(Vector3(1.25f, 1.0f, 1.25f), position);
}
