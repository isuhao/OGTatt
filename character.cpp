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

#include "player.h"
#include "bullet.h"
#include "muzzle.h"
#include "ogtattcam.h"
#include "spawnmaster.h"
#include "inputmaster.h"

#include "character.h"

void Character::RegisterObject(Context *context)
{
    context->RegisterFactory<Character>();
}

Character::Character(Context* context):
    Controllable(context),
    male_{static_cast<bool>(Random(2))},
    maxHealth_{100.0f},
    hairStyle_{static_cast<Hair>(Random(HAIR_ALL))},
    sinceLastTurn_{},
    turnInterval_{}
{
}

void Character::OnNodeSet(Node *node)
{
    Controllable::OnNodeSet(node_);

    node_->SetRotation(Quaternion(Random(360.0f), Vector3::UP));

    //Setup physics components
    rigidBody_->SetMass(1.0f);
    rigidBody_->SetFriction(0.0f);
    rigidBody_->SetRestitution(0.0f);
    rigidBody_->SetMass(1.0f);
    rigidBody_->SetLinearFactor(Vector3::ONE - Vector3::UP);
    rigidBody_->SetLinearDamping(0.95f);
    rigidBody_->SetAngularFactor(Vector3::UP);
    rigidBody_->SetLinearRestThreshold(0.01f);
    rigidBody_->SetAngularRestThreshold(0.1f);
    rigidBody_->SetAngularDamping(1.0f);

    shot_sfx = MC->GetSample("Shot");

    //Reroll hair on bald
//    if (hairStyle_ == HAIR_BALD)
//        hairStyle_ = static_cast<Hair>(Random(static_cast<int>(HAIR_ALL)));

    //Set up graphics components
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
        model_->SetModel(MC->GetModel("Male"));
    else
        model_->SetModel(MC->GetModel("Female"));

    for (unsigned m{0}; m < model_->GetNumGeometries(); ++m){
        model_->SetMaterial(m, MC->CACHE->GetTempResource<Material>("Materials/Basic.xml"));
        Color diffColor{ colors_[m] };
        if (m == 4){
            if (hairStyle_ == HAIR_BALD)
                diffColor = colors_[0];
            else if (hairStyle_ == HAIR_MOHAWK)
                diffColor = LucKey::RandomHairColor(true);
        }
        model_->GetMaterial(m)->SetShaderParameter("MatDiffColor", diffColor);
        Color specColor{ diffColor * (1.0f-0.1f*m) };
        specColor.a_ = 23.0f - 4.0f * m;
        model_->GetMaterial(m)->SetShaderParameter("MatSpecColor", specColor);
    }

    if (hairStyle_){
        Node* hairNode{node_->GetChild("Head", true)->CreateChild("Hair")};
        hairNode->SetScale(1.0f - (0.13f * !male_));
        hairModel_ = hairNode->CreateComponent<AnimatedModel>();
        hairModel_->SetCastShadows(true);

        switch (hairStyle_){
        default: case HAIR_BALD: case HAIR_SHORT: hairModel_->SetModel(nullptr); break;
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
        case HAIR_FLATTOP:
            hairModel_->SetModel(MC->GetModel("Flattop"));
            break;
        }
        if (hairStyle_ != HAIR_BALD && hairStyle_ != HAIR_SHORT)
        {
            hairModel_->SetMorphWeight(0, Random());

            //Set color for hair model
            hairModel_->SetMaterial(CACHE->GetTempResource<Material>("Materials/Basic.xml"));
            Color diffColor = colors_[4];
            hairModel_->GetMaterial()->SetShaderParameter("MatDiffColor", diffColor);
            Color specColor{diffColor * 0.23f};
            specColor.a_ = 23.0f;
            hairModel_->GetMaterial()->SetShaderParameter("MatSpecColor", specColor);
        }
    }

    animCtrl_ = node_->CreateComponent<AnimationController>();
    animCtrl_->PlayExclusive("Models/IdleRelax.ani", 1, true, 0.1f);
    animCtrl_->SetSpeed("Models/IdleRelax.ani", 1.0f);
    animCtrl_->SetStartBone("Models/IdleRelax.ani", "MasterBone");

    collisionShape_->SetCylinder(0.4f, 1.0f);
}

void Character::Set(Vector3 position)
{
    Controllable::Set(position);
    health_= maxHealth_;
}

void Character::Update(float timeStep)
{
    Controllable::Update(timeStep);

    //Orientation vectors
    Player* player{ GetSubsystem<InputMaster>()->GetPlayerByControllable(this) };
    if (player){
        OGTattCam* cam{ MC->GetCamera(player->GetPlayerId()) };
        if (cam){
            Vector3 camRight{ cam->GetNode()->GetRight() };
            Vector3 camForward{ cam->GetNode()->GetDirection() };
            camRight = LucKey::Scale(camRight, Vector3::ONE - Vector3::UP).Normalized();
            camForward = LucKey::Scale(camForward, Vector3::ONE - Vector3::UP).Normalized();
        }
    }
    //Movement values
    float thrust{ 256.0 };
    float maxSpeed{ 18.0 };

    //Apply movement
    Vector3 force{ move_ * thrust * (1.0f + 0.666f * actions_[0]) * timeStep };
    rigidBody_->ApplyForce(force);

    //Update rotation according to direction of the player's movement.
    Vector3 velocity = rigidBody_->GetLinearVelocity();
    Vector3 lookDirection = velocity + 2.0f * aim_;
    Quaternion rotation = node_->GetWorldRotation();
    Quaternion aimRotation = rotation;
    aimRotation.FromLookRotation(lookDirection);
    node_->SetRotation(rotation.Slerp(aimRotation, 7.0f * timeStep * velocity.Length()));

    //Update animation
    if (velocity.Length() > 0.1f){

        animCtrl_->PlayExclusive("Models/WalkRelax.ani", 1, true, 0.15f);
        animCtrl_->SetSpeed("Models/WalkRelax.ani", velocity.Length() * 2.3f);
        animCtrl_->SetStartBone("Models/WalkRelax.ani", "MasterBone");
    }
    else {

        animCtrl_->PlayExclusive("Models/IdleRelax.ani", 1, true, 0.15f);
        animCtrl_->SetStartBone("Models/IdleRelax.ani", "MasterBone");
    }

    //Shooting
    float bulletHeight{ 0.34f };

    sinceLastShot_ += timeStep;

    if (aim_.Length() > 0.1f) {
        if (sinceLastShot_ > shotInterval_)
        {
            sinceLastShot_ = 0.0;
            Bullet* bullet{ SPAWN->Create<Bullet>() };
            bullet->Set(node_->GetPosition() + Vector3::UP * bulletHeight + collisionShape_->GetSize().x_*0.5f * aim_, aim_);

            Muzzle* muzzle{ SPAWN->Create<Muzzle>() };
            muzzle->Set(node_->GetPosition() + Vector3::UP * bulletHeight + 0.1f * aim_, aim_);
            PlaySample(shot_sfx);
        }
    }
}

void Character::Hit(float damage)
{
    health_ -= damage;

    if (health_ <= 0.0f){
        Disable();
    }
}

Substance Character::GetSubstance()
{
    return Substance::Flesh;
}

void Character::Think(float timeStep)
{
    aim_ = Quaternion(Random(360.0f), Vector3::UP) * Vector3::FORWARD * !Random(23);

    sinceLastTurn_ += timeStep;
    if (sinceLastTurn_ > turnInterval_){
        sinceLastTurn_ = 0.0f;
        turnInterval_ = Random(2.3f, 5.0f);
        int r = Random(5);
        switch (r) {
        case 0:
            move_ = Vector3::FORWARD;
            break;
        case 1:
            move_ = Vector3::LEFT;
            break;
        case 2:
            move_ = Vector3::BACK;
            break;
        case 3:
            move_ = Vector3::RIGHT;
            break;
        case 4:
            move_ = Vector3::ZERO;
            break;
        default:
            break;
        }
        if (r!=4){
            move_ += Vector3(Random(-0.1f, 0.1f), 0.0f, Random(-0.1f, 0.1f));
        }
    }
}
