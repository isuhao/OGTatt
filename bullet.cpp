/* OG Tatt
// Copyright (C) 2016 LucKey Productions (luckeyproductions.nl)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "bullet.h"

Bullet::Bullet(Node* owner):
    SceneObject(),
    owner_{owner},
    lifetime_{1.0f},
    damage_{0.25f}
{
    rootNode_->SetName("Bullet");
    rootNode_->SetScale(Vector3(1.5f, 1.5f, 5.0f));
    model_ = rootNode_->CreateComponent<StaticModel>();
    model_->SetModel(MC->GetModel("Bullet"));
    model_->SetMaterial(MC->GetMaterial("Bullet"));

    rigidBody_ = rootNode_->CreateComponent<RigidBody>();
    rigidBody_->SetMass(0.23f);
    rigidBody_->SetLinearFactor(Vector3::ONE - Vector3::UP);

    SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(Bullet, HandleSceneUpdate));
    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(Bullet, HandlePostRenderUpdate));
}

void Bullet::HandleSceneUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace SceneUpdate;

    float timeStep = eventData[P_TIMESTEP].GetFloat();

    age_ += timeStep;
//    rootNode_->SetScale(Vector3(Max(1.75f - 10.0f*age_, 1.0f),
//                                Max(1.75f - 10.0f*age_, 1.0f),
//                                Min(Min(150.0f*age_, 7.0f), Max(7.0f-timeSinceHit_*42.0f, 0.1f))
//                                ));
    if (age_ > lifetime_) {
        Disable();
    }
}

void Bullet::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace PostRenderUpdate;

    float timeStep = eventData[P_TIMESTEP].GetFloat();
    if (timeStep > 0.0f && !fading_) HitCheck(timeStep);
}

void Bullet::Disable()
{
    fading_ = true;
    rootNode_->SetEnabled(false);
}

void Bullet::HitCheck(float timeStep) {
    if (!fading_) {
        PODVector<PhysicsRaycastResult> hitResults;
        Ray bulletRay(rootNode_->GetPosition() - rigidBody_->GetLinearVelocity() * timeStep, rootNode_->GetDirection());
        if (MC->PhysicsRayCast(hitResults, bulletRay, rigidBody_->GetLinearVelocity().Length()*timeStep*1.5f, M_MAX_UNSIGNED)){
            for (PhysicsRaycastResult r : hitResults){
                if (!r.body_->IsTrigger() && r.body_->GetNode() != owner_){
                    r.body_->ApplyImpulse(rigidBody_->GetLinearVelocity() * 0.023f - 0.23f * r.normal_,
r.position_ - r.body_->GetNode()->GetWorldPosition());
                    Substance substance{Substance::Flesh};
                    if (r.body_->GetNode()->GetNameHash() == StringHash("Level")){
                        substance = Substance::Rock;
                    } else if (r.body_->GetNode()->GetNameHash() == StringHash("StreetLight") ||
                               r.body_->GetNode()->GetNameHash() == StringHash("Honti") ||
                               r.body_->GetNode()->GetNameHash() == StringHash("Cookiejar")) {
                        substance = Substance::Metal;
                    }
                    new HitFX(r.position_, substance);
                    //Deal damage, asks for Component-based approach
//                    unsigned hitID = r.body_->GetNode()->GetID();
                    Disable();
                    return;
                }
            }
        }
    }
}
