#include "npc.h"

NPC::NPC(Context *context, MasterControl *masterControl, Vector3 pos):
    SceneObject(context, masterControl),
    maxHealth_{100.0f},
    health_{maxHealth_}
{
    rootNode_->SetName("NPC");
    rootNode_->SetPosition(pos);

    rigidBody_ = rootNode_->CreateComponent<RigidBody>();
    rigidBody_->SetLinearFactor(Vector3::ONE - Vector3::UP);
    rigidBody_->SetLinearDamping(0.9f);
    rigidBody_->SetAngularFactor(Vector3::UP);
    rigidBody_->SetAngularDamping(0.9f);
    rigidBody_->SetLinearRestThreshold(0.023f);

    collisionShape_ = rootNode_->CreateComponent<CollisionShape>();
    collisionShape_->SetCylinder(0.4f, 0.5f);
}
