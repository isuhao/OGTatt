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

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/Graphics/ParticleEffect.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource.h>

#include "mastercontrol.h"
#include "player.h"
#include "ogtattcam.h"
#include "bullet.h"
#include "muzzle.h"

Player::Player(Context *context, MasterControl *masterControl):
    SceneObject(context, masterControl)
{
    rootNode_->SetName("Player");

    model_ = rootNode_->CreateComponent<AnimatedModel>();
    model_->SetModel(masterControl_->resources.models.characters.male);

    model_->SetMaterial(1, masterControl_->GetRandomCloth());
    model_->SetMaterial(2, masterControl_->GetRandomSkin());
    model_->SetMaterial(0, masterControl_->GetRandomCloth());
    model_->SetMaterial(3, masterControl_->resources.materials.metal);

    model_->SetCastShadows(true);

    animCtrl_ = rootNode_->CreateComponent<AnimationController>();
    animCtrl_->PlayExclusive("Resources/Models/IdleRelax.ani", 0, true, 0.1f);
    animCtrl_->SetSpeed("Resources/Models/IdleRelax.ani", 1.0f);
    animCtrl_->SetStartBone("Resources/Models/IdleRelax.ani", "MasterBone");

    rigidBody_ = rootNode_->CreateComponent<RigidBody>();
    rigidBody_->SetFriction(0.0f);
    rigidBody_->SetRestitution(0.0f);
    rigidBody_->SetMass(1.0f);
    rigidBody_->SetLinearFactor(Vector3::ONE - Vector3::UP);
    rigidBody_->SetLinearDamping(0.95f);
    rigidBody_->SetAngularFactor(Vector3::UP);
    rigidBody_->SetAngularDamping(1.0f);
    rigidBody_->SetLinearRestThreshold(0.01f);
    rigidBody_->SetAngularRestThreshold(0.1f);

    CollisionShape* collisionShape = rootNode_->CreateComponent<CollisionShape>();
    collisionShape->SetCylinder(0.3f, 0.5f);

    sample_ = masterControl_->cache_->GetResource<Sound>("Resources/Samples/Shot.ogg");
    sample_->SetLooped(false);
    for (int i = 0; i < 3; i++){
        sampleSources_.Push(SharedPtr<SoundSource>(rootNode_->CreateComponent<SoundSource>()));
        sampleSources_[i]->SetGain(0.3f);
        sampleSources_[i]->SetSoundType(SOUND_EFFECT);
    }

    /*scoreText_ = masterControl_->ui_->GetRoot()->CreateChild<Text>();
    scoreText_->SetText(String(score_));
    scoreText_->SetFont(masterControl_->cache_->GetResource<Font>("Resources/Fonts/skirmishergrad.ttf"), 32);
    scoreText_->SetHorizontalAlignment(HA_LEFT);
    scoreText_->SetVerticalAlignment(VA_TOP);
    scoreText_->SetPosition(0, masterControl_->ui_->GetRoot()->GetHeight()/2.1);*/

    SubscribeToEvent(E_UPDATE, HANDLER(Player, HandleUpdate));
}

void Player::AddScore(int points)
{
    score_ += points;
}

void Player::PlaySample(Sound* sample)
{
    for (int i = 0; i < sampleSources_.Size(); i++){
        if (!sampleSources_[i]->IsPlaying()){
            sampleSources_[i]->Play(sample_);
            break;
        }
    }
}

void Player::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    using namespace Update;

    //Take the frame time step, which is stored as a double
    double timeStep = eventData[P_TIMESTEP].GetFloat();

    Input* input = GetSubsystem<Input>();

    //Orientation vectors
    Vector3 camRight = masterControl_->world.camera->rootNode_->GetRight();
    Vector3 camForward = masterControl_->world.camera->rootNode_->GetDirection();
    camRight = OGTatt::Scale(camRight, Vector3::ONE - Vector3::UP).Normalized();
    camForward = OGTatt::Scale(camForward, Vector3::ONE - Vector3::UP).Normalized();
    //Movement values
    Vector3 move = Vector3::ZERO;
    Vector3 moveJoy = Vector3::ZERO;
    Vector3 moveKey = Vector3::ZERO;
    double thrust = 256.0;
    double maxSpeed = 18.0;
    //Firing values
    Vector3 fire = Vector3::ZERO;
    Vector3 fireJoy = Vector3::ZERO;
    Vector3 fireKey = Vector3::ZERO;

    //Read input
    JoystickState* joystickState = input->GetJoystickByIndex(0);
    if (joystickState){
    moveJoy = camRight * joystickState->GetAxisPosition(0) +
            -camForward * joystickState->GetAxisPosition(1);
    fireJoy = camRight * joystickState->GetAxisPosition(2) +
            -camForward * joystickState->GetAxisPosition(3);
    }
    moveKey = -camRight * input->GetKeyDown(KEY_LEFT) +
               camRight * input->GetKeyDown(KEY_RIGHT) +
            camForward * input->GetKeyDown(KEY_UP) +
            -camForward * input->GetKeyDown(KEY_DOWN);
    fireKey = -camRight * input->GetKeyDown(KEY_J) +
               camRight * input->GetKeyDown(KEY_L) +
            camForward * input->GetKeyDown(KEY_I) +
            -camForward * input->GetKeyDown(KEY_K);

    //Pick most significant input
    moveJoy.Length() > moveKey.Length() ? move = moveJoy : move = moveKey;
    fireJoy.Length() > fireKey.Length() ? fire = fireJoy : fire = fireKey;

    //Restrict move vector length
    if (move.Length() > 1.0f) move.Normalize();
    //Deadzone
    else if (move.Length() < 0.01f) move *= 0.0f;
    //Run
    move = move * (1.0f + 0.666f*input->GetKeyDown(KEY_SHIFT));

    //Apply movement
    Vector3 force = move * thrust * timeStep;
        rigidBody_->ApplyForce(force);

    //Update rotation according to direction of the player's movement.
        Vector3 velocity = rigidBody_->GetLinearVelocity();
        Vector3 lookDirection = velocity + 2.0f*fire;
        Quaternion rotation = rootNode_->GetWorldRotation();
        Quaternion aimRotation = rotation;
        aimRotation.FromLookRotation(lookDirection);
        rootNode_->SetRotation(rotation.Slerp(aimRotation, 7.0f * timeStep * velocity.Length()));

        //Update animation
        if (velocity.Length() > 0.1f){
            animCtrl_->PlayExclusive("Resources/Models/WalkRelax.ani", 0, true, 0.15f);
            animCtrl_->SetSpeed("Resources/Models/WalkRelax.ani", velocity.Length()*2.3f);
            animCtrl_->SetStartBone("Resources/Models/WalkRelax.ani", "MasterBone");
        }
        else {
            animCtrl_->PlayExclusive("Resources/Models/IdleRelax.ani", 0, true, 0.15f);
            animCtrl_->SetStartBone("Resources/Models/IdleRelax.ani", "MasterBone");
        }

        //Shooting
    sinceLastShot_ += timeStep;

    if (fire.Length() > 0.1f) fire.Normalize();
    else if (input->GetKeyDown(KEY_CTRL)) fire = rootNode_->GetDirection();
    else fire *= 0.0f;

    if (fire.Length() > 0.1f) {
        if (sinceLastShot_ > shotInterval_)
        {
            sinceLastShot_ = 0.0;
            Bullet* bullet = new Bullet(context_, masterControl_);
            bullet->rootNode_->SetPosition(rootNode_->GetPosition() + Vector3::UP * 0.5f);
            bullet->rootNode_->LookAt(rootNode_->GetPosition() + fire);
            bullet->rigidBody_->ApplyForce(fire*256.0f);
            Muzzle* muzzle = new Muzzle(context_, masterControl_, rootNode_->GetPosition() + Vector3::UP * 0.5f + 0.1f*fire);
            muzzle->rootNode_->LookAt(rootNode_->GetPosition() + fire);
            PlaySample(sample_);
        }
    }
}

