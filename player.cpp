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

Player::Player():
    Character(Vector3::ZERO)
{
    rootNode_->SetName("Player");

    rigidBody_->SetAngularDamping(1.0f);

    shot_sfx = MC->cache_->GetResource<Sound>("Samples/Shot.ogg");
    shot_sfx->SetLooped(false);

    /*scoreText_ = MC->ui_->GetRoot()->CreateChild<Text>();
    scoreText_->SetText(String(score_));
    scoreText_->SetFont(MC->cache_->GetResource<Font>("Fonts/skirmishergrad.ttf"), 32);
    scoreText_->SetHorizontalAlignment(HA_LEFT);
    scoreText_->SetVerticalAlignment(VA_TOP);
    scoreText_->SetPosition(0, MC->ui_->GetRoot()->GetHeight()/2.1);*/

    SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(Player, HandleSceneUpdate));
}

void Player::AddScore(int points)
{
    score_ += points;
}

void Player::HandleSceneUpdate(StringHash eventType, VariantMap &eventData)
{
    //Take the frame time step, which is stored as a double
    double timeStep = eventData[SceneUpdate::P_TIMESTEP].GetFloat();

    Input* input = GetSubsystem<Input>();

    //Orientation vectors
    Vector3 camRight = MC->world.camera->rootNode_->GetRight();
    Vector3 camForward = MC->world.camera->rootNode_->GetDirection();
    camRight = LucKey::Scale(camRight, Vector3::ONE - Vector3::UP).Normalized();
    camForward = LucKey::Scale(camForward, Vector3::ONE - Vector3::UP).Normalized();
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
    moveKey = -camRight * (input->GetKeyDown(KEY_LEFT) || input->GetKeyDown(KEY_A)) +
            camRight * (input->GetKeyDown(KEY_RIGHT) || input->GetKeyDown(KEY_D)) +
            camForward * (input->GetKeyDown(KEY_UP) || input->GetKeyDown(KEY_W)) +
            -camForward * (input->GetKeyDown(KEY_DOWN) || input->GetKeyDown(KEY_S));

    fireKey = -camRight * (input->GetKeyDown(KEY_J) || input->GetKeyDown(KEY_KP_4)) +
            camRight * (input->GetKeyDown(KEY_L) || input->GetKeyDown(KEY_KP_6)) +
            camForward * (input->GetKeyDown(KEY_I) || input->GetKeyDown(KEY_KP_8)) +
            -camForward * (input->GetKeyDown(KEY_K) || input->GetKeyDown(KEY_KP_2) || input->GetKeyDown(KEY_KP_5)) +

            Quaternion(45.0f, Vector3::UP)*-camRight * input->GetKeyDown(KEY_KP_7) +
            Quaternion(45.0f, Vector3::UP)* camRight * input->GetKeyDown(KEY_KP_3) +
            Quaternion(45.0f, Vector3::UP)* camForward * input->GetKeyDown(KEY_KP_9) +
            Quaternion(45.0f, Vector3::UP)*-camForward * input->GetKeyDown(KEY_KP_1);

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
        animCtrl_->PlayExclusive("Models/WalkRelax.ani", 0, true, 0.15f);
        animCtrl_->SetSpeed("Models/WalkRelax.ani", velocity.Length()*2.3f);
        animCtrl_->SetStartBone("Models/WalkRelax.ani", "MasterBone");
    }
    else {
        animCtrl_->PlayExclusive("Models/IdleRelax.ani", 0, true, 0.15f);
        animCtrl_->SetStartBone("Models/IdleRelax.ani", "MasterBone");
    }

    //Shooting
    float bulletHeight{0.42f};

    sinceLastShot_ += timeStep;

    if (fire.Length() > 0.1f) fire.Normalize();
    else if (input->GetKeyDown(KEY_CTRL)) fire = rootNode_->GetDirection();
    else fire *= 0.0f;

    if (fire.Length() > 0.1f) {
        if (sinceLastShot_ > shotInterval_)
        {
            sinceLastShot_ = 0.0;
            Bullet* bullet = new Bullet();
            bullet->rootNode_->SetPosition(rootNode_->GetPosition() + Vector3::UP * bulletHeight);
            bullet->rootNode_->LookAt(rootNode_->GetPosition() + fire);
            bullet->rigidBody_->ApplyForce((Vector3(Random(-0.01f, 0.01f),
                                                    Random(-0.01f, 0.01f),
                                                    Random(-0.01f, 0.01f))
                                            + fire) * 256.0f);
            Muzzle* muzzle = new Muzzle(rootNode_->GetPosition() + Vector3::UP * bulletHeight + 0.1f * fire);
            muzzle->rootNode_->LookAt(rootNode_->GetPosition() + fire);
            PlaySample(shot_sfx);
        }
    }
}

