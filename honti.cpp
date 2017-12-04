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

#include "honti.h"

void Honti::RegisterObject(Context *context)
{
    context->RegisterFactory<Honti>();
}

Honti::Honti(Context* context) :
    Bike(context)
{
    durability_ = initialDurability_ = 500.0f;
}

void Honti::OnNodeSet(Node *node)
{ if (!node) return;

    Bike::OnNodeSet(node);

    model_->SetModel(MC->GetModel("Honti"));
    SharedPtr<Material> paint{MC->GetMaterial("Glass")->Clone()};
    model_->SetMaterial(0, paint);
    paint->SetShaderParameter("MatDiffColor", LucKey::RandomColor());
    model_->SetMaterial(1, MC->GetMaterial("Glass"));
    model_->SetMaterial(2, MC->GetMaterial("Darkness"));
    model_->SetMaterial(3, MC->GetMaterial("Headlight"));
    model_->SetMaterial(4, MC->GetMaterial("Taillight"));

//    chassisBody_->SetLinearFactor(Vector3::ONE - Vector3::UP);
//    chassisBody_->SetAngularFactor(Vector3::UP);
    rigidBody_->SetFriction(0.42f);
    rigidBody_->SetLinearDamping(0.9f);
    rigidBody_->SetLinearRestThreshold(0.01f);
    rigidBody_->SetAngularDamping(0.9f);
    rigidBody_->SetAngularRestThreshold(0.1f);
    collisionShape_->SetBox(Vector3(0.23f, 0.666f, 1.1f), Vector3::UP * 0.33f + Vector3::RIGHT * 0.034f /*only when parked*/, Quaternion(9.0f, Vector3::FORWARD));

    particleNode_->SetPosition(Vector3(0.023f, 0.5f, 0.08f));

    SetupLights(1, 1, BoundingBox(Vector3(-1.0f, 0.55f, -0.44f), Vector3(1.0f, 0.52f, 0.52f)));
}





