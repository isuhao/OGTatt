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

#include "cookiejar.h"

Cookiejar::Cookiejar(Context *context, MasterControl *masterControl, Vector3 position) :
    Car(context, masterControl, position)
{
    rootNode_->SetName("Cookiejar");

    chassisModel_->SetModel(masterControl_->resources.models.vehicles.cookiejar);
    SharedPtr<Material> paint = masterControl_->resources.materials.paint->Clone();
    chassisModel_->SetMaterial(0, paint);
    paint->SetShaderParameter("MatDiffColor", LucKey::RandomColor());
    chassisModel_->SetMaterial(1, masterControl_->resources.materials.glass);
    chassisModel_->SetMaterial(2, masterControl_->resources.materials.darkness);
    chassisModel_->SetMaterial(3, masterControl_->resources.materials.headlights);
    chassisModel_->SetMaterial(4, masterControl_->resources.materials.taillights);
    chassisModel_->SetMaterial(5, masterControl_->cache_->GetResource<Material>("Resources/Materials/Decal.xml"));

    for (unsigned m = 0; m < chassisModel_->GetNumMorphs(); m++){
        chassisModel_->SetMorphWeight(m, Random());
    }

    rigidBody_->SetMass(25.0f);
    rigidBody_->SetLinearFactor(Vector3::ONE - Vector3::UP);
    rigidBody_->SetAngularFactor(Vector3::UP);
    rigidBody_->SetFriction(0.42f);
    rigidBody_->SetLinearDamping(0.9f);
    rigidBody_->SetLinearRestThreshold(0.01f);
    rigidBody_->SetAngularDamping(0.9f);
    rigidBody_->SetAngularRestThreshold(0.1f);
    collisionShape_->SetBox(Vector3(1.16f, 0.6f, 2.1f));
    particleNode_->SetPosition(Vector3(0.023f, 0.5f, 0.9f));
}





