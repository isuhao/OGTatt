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
    owner_{nullptr},
    age_{0.0f},
    timeSinceHit_{0.0f},
    lifetime_{1.0f},
    fading_{false},
    damage_{23.0f}
{
}

void Bullet::OnNodeSet(Node *node)
{ if (!node) return;

    SceneObject::OnNodeSet(node);

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
    if (fading_)
        timeSinceHit_ += timeStep;

    node_->SetScale(Vector3(Max(1.75f - 10.0f * age_, 1.0f),
                                Max(1.75f - 10.0f * age_, 1.0f),
                                Min(Min(150.0f * age_, 7.0f), Max(7.0f - timeSinceHit_ * 420.0f, 0.1f))
                                ));
    if (age_ > lifetime_)
        Disable();

    if (timeStep > 0.0f && !fading_)
        HitCheck(timeStep);
}

void Bullet::Set(Vector3 position, Vector3 direction, Node* owner)
{
    owner_ = owner;

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

    if (node_->GetScale().x_ <= 0.0f) {

        SceneObject::Disable();
    }
}

void Bullet::HitCheck(float timeStep)
{
    PODVector<PhysicsRaycastResult> hitResults;
    Ray bulletRay(node_->GetPosition() - rigidBody_->GetLinearVelocity() * timeStep, node_->GetDirection());
    if (MC->PhysicsRayCast(hitResults, bulletRay, rigidBody_->GetLinearVelocity().Length() * timeStep * 1.5f, M_MAX_UNSIGNED)){
        for (PhysicsRaycastResult r : hitResults) {

            Node* hitNode{ r.body_->GetNode() };

            if (!r.body_->IsTrigger() && hitNode != owner_) {

                r.body_->ApplyImpulse(rigidBody_->GetLinearVelocity() * 0.023f - 0.23f * r.normal_,
                                      r.position_ - hitNode->GetWorldPosition());

                Substance substance{ Substance::Rock };
                if (hitNode->GetNameHash() == StringHash("Level")){
                    substance = Substance::Rock;
                } else if (SceneObject* sceneObject = hitNode->GetDerivedComponent<SceneObject>()) {
                    substance = sceneObject->GetSubstance();
                } else if (SceneObject* sceneObject = hitNode->GetParent()->GetDerivedComponent<SceneObject>()) {
                    substance = sceneObject->GetSubstance();
                }
                SPAWN->Create<HitFX>()->Set(r.position_, substance);

                if (hitNode->HasComponent<Character>())
                    hitNode->GetComponent<Character>()->Hit(damage_);

                for (Component* c : hitNode->GetComponents()) {

                    if (c->IsInstanceOf<Vehicle>()) {
                        static_cast<Vehicle*>(c)->Hit(damage_);
                        break;
                    }
                }

                Disable();
                return;
            }
        }
    }
}
