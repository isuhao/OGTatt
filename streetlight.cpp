/* OG Tatt
// Copyright (C) 2015 LucKey Productions (luckeyproductions.nl)
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

StreetLight::StreetLight(Context* context, MasterControl* masterControl, Tile* tile):
    Deco(context, masterControl, tile, "StreetLight")
{
    rootNode_->SetRotation(Quaternion(0.0f, tile->coords_.y_%2 * 180.0f, 0.0f));
    rootNode_->SetPosition(Vector3(0.125f - (Random(10)%2)*0.25f, 0.0f, 0.125f - (Random(10)%2)*0.25f));

    StaticModel* model_ = rootNode_->CreateComponent<StaticModel>();
    model_->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/StreetLight.mdl"));
    model_->SetMaterial(0, masterControl_->cache_->GetResource<Material>("Resources/Materials/Metal.xml"));
    model_->SetCastShadows(true);

    rootNode_->CreateComponent<RigidBody>();
    CollisionShape* collider = rootNode_->CreateComponent<CollisionShape>();
    collider->SetCylinder(0.1f, 2.0f);

//    lightNode_ = rootNode_->CreateChild("LightNode");
//    lightNode_->SetPosition(Vector3(0.0f, 2.3f, 0.5f));
//    light_ = lightNode_->CreateComponent<Light>();
//    light_->SetBrightness(1.23f);
//    light_->SetColor(Color(1.0f, 0.8f, 0.6f));
//    light_->SetRange(5.0f);
//    light_->SetCastShadows(true);
//    light_->SetShadowBias(BiasParameters(0.00023f, 0.5f));
//    light_->SetShadowCascade(CascadeParameters(1.0f, 2.0f, 3.0f, 5.0f, 0.5f));
//    light_->SetShadowResolution(0.25f);

    SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(StreetLight, HandleSceneUpdate));
}

void StreetLight::HandleSceneUpdate(StringHash eventType, VariantMap& eventData)
{
    //float brightness = masterControl_->Sine(50.0f, 0.95f, 1.0f);
    //light_->SetBrightness(brightness);
}
