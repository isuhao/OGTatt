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

#include "ogtattcam.h"
#include "player.h"

OGTattCam::OGTattCam():
    Object(MC->GetContext()),
    altitude_{23.0f},
    yaw_(0.0f), pitch_(88.0f), roll_{0.0f},
    yawDelta_{0.0}, pitchDelta_{0.0},
    forceMultiplier{1.0},
    smoothTargetPosition_{Vector3::ZERO},
    smoothTargetVelocity_{Vector3::ZERO}
{
    float viewRange{500.0f};
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(OGTattCam, HandleUpdate));

    //Create the camera. Limit far clip distance to match the fog
    rootNode_ = MC->world.scene->CreateChild("CamTrans");
    camera_ = rootNode_->CreateComponent<Camera>();
    camera_->SetFarClip(viewRange);
    camera_->SetNearClip(0.1f);
    camera_->SetFov(30.0f);

    Zone* zone = rootNode_->CreateComponent<Zone>();
    zone->SetBoundingBox(BoundingBox(Vector3(-100.0f, -50.0f, -100.0f), Vector3(100.0f, 50.0f, 100.0f)));
    zone->SetAmbientColor(Color(0.13f, 0.23f, 0.8f));
    zone->SetFogColor(Color(0.42f, 0.3f, 0.23f, 1.0f));
    zone->SetFogStart(23.0f);
    zone->SetFogEnd(viewRange);

    //Set an initial position for the camera scene node above the origin
    rootNode_->SetPosition(Vector3(0.0f, 23.0f, -0.5f));
    rootNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));
    rigidBody_ = rootNode_->CreateComponent<RigidBody>();
    rigidBody_->SetAngularDamping(10.0f);
    rigidBody_->SetLinearDamping(0.9f);
    rigidBody_->SetUseGravity(false);
//    CollisionShape* collisionShape = rootNode_->CreateComponent<CollisionShape>();
//    collisionShape->SetSphere(0.1f);
    rigidBody_->SetMass(1.0f);

    SetupViewport();
}

void OGTattCam::SetupViewport()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();

    //Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
    SharedPtr<Viewport> viewport(new Viewport(context_, MC->world.scene, camera_));

    //Add anti-asliasing and bloom
    effectRenderPath = viewport->GetRenderPath();
    effectRenderPath->Append(cache->GetResource<XMLFile>("PostProcess/FXAA3.xml"));
    effectRenderPath->SetEnabled("FXAA3", true);
//    effectRenderPath->Append(cache->GetResource<XMLFile>("PostProcess/Bloom.xml"));
//    effectRenderPath->SetShaderParameter("BloomThreshold", 0.666f);
//    effectRenderPath->SetShaderParameter("BloomMix", Vector2(0.88f, 0.9f));
//    effectRenderPath->SetEnabled("Bloom", true);

    renderer->SetViewport(0, viewport);
}

Vector3 OGTattCam::GetWorldPosition()
{
    return rootNode_->GetWorldPosition();
}

Quaternion OGTattCam::GetRotation()
{
    return rootNode_->GetRotation();
}

void OGTattCam::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    Vector3 targetPosition = MC->world.player_->GetWorldPosition();
    Vector3 targetVelocity = MC->world.player_->GetLinearVelocity();

    using namespace Update;

    //Take the frame time step, which is stored as a double
    float t{eventData[P_TIMESTEP].GetFloat()};
    //Movement speed as world units per second
    const float MOVE_SPEED{1024.0f};
    //Mouse sensitivity as degrees per pixel
    const float MOUSE_SENSITIVITY{0.1f};

    //Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees. Only move the camera when the cursor is hidden.
    Input* input{GetSubsystem<Input>()};
    IntVector2 mouseMove{input->GetMouseMove()};
    yawDelta_ = 0.5f * (yawDelta_ + MOUSE_SENSITIVITY * mouseMove.x_);
    pitchDelta_ = 0.5f * (pitchDelta_ + MOUSE_SENSITIVITY * mouseMove.y_);
    yaw_ += rootNode_->GetRotation().y_ + yawDelta_;
    pitch_ += rootNode_->GetRotation().x_ + pitchDelta_;
    pitch_ = Clamp(pitch_, -89.0f, 89.0f);
    //Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
    //rootNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));

    //Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
    Vector3 camForward{rootNode_->GetDirection()};
    camForward = LucKey::Scale(camForward, Vector3::ONE - Vector3::UP).Normalized();

    Vector3 camForce{};
//    if (input->GetKeyDown('T')) camForce += LucKey::Scale( rootNode_->GetDirection(), Vector3::ONE - Vector3::UP ).Normalized();
//    if (input->GetKeyDown('G')) camForce += LucKey::Scale( rootNode_->GetDirection(), -(Vector3::ONE - Vector3::UP) ).Normalized();
//    if (input->GetKeyDown('H')) camForce += LucKey::Scale( rootNode_->GetWorldRight(), Vector3::ONE - Vector3::UP ).Normalized() + centerForce;
//    if (input->GetKeyDown('F')) camForce += LucKey::Scale( rootNode_->GetWorldRight(), -(Vector3::ONE - Vector3::UP) ).Normalized() + centerForce;
//    if (input->GetKeyDown('Y')) camForce += Vector3::UP;
//    if (input->GetKeyDown('R') && rootNode_->GetPosition().y_ > 1.0f) camForce += Vector3::DOWN;

    if (input->GetKeyDown('Y')) altitude_ += (5.0f + (input->GetKeyDown(KEY_LSHIFT)||input->GetKeyDown(KEY_RSHIFT)) * 23.0f) * t;
    if (input->GetKeyDown('R')) altitude_ -= (5.0f + (input->GetKeyDown(KEY_LSHIFT)||input->GetKeyDown(KEY_RSHIFT)) * 23.0f) * t;

    //Read joystick input
    /*JoystickState* joystickState = input->GetJoystickByIndex(0);
    if (joystickState){
        rootNode_->RotateAround(targetPosition, Quaternion(-joystickState->GetAxisPosition(2) * timeStep * 0.1f*MOVE_SPEED, Vector3::UP), TS_WORLD);
        camForce -= joystickState->GetAxisPosition(3) * camForward * timeStep * MOVE_SPEED;
        if (joystickState->GetButtonDown(JB_R2)) camForce += Vector3::UP;
        if (joystickState->GetButtonDown(JB_L2) && rootNode_->GetPosition().y_ > 1.0f) camForce += Vector3::DOWN;

    }*/

    camForce = camForce.Normalized() * MOVE_SPEED;

    if ( forceMultiplier < 8.0f && (input->GetKeyDown(KEY_LSHIFT)||input->GetKeyDown(KEY_RSHIFT)) ){
        forceMultiplier += 0.23f;
    } else forceMultiplier = pow(forceMultiplier, 0.75f);
    rigidBody_->ApplyForce(forceMultiplier * camForce * t);

    //Prevent camera from going too low
    if (rootNode_->GetPosition().y_ < 1.5f)
    {
        rootNode_->SetPosition(Vector3(rootNode_->GetPosition().x_, 1.5f, rootNode_->GetPosition().z_));
        rigidBody_->SetLinearVelocity(Vector3(rigidBody_->GetLinearVelocity().x_, 0.0f, rigidBody_->GetLinearVelocity().z_));
    }

//    smoothTargetPosition_ = 0.1f * (9.0f * smoothTargetPosition_ + targetPosition);
    smoothTargetVelocity_ = 0.01f * (99.0f * smoothTargetVelocity_ + targetVelocity);
    rootNode_->SetPosition(Vector3(0.5f * (targetPosition.x_ + rootNode_->GetPosition().x_) + 0.5f * smoothTargetVelocity_.x_,
                                  0.5f * (targetPosition.y_ + altitude_ + 5.0f * smoothTargetVelocity_.Length()),
                                  0.5f * (targetPosition.z_ + rootNode_->GetPosition().z_) + 0.5f * smoothTargetVelocity_.z_ - 0.23f - 0.03f * smoothTargetVelocity_.Length()));
//    rootNode_->Translate(smoothTargetVelocity_ * timeStep, TS_WORLD);
    /*
    Quaternion camRot = rootNode_->GetWorldRotation();
    Quaternion aimRotation = camRot;
    aimRotation.FromLookRotation(smoothTargetPosition_ - rootNode_->GetWorldPosition());
    rootNode_->SetRotation(camRot.Slerp(aimRotation, 2.0f*timeStep));
    */

}

void OGTattCam::Lock(SharedPtr<Node> node)
{
}
