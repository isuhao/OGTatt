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

#include "streetlight.h"

void StreetLight::RegisterObject(Context *context)
{
    context->RegisterFactory<StreetLight>();
}

StreetLight::StreetLight(Context* context):
    SceneObject(context),
    brightness_{1.8f}
{
}

void StreetLight::OnNodeSet(Node *node)
{ if (!node) return;

    SceneObject::OnNodeSet(node);
//    node_->SetPosition(pos);
//    rootNode_->SetRotation(rot);

    model_ = node_->CreateComponent<StaticModel>();
    model_->SetModel(MC->GetModel("StreetLight"));
    model_->SetMaterial(0, MC->GetMaterial("Metal"));
    model_->SetMaterial(1, MC->GetMaterial("Headlight"));
    model_->SetCastShadows(true);

    node_->CreateComponent<RigidBody>();
    CollisionShape* cylinderCollider = node_->CreateComponent<CollisionShape>();
    cylinderCollider->SetCylinder(0.1f, 2.5f, Vector3::UP * 1.32f);

    CollisionShape* boxCollider = node_->CreateComponent<CollisionShape>();
    boxCollider->SetBox(Vector3(0.15f, 0.11f, 0.5f), Vector3(0.01f, 2.75f, 0.3f));

    lightNode_ = node_->CreateChild("LightNode");
    lightNode_->SetPosition(Vector3(0.0f, 2.3f, 0.5f));
    lightNode_->SetDirection(Vector3::DOWN);
    light_ = lightNode_->CreateComponent<Light>();
    light_->SetLightType(LIGHT_SPOT);
    light_->SetBrightness(brightness_);
    light_->SetColor(Color(1.0f, 0.9f, 0.23f));
    light_->SetRange(6.0f);
    light_->SetFov(130.0f);
    light_->SetCastShadows(true);

    SubscribeToEvent(node_, E_NODECOLLISIONSTART, URHO3D_HANDLER(StreetLight, HandleNodeCollisionStart));
}

void StreetLight::SwitchOff()
{
    light_->SetEnabled(false);
    model_->SetMaterial(1, MC->GetMaterial("HeadlightOff"));
}

void StreetLight::HandleNodeCollisionStart(StringHash eventType, VariantMap& eventData)
{
    MemoryBuffer contacts{ eventData[NodeCollisionStart::P_CONTACTS].GetBuffer() };

    while (!contacts.IsEof())
    {
        Vector3 contactPosition{ contacts.ReadVector3() };
        Vector3 contactNormal{ contacts.ReadVector3() };
        float contactDistance{ contacts.ReadFloat() };
        float contactImpulse{ contacts.ReadFloat() };

        if (contactImpulse > 235.0f){

            Vector3 flatNormal{ LucKey::Scale(contactNormal, Vector3(1.0f, 0.0f, 1.0f)) };
            Vector3 rotAxis{ Quaternion(90.0f, Vector3::UP) * flatNormal.Normalized() };

            node_->Rotate(Quaternion(Min(contactImpulse * 0.023f, 23.5f), rotAxis));

            if (!Random(5))
                SwitchOff();

            if (node_->GetUp().Angle(Vector3::UP) > 42.0f) {
                RigidBody* rigidBody{ node_->GetComponent<RigidBody>() };
                rigidBody->SetMass(2.3f);
                rigidBody->SetFriction(2.3f);

                SwitchOff();
            }
        }
    }
}
