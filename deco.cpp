#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Scene.h>

#include "deco.h"
#include "tile.h"

Deco::Deco(Context* context, MasterControl *masterControl, Tile* tile, String name):
    SceneObject(context, masterControl)
{
    rootNode_->SetParent(tile->rootNode_);
    rootNode_->SetPosition(Vector3::ZERO);
    //rootNode_->SetPosition(Vector3(Random(-0.25f, 0.25f), 0.0f, Random(-0.25f, 0.25f)));
}
