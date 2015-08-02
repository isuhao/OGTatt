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

#include "pedestrian.h"

Pedestrian::Pedestrian(Context *context, MasterControl *masterControl, Vector3 pos):
    NPC(context, masterControl, pos)
{
    male_ = (Random(1.0f)>0.5f) ? true : false;
    rootNode_->SetName("Pedestrian");
    rootNode_->SetRotation(Quaternion(0.0f, Random(360.0f), 0.0f));

    rigidBody_->SetMass(5.0f);

    modelNode_ = rootNode_->CreateChild("ModelNode");

    bodyModel_ = modelNode_->CreateComponent<AnimatedModel>();
    if (male_){
        bodyModel_->SetModel(masterControl_->resources.models.characters.male);
        bodyModel_->SetMaterial(1, masterControl_->resources.materials.cloth);
        bodyModel_->SetMaterial(2, masterControl_->resources.materials.skin);
        bodyModel_->SetMaterial(0, masterControl_->resources.materials.pants);
        bodyModel_->SetMaterial(3, masterControl_->resources.materials.metal);
    }
    else{
        bodyModel_->SetModel(masterControl_->resources.models.characters.female);
        bodyModel_->SetMaterial(2, masterControl_->resources.materials.cloth);
        bodyModel_->SetMaterial(1, masterControl_->resources.materials.skin);
        bodyModel_->SetMaterial(0, masterControl_->resources.materials.pants);
        bodyModel_->SetMaterial(3, masterControl_->resources.materials.metal);
    }
    bodyModel_->SetCastShadows(true);

    animCtrl_ = rootNode_->CreateComponent<AnimationController>();
    animCtrl_->PlayExclusive("Resources/Models/Idle.ani", 0, true, 0.25f);
    animCtrl_->SetSpeed("Resources/Models/Idle.ani", 1.0f);
    animCtrl_->SetStartBone("Resources/Models/Idle.ani", "MasterBone");
}
