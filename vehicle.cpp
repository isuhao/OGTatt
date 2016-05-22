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

#include "vehicle.h"
#include "explosion.h"

Vehicle::Vehicle(Vector3 position, Quaternion rotation):
    SceneObject()
{
    rootNode_->SetName("Vehicle");
    rootNode_->SetPosition(position);
    rootNode_->SetRotation(rotation);
    chassisModel_ = rootNode_->CreateComponent<AnimatedModel>();
    chassisModel_->SetCastShadows(true);

    rigidBody_ = rootNode_->CreateComponent<RigidBody>();

    collisionShape_ = rootNode_->CreateComponent<CollisionShape>();

    particleNode_ = rootNode_->CreateChild("Fire");
    flameEmitter_ = particleNode_->CreateComponent<ParticleEmitter>();
    flameEmitter_->SetEffect(MC->cache_->GetResource<ParticleEffect>("Particles/fire1.xml"));
    flameEmitter_->SetEmitting(false);

//    decal_ = rootNode_->CreateComponent<DecalSet>();
//    decal_->SetMaterial(MC->cache_->GetResource<Material>("Materials/Decal.xml"));
//    Quaternion decalRotation = rootNode_->GetRotation();
//    decalRotation  = decalRotation * Quaternion(90.0f, rootNode_->GetRight());
//    decal_->AddDecal(chassisModel_, rootNode_->GetWorldPosition()-0.23f*rootNode_->GetDirection(), decalRotation, 0.666f, 1.0f, 2.3f, Vector2::ZERO, Vector2::ONE);

}

void Vehicle::Hit(float damage)
{
}

void Vehicle::Destroy()
{
    new Explosion(rootNode_->GetPosition(), 1.0f);
    for (unsigned i{0}; i < chassisModel_->GetNumGeometries(); ++i){
        chassisModel_->SetMaterial(i, MC->cache_->GetResource<Material>("Materials/Darkness.xml"));
    }
    flameEmitter_->SetEmitting(true);
}
