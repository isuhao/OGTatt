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

#include "spawnmaster.h"

#include "car.h"

Car::Car(Context* context) :
    Vehicle(context)
{
}

void Car::OnNodeSet(Node *node)
{ if (!node) return;

    Vehicle::OnNodeSet(node);

    SubscribeToEvent(node_, E_NODECOLLISIONSTART, URHO3D_HANDLER(Car, HandleNodeCollisionStart));
}

void Car::Update(float timeStep)
{
    Vehicle::Update(timeStep);

//    node_->GetChild("Hood", true)->SetRotation(Quaternion(Clamp(node_->WorldToLocal(rigidBody_->GetLinearVelocity()).z_ * -50.0f + 90.0f, 0.0f, 90.0f), 180.0f, 0.0f));
}

void Car::HandleNodeCollisionStart(StringHash eventType, VariantMap& eventData)
{ (void)eventType;

    MemoryBuffer contacts{ eventData[NodeCollisionStart::P_CONTACTS].GetBuffer() };

    while (!contacts.IsEof())
    {
        Vector3 contactPosition{ contacts.ReadVector3() };
        Vector3 contactNormal{ contacts.ReadVector3() };
        float contactDistance{ contacts.ReadFloat() };
        float contactImpulse{ contacts.ReadFloat() };

        if (contactImpulse > 100.0f){

            Vector3 localContactPos{ node_->GetRotation().Inverse() * (contactPosition - node_->GetPosition()) };

            Vector3 carSize{ collisionShape_->GetSize() };
            float carWidth{ carSize.x_ };
            float carLength{ carSize.z_ };

            if (localContactPos.z_ > carLength * 0.25f){
                int morphIndex{ Floor(3.0f * (localContactPos.x_ + carWidth * 0.5f) / carWidth) };

                model_->SetMorphWeight(morphIndex, Clamp(model_->GetMorphWeight(morphIndex)
                                              + contactImpulse * 0.01f, 0.0f, 1.0f));
            }

            Hit(contactImpulse * 0.023f);
        }
    }
}
