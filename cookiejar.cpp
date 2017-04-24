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

#include "inputmaster.h"

void Cookiejar::RegisterObject(Context *context)
{
    context->RegisterFactory<Cookiejar>();
}

Cookiejar::Cookiejar(Context* context) :
    Car(context)
{
    durability_ = initialDurability_ = 100.0f;
}

void Cookiejar::OnNodeSet(Node *node)
{ if (!node) return;

    Car::OnNodeSet(node);

    chassisModel_->SetModel(MC->GetModel("Cookiejar"));

    SharedPtr<Material> paint = MC->GetMaterial("Paint")->Clone();
    chassisModel_->SetMaterial(0, paint);
    paint->SetShaderParameter("MatDiffColor", LucKey::RandomColor());

    chassisModel_->SetMaterial(1, MC->GetMaterial("Glass"));
    chassisModel_->SetMaterial(2, MC->GetMaterial("Darkness"));
    chassisModel_->SetMaterial(3, MC->GetMaterial("Headlight"));
    chassisModel_->SetMaterial(4, MC->GetMaterial("Taillight"));
    chassisModel_->SetMaterial(5, decalMaterial_);

    rigidBody_->SetMass(70.0f);

    chassisCollisionShape_->SetBox(Vector3(1.16f, 0.7f, 2.1f), Vector3::UP * 0.4f);

    particleNode_->SetPosition(Vector3(0.023f, 0.5f, 0.9f));
    SetupLights(2, 2, BoundingBox(Vector3(-0.42f, 0.33f, -1.1f), Vector3(0.42f, 0.41f, 0.9f)));

    GetSubsystem<InputMaster>()->SetPlayerControl(MC->GetPlayer(1), this);
}





