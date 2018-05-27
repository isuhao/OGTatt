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

#include "bike.h"

Bike::Bike(Context* context):
    Vehicle(context)
{
}

void Bike::OnNodeSet(Node *node)
{ if (!node) return;

    Vehicle::OnNodeSet(node);

    rigidBody_->SetMass(10.0f);
    maxEngineForce_ = 88.0f;
}

void Bike::Update(float timeStep)
{
    Vehicle::Update(timeStep);

    if (Node* forkNode = node_->GetChild("RootBone", true))
        forkNode->SetRotation(Quaternion(0.0f, 180.0f, raycastVehicle_->GetSteeringValue(0) * (rigidBody_->GetLinearVelocity().Length() + 1.0f) * 5.0f));

    if (Node* forkNode = node_->GetChild("ForkBone", true))
        forkNode->SetRotation(Quaternion(200.0f, raycastVehicle_->GetSteeringValue(0) * 42.0f, 0.0f));
}
