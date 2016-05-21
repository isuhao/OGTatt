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

StreetLight::StreetLight(Context* context, MasterControl* masterControl, Vector3 pos, Quaternion rot):
    SceneObject(context, masterControl),
    brightness_{1.8f}
{
    rootNode_->SetPosition(pos);
    rootNode_->SetRotation(rot);

    StaticModel* model_ = rootNode_->CreateComponent<StaticModel>();
    model_->SetModel(masterControl_->cache_->GetResource<Model>("Models/StreetLight.mdl"));
    model_->SetMaterial(0, masterControl_->cache_->GetResource<Material>("Materials/Metal.xml"));
    model_->SetCastShadows(true);

    rootNode_->CreateComponent<RigidBody>();
    CollisionShape* collider = rootNode_->CreateComponent<CollisionShape>();
    collider->SetCylinder(0.1f, 2.0f);

    lightNode_ = rootNode_->CreateChild("LightNode");
    lightNode_->SetPosition(Vector3(0.0f, 2.3f, 0.5f));
    lightNode_->SetDirection(Vector3::DOWN);
    light_ = lightNode_->CreateComponent<Light>();
    light_->SetLightType(LIGHT_SPOT);
    light_->SetBrightness(brightness_);
    light_->SetColor(Color(1.0f, 0.6f, 0.4f));
    light_->SetRange(4.0f);
    light_->SetFov(90.0f);
    light_->SetCastShadows(true);
    light_->SetShadowBias(BiasParameters(0.00023f, 0.5f));
    light_->SetShadowCascade(CascadeParameters(1.0f, 2.0f, 3.0f, 5.0f, 0.5f));
    light_->SetShadowResolution(0.25f);

    SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(StreetLight, HandleSceneUpdate));
}

void StreetLight::HandleSceneUpdate(StringHash eventType, VariantMap& eventData)
{
    light_->SetBrightness(masterControl_->Sine(50.0f, 0.9666f*brightness_, brightness_));
}
