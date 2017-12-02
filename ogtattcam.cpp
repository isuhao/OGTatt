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
#include "inputmaster.h"
#include "player.h"

void OGTattCam::RegisterObject(Context *context)
{
    context->RegisterFactory<OGTattCam>();
}

OGTattCam::OGTattCam(Context* context):
    LogicComponent(context),
    altitude_{23.0f},
    smoothTargetPosition_{Vector3::ZERO},
    smoothTargetVelocity_{Vector3::ZERO}
{
}

void OGTattCam::OnNodeSet(Node *node)
{ if (!node) return;

    float viewRange{500.0f};

    //Create the camera. Limit far clip distance to match the fog
    node_ = MC->world.scene->CreateChild("CamTrans");
    node_->SetRotation(Quaternion(88.0f, 0.0f, 0.0f));
    camera_ = node_->CreateComponent<Camera>();
    camera_->SetFarClip(viewRange);
    camera_->SetNearClip(0.1f);
    camera_->SetFov(30.0f);

    Zone* zone{ node_->CreateComponent<Zone>() };
    zone->SetBoundingBox(BoundingBox(Vector3(-100.0f, -50.0f, -100.0f), Vector3(100.0f, 50.0f, 100.0f)));
    zone->SetAmbientColor(Color(0.13f, 0.23f, 0.8f));
    zone->SetFogColor(Color(0.42f, 0.3f, 0.23f, 1.0f));
    zone->SetFogStart(23.0f);
    zone->SetFogEnd(viewRange);

    SoundListener* listener{ node_->CreateComponent<SoundListener>() };
}

void OGTattCam::Set(Vector3 position, int playerId)
{
    playerId_ = playerId;

    SetupViewport();
}

void OGTattCam::SetupViewport()
{
    //Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
    RENDERER->SetNumViewports(2);
    SharedPtr<Viewport> viewport(new Viewport(context_, MC->world.scene, camera_));
//    viewport->SetRect(IntRect((playerId_ == 2) * (GRAPHICS->GetWidth()/2),
//                              0,
//                              (GRAPHICS->GetWidth()/2) + ((playerId_ == 2) * (GRAPHICS->GetWidth()/2)),
//                              GRAPHICS->GetHeight()));

    //Add anti-asliasing and bloom
    effectRenderPath = viewport->GetRenderPath();
    effectRenderPath->Append(CACHE->GetResource<XMLFile>("PostProcess/FXAA3.xml"));
    effectRenderPath->SetEnabled("FXAA3", true);
    effectRenderPath->Append(CACHE->GetResource<XMLFile>("PostProcess/BloomHDR.xml"));
    effectRenderPath->SetShaderParameter("BloomHDRThreshold", 0.8f);
    effectRenderPath->SetShaderParameter("BloomHDRMix", Vector2(0.88f, 0.5f));
    effectRenderPath->SetEnabled("BloomHDR", true);

    RENDERER->SetViewport(playerId_ - 1, viewport);
}

Vector3 OGTattCam::GetWorldPosition()
{
    return node_->GetWorldPosition();
}

Quaternion OGTattCam::GetRotation()
{
    return node_->GetRotation();
}

void OGTattCam::Update(float timeStep)
{
    if (!GetSubsystem<InputMaster>()->GetControllableByPlayer(playerId_))
        return;

    Vector3 targetPosition = GetSubsystem<InputMaster>()->GetControllableByPlayer(playerId_)->GetWorldPosition();
    Vector3 targetVelocity = GetSubsystem<InputMaster>()->GetControllableByPlayer(playerId_)->GetLinearVelocity();

    //Movement speed as world units per second
    const float MOVE_SPEED{1024.0f};
    //Mouse sensitivity as degrees per pixel
    const float MOUSE_SENSITIVITY{0.1f};

    //Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
    Vector3 camForward{ node_->GetDirection() };
    camForward = LucKey::Scale(camForward, Vector3::ONE - Vector3::UP).Normalized();

    Vector3 camForce{};
//    if (input->GetKeyDown('T')) camForce += LucKey::Scale( rootNode_->GetDirection(), Vector3::ONE - Vector3::UP ).Normalized();
//    if (input->GetKeyDown('G')) camForce += LucKey::Scale( rootNode_->GetDirection(), -(Vector3::ONE - Vector3::UP) ).Normalized();
//    if (input->GetKeyDown('H')) camForce += LucKey::Scale( rootNode_->GetWorldRight(), Vector3::ONE - Vector3::UP ).Normalized() + centerForce;
//    if (input->GetKeyDown('F')) camForce += LucKey::Scale( rootNode_->GetWorldRight(), -(Vector3::ONE - Vector3::UP) ).Normalized() + centerForce;
//    if (input->GetKeyDown('Y')) camForce += Vector3::UP;
//    if (input->GetKeyDown('R') && rootNode_->GetPosition().y_ > 1.0f) camForce += Vector3::DOWN;

    if (INPUT->GetKeyDown('R'))
        altitude_ += (5.0f + (INPUT->GetKeyDown(KEY_LSHIFT)||INPUT->GetKeyDown(KEY_RSHIFT)) * 23.0f) * timeStep;

    if (INPUT->GetKeyDown('Y'))
        altitude_ -= (5.0f + (INPUT->GetKeyDown(KEY_LSHIFT)||INPUT->GetKeyDown(KEY_RSHIFT)) * 23.0f) * timeStep;

    //Read joystick input
    /*JoystickState* joystickState = input->GetJoystickByIndex(0);
    if (joystickState){
        rootNode_->RotateAround(targetPosition, Quaternion(-joystickState->GetAxisPosition(2) * timeStep * 0.1f*MOVE_SPEED, Vector3::UP), TS_WORLD);
        camForce -= joystickState->GetAxisPosition(3) * camForward * timeStep * MOVE_SPEED;
        if (joystickState->GetButtonDown(JB_R2)) camForce += Vector3::UP;
        if (joystickState->GetButtonDown(JB_L2) && rootNode_->GetPosition().y_ > 1.0f) camForce += Vector3::DOWN;

    }*/

    camForce = camForce.Normalized() * MOVE_SPEED;

    if ( forceMultiplier < 8.0f && (INPUT->GetKeyDown(KEY_LSHIFT) || INPUT->GetKeyDown(KEY_RSHIFT)) ) {

        forceMultiplier += 0.23f;

    } else forceMultiplier = pow(forceMultiplier, 0.75f);
//    rigidBody_->ApplyForce(forceMultiplier * camForce * timeStep);

    //Prevent camera from going too low
    if (node_->GetPosition().y_ < 1.5f)
    {
        node_->SetPosition(Vector3(node_->GetPosition().x_, 1.5f, node_->GetPosition().z_));
//        rigidBody_->SetLinearVelocity(Vector3(rigidBody_->GetLinearVelocity().x_, 0.0f, rigidBody_->GetLinearVelocity().z_));
    }

//    smoothTargetPosition_ = 0.1f * (9.0f * smoothTargetPosition_ + targetPosition);
    smoothTargetVelocity_ = 0.01f * (99.0f * smoothTargetVelocity_ + targetVelocity);
    node_->SetPosition(Vector3(0.5f * (targetPosition.x_ + node_->GetPosition().x_) + 0.5f * smoothTargetVelocity_.x_,
                                  0.5f * (targetPosition.y_ + altitude_ + 10.0f * smoothTargetVelocity_.Length()),
                                  0.5f * (targetPosition.z_ + node_->GetPosition().z_) + 0.5f * smoothTargetVelocity_.z_ - 0.23f - 0.03f * smoothTargetVelocity_.Length()));
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
