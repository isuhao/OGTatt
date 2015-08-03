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
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/Graphics/ParticleEffect.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource.h>

#include "mastercontrol.h"
#include "vehicle.h"
#include "explosion.h"

Vehicle::Vehicle(Context *context, MasterControl *masterControl, Vector3 position):
    SceneObject(context, masterControl)
{
    rootNode_->SetName("Vehicle");
    rootNode_->SetPosition(position);
    chassisModel_ = rootNode_->CreateComponent<StaticModel>();
    chassisModel_->SetModel(masterControl_->resources.models.vehicles.cookieJar);
    chassisModel_->SetCastShadows(true);
    chassisModel_->SetMaterial(1, masterControl_->resources.materials.metal);
    chassisModel_->SetMaterial(0, masterControl_->resources.materials.paint);

    rigidBody_ = rootNode_->CreateComponent<RigidBody>();
    rigidBody_->SetMass(25.0f);
    rigidBody_->SetLinearFactor(Vector3::ONE - Vector3::UP);
    rigidBody_->SetAngularFactor(Vector3::UP);
    rigidBody_->SetLinearDamping(0.9f);
    rigidBody_->SetLinearRestThreshold(0.01f);
    rigidBody_->SetAngularDamping(0.9f);
    rigidBody_->SetAngularRestThreshold(0.1f);

    CollisionShape* collisionShape = rootNode_->CreateComponent<CollisionShape>();
    collisionShape->SetBox(Vector3(1.0f, 0.6f, 2.0f));

    Node* particleNode = rootNode_->CreateChild("Fire");
    particleNode->SetPosition(Vector3(0.35, 0.5f, 0.9f));
    ParticleEmitter* flameEmitter = particleNode->CreateComponent<ParticleEmitter>();
    flameEmitter->SetEffect(masterControl_->cache_->GetResource<ParticleEffect>("Resources/Particles/fire1.xml"));
    flameEmitter->SetEmitting(false);
    //Destroy();
}

void Vehicle::Hit(double damage)
{
}

void Vehicle::Destroy()
{
    new Explosion(context_, masterControl_, rootNode_->GetPosition(), 1.0);
    for (int i = 0; i < chassisModel_->GetNumGeometries(); i++){
        chassisModel_->SetMaterial(i, masterControl_->resources.materials.darkness);
    }
}
