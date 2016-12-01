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

#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/Graphics/ParticleEffect.h>
#include <Urho3D/Resource/ResourceCache.h>

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
{
    SceneObject::OnNodeSet(node_);
//    node_->SetPosition(pos);
//    rootNode_->SetRotation(rot);

    StaticModel* model_ = node_->CreateComponent<StaticModel>();
    model_->SetModel(MC->GetModel("StreetLight"));
    model_->SetMaterial(0, MC->GetMaterial("Metal"));
    model_->SetMaterial(1, MC->GetMaterial("Headlight"));
    model_->SetCastShadows(true);

    node_->CreateComponent<RigidBody>();
    CollisionShape* collider = node_->CreateComponent<CollisionShape>();
    collider->SetCylinder(0.1f, 2.0f);
    collider->SetPosition(Vector3::UP);

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
    light_->SetShadowBias(BiasParameters(0.00001f, 0.5f));
    light_->SetShadowCascade(CascadeParameters(0.23f, 2.0f, 3.0f, 5.0f, 0.5f));
}
