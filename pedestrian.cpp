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
    NPC(context, masterControl, pos),
    sinceLastTurn_{0.0f},
    turnInterval_{1.0f},
    direction_{Vector3(Random(-1.0f, 1.0f), 0.0f, Random(-1.0f, 1.0f))},
    walkSpeed_{Random(4.2f, 5.0f)}
{
    male_ = (Random(1.0f)>0.5f) ? true : false;
    rootNode_->SetName("Pedestrian");
    rootNode_->SetRotation(Quaternion(0.0f, Random(360.0f), 0.0f));

    modelNode_ = rootNode_->CreateChild("ModelNode");

    bodyModel_ = modelNode_->CreateComponent<AnimatedModel>();
    if (male_){
        bodyModel_->SetModel(masterControl_->resources.models.characters.male);
        bodyModel_->SetMaterial(1, masterControl_->GetRandomCloth());
        bodyModel_->SetMaterial(2, masterControl_->GetRandomSkin());
        bodyModel_->SetMaterial(0, masterControl_->GetRandomCloth());
        bodyModel_->SetMaterial(3, masterControl_->resources.materials.metal);
    }
    else{
        bodyModel_->SetModel(masterControl_->resources.models.characters.female);
        bodyModel_->SetMaterial(2, masterControl_->GetRandomCloth());
        bodyModel_->SetMaterial(1, masterControl_->GetRandomSkin());
        bodyModel_->SetMaterial(0, masterControl_->GetRandomCloth());
        bodyModel_->SetMaterial(3, masterControl_->resources.materials.metal);
    }
    bodyModel_->SetCastShadows(true);

    animCtrl_ = rootNode_->CreateComponent<AnimationController>();
    animCtrl_->PlayExclusive("Resources/Models/IdleRelax.ani", 0, true, 0.1f);
    animCtrl_->SetSpeed("Resources/Models/IdleRelax.ani", 1.0f);
    animCtrl_->SetStartBone("Resources/Models/IdleRelax.ani", "MasterBone");

    SubscribeToEvent(E_UPDATE, HANDLER(Pedestrian, HandleUpdate));
}

void Pedestrian::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    using namespace Update;
    float timeStep = eventData[P_TIMESTEP].GetFloat();
    sinceLastTurn_ += timeStep;
    if (sinceLastTurn_ > turnInterval_){
        sinceLastTurn_ = 0.0f;
        turnInterval_ = Random(2.3f, 5.0f);
        int r = Random(5);
        switch (r) {
        case 0:
            direction_ = Vector3::FORWARD;
            break;
        case 1:
            direction_ = Vector3::LEFT;
            break;
        case 2:
            direction_ = Vector3::BACK;
            break;
        case 3:
            direction_ = Vector3::RIGHT;
            break;
        case 4:
            direction_ = Vector3::ZERO;
            break;
        default:
            break;
        }
        if (r!=4){
            direction_ += Vector3(Random(-0.1f, 0.1f), 0.0f, Random(-0.1f, 0.1f));
        }
    }
    if (direction_.Length() > 0.23f) rigidBody_->ApplyForce(direction_*walkSpeed_);

    //Update rotation according to movement direction.
        Vector3 velocity = rigidBody_->GetLinearVelocity();
        Quaternion rotation = rootNode_->GetWorldRotation();
        Quaternion aimRotation = rotation;
        aimRotation.FromLookRotation(velocity);
        rootNode_->SetRotation(rotation.Slerp(aimRotation, 7.0f * timeStep * velocity.Length()));

        if (velocity.Length() > 0.1f){
            animCtrl_->PlayExclusive("Resources/Models/WalkRelax.ani", 0, true, 0.15f);
            animCtrl_->SetSpeed("Resources/Models/WalkRelax.ani", velocity.Length()*2.3f);
            animCtrl_->SetStartBone("Resources/Models/WalkRelax.ani", "MasterBone");
        }
        else {
            animCtrl_->PlayExclusive("Resources/Models/IdleRelax.ani", 0, true, 0.15f);
            animCtrl_->SetStartBone("Resources/Models/IdleRelax.ani", "MasterBone");
        }
}
