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
}

void Honti::OnNodeSet(Node *node)
{
    Bike::OnNodeSet(node_);

    chassisModel_->SetModel(MC->GetModel("Honti"));
    SharedPtr<Material> paint{MC->GetMaterial("Glass")->Clone()};
    chassisModel_->SetMaterial(0, paint);
    paint->SetShaderParameter("MatDiffColor", LucKey::RandomColor());
    chassisModel_->SetMaterial(1, MC->GetMaterial("Glass"));
    chassisModel_->SetMaterial(2, MC->GetMaterial("Darkness"));
    chassisModel_->SetMaterial(3, MC->GetMaterial("Headlight"));
    chassisModel_->SetMaterial(4, MC->GetMaterial("Taillight"));

    chassisBody_->SetMass(5.0f);
//    chassisBody_->SetLinearFactor(Vector3::ONE - Vector3::UP);
//    chassisBody_->SetAngularFactor(Vector3::UP);
    chassisBody_->SetFriction(0.42f);
    chassisBody_->SetLinearDamping(0.9f);
    chassisBody_->SetLinearRestThreshold(0.01f);
    chassisBody_->SetAngularDamping(0.9f);
    chassisBody_->SetAngularRestThreshold(0.1f);
    chassisCollisionShape_->SetBox(Vector3(0.23f, 0.666f, 1.1f), Vector3::RIGHT * 0.034f /*only when parked*/, Quaternion(9.0f, Vector3::FORWARD));

    particleNode_->SetPosition(Vector3(0.023f, 0.5f, 0.08f));

    SetupLights(1, 1, BoundingBox(Vector3(-1.0f, 0.55f, -0.44f), Vector3(1.0f, 0.52f, 0.52f)));
}





