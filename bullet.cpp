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

#include "hitfx.h"
#include "spawnmaster.h"
#include "character.h"
#include "streetlight.h"
#include "honti.h"
#include "cookiejar.h"

#include "bullet.h"

void Bullet::RegisterObject(Context *context)
{
    context->RegisterFactory<Bullet>();
}

Bullet::Bullet(Context* context):
    SceneObject(context),
//    owner_{owner},
    lifetime_{1.0f},
    damage_{23.0f}
{
}

void Bullet::OnNodeSet(Node *node)
{
    SceneObject::OnNodeSet(node_);

    node_->SetName("Bullet");
    node_->SetScale(Vector3(1.5f, 1.5f, 5.0f));
    model_ = node_->CreateComponent<StaticModel>();
    model_->SetModel(MC->GetModel("Bullet"));
    model_->SetMaterial(MC->GetMaterial("Bullet"));

    rigidBody_ = node_->CreateComponent<RigidBody>();
    rigidBody_->SetMass(0.23f);
    rigidBody_->SetLinearFactor(Vector3::ONE - Vector3::UP);
}

void Bullet::Update(float timeStep)
{
    age_ += timeStep;
    node_->SetScale(Vector3(Max(1.75f - 10.0f * age_, 1.0f),
                                Max(1.75f - 10.0f * age_, 1.0f),
                                Min(Min(150.0f * age_, 7.0f), Max(7.0f - timeSinceHit_ * 42.0f, 0.1f))
                                ));
    if (age_ > lifetime_) {
        Disable();
    }

    if (timeStep > 0.0f && !fading_)
        HitCheck(timeStep);
}

void Bullet::Set(Vector3 position, Vector3 direction)
{
    age_ = 0.0f;
    timeSinceHit_ = 0.0f;
    fading_ = false;

    rigidBody_->SetLinearVelocity(Vector3::ZERO);
    rigidBody_->ResetForces();
    SceneObject::Set(position);

    node_->SetDirection(direction);
    rigidBody_->ApplyForce((Vector3(Random(-0.01f, 0.01f),
                                    Random(-0.01f, 0.01f),
                                    Random(-0.01f, 0.01f))
                            + direction) * 256.0f);
}

void Bullet::Disable()
{
    fading_ = true;
    node_->SetEnabled(false);
}

void Bullet::HitCheck(float timeStep) {
    if (!fading_) {
        PODVector<PhysicsRaycastResult> hitResults;
        Ray bulletRay(node_->GetPosition() - rigidBody_->GetLinearVelocity() * timeStep, node_->GetDirection());
        if (MC->PhysicsRayCast(hitResults, bulletRay, rigidBody_->GetLinearVelocity().Length()*timeStep*1.5f, M_MAX_UNSIGNED)){
            for (PhysicsRaycastResult r : hitResults){
                if (!r.body_->IsTrigger() && r.body_->GetNode() != owner_){
                    r.body_->ApplyImpulse(rigidBody_->GetLinearVelocity() * 0.023f - 0.23f * r.normal_,
r.position_ - r.body_->GetNode()->GetWorldPosition());
                    Substance substance{ Substance::Flesh };
                    if (r.body_->GetNode()->GetNameHash() == StringHash("Level")){
                        substance = Substance::Rock;
                    } else if (r.body_->GetNode()->HasComponent<StreetLight>() ||
                               r.body_->GetNode()->HasComponent<Honti>() ||
                               r.body_->GetNode()->HasComponent<Cookiejar>()) {
                        substance = Substance::Metal;
                    }
                    SPAWN->Create<HitFX>()->Set(r.position_, substance);

                    if (r.body_->GetNode()->HasComponent<Character>())
                        r.body_->GetNode()->GetComponent<Character>()->Hit(damage_);

                    Disable();
                    return;
                }
            }
        }
    }
}
