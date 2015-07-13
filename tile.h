#pragma once

#include "mastercontrol.h"
#include "level.h"
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Core/CoreEvents.h>

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class Level;

class Tile : public Object
{
    friend class Level;
    friend class Deco;
    OBJECT(Tile);
public:
    Tile(Context *context, const IntVector2 coords, Level *platform);

    virtual void Start();
    virtual void Stop();


    IntVector2 coords_;
    TileType buildingType_ = TT_EMPTY;
    double GetHealth(){return health_;}
    double ApplyDamage(double damage){health_ = Max(health_ - damage, 0.0);}
private:
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    MasterControl* masterControl_;
    Level* dungeon_;
    Node* rootNode_;
    CollisionShape* collisionShape_;
    Node* elements_[TE_LENGTH];
    double health_ = 1.0;
    //A node pointer for each element:
    // 516 ^
    // 402 N
    // 837 |
    void SetBuilding(TileType type);
    TileType GetBuilding();
    void FixFringe();
};
