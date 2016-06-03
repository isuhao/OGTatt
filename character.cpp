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

#include "character.h"

Character::Character(Vector3 pos):
    SceneObject(),
    male_{Random(2)},
    maxHealth_{100.0f},
    health_{maxHealth_},
    hairStyle_{static_cast<Hair>(Random(static_cast<int>(HAIRSTYLES)))}
{
    //Reroll hair on bald
    if (hairStyle_ == HAIR_BALD)
        hairStyle_ = static_cast<Hair>(Random(static_cast<int>(HAIRSTYLES)));

    rootNode_->SetName("Character");
    rootNode_->SetPosition(pos);

    //Set up graphics components
    bodyModel_ = rootNode_->CreateComponent<AnimatedModel>();
    bodyModel_->SetCastShadows(true);
    for (int c{0}; c < 5; ++c) {
        switch (c){
        case 0:{
            colors_.Push(LucKey::RandomSkinColor());
        } break;
        case 4:{
            colors_.Push(LucKey::RandomHairColor());
        } break;
        default: colors_.Push(LucKey::RandomColor()); break;
        }
    }

    if (male_)
        bodyModel_->SetModel(MC->GetModel("Male"));
    else
        bodyModel_->SetModel(MC->GetModel("Female"));

    for (unsigned m{0}; m < bodyModel_->GetNumGeometries(); ++m){
        bodyModel_->SetMaterial(m, MC->cache_->GetTempResource<Material>("Materials/Basic.xml"));
        Color diffColor{colors_[m]};
        bodyModel_->GetMaterial(m)->SetShaderParameter("MatDiffColor", diffColor);
        Color specColor{diffColor * (1.0f-0.1f*m)};
        specColor.a_ = 23.0f - 4.0f * m;
        bodyModel_->GetMaterial(m)->SetShaderParameter("MatSpecColor", specColor);
    }

    if (hairStyle_){
        hairModel_ = rootNode_->GetChild("Head", true)->CreateComponent<AnimatedModel>();
        hairModel_->SetCastShadows(true);

        switch (hairStyle_){
        default: case HAIR_BALD: hairModel_->SetModel(nullptr); break;
        case HAIR_MOHAWK:
            hairModel_->SetModel(MC->GetModel("Mohawk"));
            break;
        case HAIR_SEAGULL:
            hairModel_->SetModel(MC->GetModel("Seagull"));
                    break;
        case HAIR_MUSTAIN:
            hairModel_->SetModel(MC->GetModel("Mustain"));
                    break;
        case HAIR_FROTOAD:
            hairModel_->SetModel(MC->GetModel("Frotoad"));
                    break;
        }
        if (hairStyle_ != HAIR_BALD) hairModel_->SetMorphWeight(0, Random());

        //Set color for hair model
        hairModel_->SetMaterial(MC->cache_->GetTempResource<Material>("Materials/Basic.xml"));
        Color diffColor = colors_[4];
        hairModel_->GetMaterial()->SetShaderParameter("MatDiffColor", diffColor);
        Color specColor{diffColor * 0.23f};
        specColor.a_ = 23.0f;
        hairModel_->GetMaterial()->SetShaderParameter("MatSpecColor", specColor);
    }

    animCtrl_ = rootNode_->CreateComponent<AnimationController>();
    animCtrl_->PlayExclusive("Models/IdleRelax.ani", 0, true, 0.1f);
    animCtrl_->SetSpeed("Models/IdleRelax.ani", 1.0f);
    animCtrl_->SetStartBone("Models/IdleRelax.ani", "MasterBone");

    //Setup physics components
    rigidBody_ = rootNode_->CreateComponent<RigidBody>();
    rigidBody_->SetMass(1.0f);
    rigidBody_->SetFriction(0.0f);
    rigidBody_->SetRestitution(0.0f);
    rigidBody_->SetMass(1.0f);
    rigidBody_->SetLinearFactor(Vector3::ONE - Vector3::UP);
    rigidBody_->SetLinearDamping(0.95f);
    rigidBody_->SetAngularFactor(Vector3::UP);
    rigidBody_->SetAngularDamping(0.5f);
    rigidBody_->SetLinearRestThreshold(0.01f);
    rigidBody_->SetAngularRestThreshold(0.1f);

    collisionShape_ = rootNode_->CreateComponent<CollisionShape>();
    collisionShape_->SetCylinder(0.4f, 1.0f);
}

Substance Character::GetSubstance()
{
    return Substance::Flesh;
}
