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

#include "mastercontrol.h"
#include "vehicle.h"
#include "explosion.h"

Vehicle::Vehicle(Context *context, MasterControl *masterControl, Vector3 position):
    SceneObject(context, masterControl)
{
    rootNode_->SetName("Vehicle");
    rootNode_->SetPosition(position);
    chassisModel_ = rootNode_->CreateComponent<AnimatedModel>();
    chassisModel_->SetModel(masterControl_->resources.models.vehicles.cookieJar);
    chassisModel_->SetCastShadows(true);
    chassisModel_->SetMaterial(0, masterControl_->cache_->GetTempResource<Material>("Resources/Materials/Paint.xml"));
    chassisModel_->GetMaterial(0)->SetShaderParameter("MatDiffColor", RandomColor());
    chassisModel_->SetMaterial(1, masterControl_->resources.materials.glass);
    chassisModel_->SetMaterial(2, masterControl_->resources.materials.darkness);
    chassisModel_->SetMaterial(3, masterControl_->resources.materials.headlights);
    chassisModel_->SetMaterial(4, masterControl_->resources.materials.taillights);

    for (unsigned m = 0; m < chassisModel_->GetNumMorphs(); m++){
        chassisModel_->SetMorphWeight(m, Random());
    }

    rigidBody_ = rootNode_->CreateComponent<RigidBody>();
    rigidBody_->SetMass(25.0f);
    rigidBody_->SetLinearFactor(Vector3::ONE - Vector3::UP);
    rigidBody_->SetAngularFactor(Vector3::UP);
    rigidBody_->SetFriction(0.42f);
    rigidBody_->SetLinearDamping(0.9f);
    rigidBody_->SetLinearRestThreshold(0.01f);
    rigidBody_->SetAngularDamping(0.9f);
    rigidBody_->SetAngularRestThreshold(0.1f);

    CollisionShape* collisionShape = rootNode_->CreateComponent<CollisionShape>();
    collisionShape->SetBox(Vector3(1.16f, 0.6f, 2.1f), Vector3::BACK*0.05f);

    Node* particleNode = rootNode_->CreateChild("Fire");
    particleNode->SetPosition(Vector3(0.023f, 0.5f, 0.9f));
    ParticleEmitter* flameEmitter = particleNode->CreateComponent<ParticleEmitter>();
    flameEmitter->SetEffect(masterControl_->cache_->GetResource<ParticleEffect>("Resources/Particles/fire1.xml"));
    flameEmitter->SetEmitting(Random(2));

    decal_ = rootNode_->CreateComponent<DecalSet>();
    decal_->SetMaterial(masterControl_->cache_->GetResource<Material>("Resources/Materials/Decal.xml"));
    Quaternion decalRotation = rootNode_->GetRotation();
    decalRotation  = decalRotation * Quaternion(90.0f, rootNode_->GetRight());
    decal_->AddDecal(chassisModel_, rootNode_->GetWorldPosition()-0.23f*rootNode_->GetDirection(), decalRotation, 0.666f, 1.0f, 2.3f, Vector2::ZERO, Vector2::ONE);
    if (!Random(5)) Destroy();
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
    decal_->SetEnabled(false);
}
