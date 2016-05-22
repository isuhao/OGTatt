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

#include "frop.h"

Frop::Frop(Vector3 pos, Quaternion rot, Vector3 scale) :
    SceneObject(),
    age_{0.0f},
    scale_{scale}
{
//    growthStart_ = Random(0.0f, 5.0f);
    rootNode_->SetPosition(pos);
    rootNode_->SetRotation(rot);
    rootNode_->SetScale(scale * 4.2f);

//    float randomWidth{Random(2.0f,3.0f)};
//    scale_ = Vector3(randomWidth, Random(0.5f,1.0f+randomWidth), randomWidth);
    fropModel_ = rootNode_->CreateComponent<StaticModel>();
    fropModel_->SetModel(MC->GetModel("Frop"));
    fropModel_->SetMaterial(MC->GetMaterial("Frop"));
    fropModel_->SetCastShadows(true);

//    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Hemp, HandleUpdate));
}

void Frop::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    /*using namespace Update;
    double timeStep = eventData[P_TIMESTEP].GetFloat();
    age_ += timeStep;
    if (age_ > growthStart_ && rootNode_->GetScale().Length() < scale_.Length()-0.01f)
        rootNode_->SetScale(rootNode_->GetScale()+(timeStep*(scale_ - rootNode_->GetScale())));*/
}
