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

#include "explosion.h"
#include "player.h"
#include "spawnmaster.h"

#include "vehicle.h"

Vehicle::Vehicle(Context* context):
    Controllable(context),
    raycastVehicle_{},
    steering_{0.0f},
    functional_{true}
{
    SetUpdateEventMask( USE_FIXEDUPDATE | USE_POSTUPDATE);
    engineForce_ = 0.0f;
    brakingForce_ = 0.0f;

    maxEngineForce_ = 350.f;//this should be engine/velocity dependent
    maxBreakingForce_ = 100.f;

    vehicleSteering_ = 0.0f;
    steeringIncrement_ = 0.04f;
    steeringClamp_ = 0.3f;
    wheelRadius_ = 0.2f;
    wheelWidth_ = 0.13f;
    wheelFriction_ = 2.0f;//BT_LARGE_FLOAT;
    suspensionStiffness_ = 42.0f;//20.f;
    suspensionDamping_ = 2.3f;//2.3f;
    suspensionCompression_ = 0.34f;//4.4f;
    rollInfluence_ = 0.01f;//1.0f;
    suspensionRestLength_ = 0.42f;//0.6

}

void Vehicle::OnNodeSet(Node *node)
{ if (!node) return;

    Controllable::OnNodeSet(node);

    particleNode_ = node_->CreateChild("Fire");
    flameEmitter_ = particleNode_->CreateComponent<ParticleEmitter>();
    flameEmitter_->SetEffect(CACHE->GetResource<ParticleEffect>("Particles/HoodFire.xml"));
    flameEmitter_->SetEmitting(false);

    decalMaterial_ = MC->GetMaterial("Decal")->Clone();

    collisionShape_ = node_->CreateComponent<CollisionShape>();

    rigidBody_->SetFriction(0.42f);
    rigidBody_->SetLinearDamping(0.2f);
    rigidBody_->SetAngularDamping(0.1f);
    rigidBody_->SetLinearRestThreshold(0.01f);
    rigidBody_->SetAngularRestThreshold(0.1f);
    rigidBody_->SetCollisionLayer(1);

    raycastVehicle_ = node_->CreateComponent<RaycastVehicle>();
    raycastVehicle_->Init();

    steerWheels_.Insert(0); steerWheels_.Insert(1);
    driveWheels_.Insert(2); driveWheels_.Insert(3);

    for (int w{0}; w < 4; ++w) {

        float x = (w % 2) ? -0.44f : 0.44f;
        float y = 0.18f;
        float z = (w / 2) ? -0.66f : 0.6f;

        Node* wheelNode{ node_->CreateChild("Wheel") };
        wheelNode->SetPosition(Vector3(x, y, z));
        wheelNode->SetRotation(x < 0.0 ? Quaternion(0.0f, 0.0f, 0.0f) : Quaternion(0.0f, 0.0f, 180.0f));
        raycastVehicle_->AddWheel(wheelNode,
                                  Vector3(0.0f, -1.0f, 0.0f),
                                  Vector3(-1.0f, 0.0f, 0.0f),
                                  suspensionRestLength_, wheelRadius_, z > 0.0f);

        raycastVehicle_->SetWheelSuspensionStiffness(w, suspensionStiffness_);
        raycastVehicle_->SetWheelDampingRelaxation(w, suspensionDamping_);
        raycastVehicle_->SetWheelDampingCompression(w, suspensionCompression_);
        raycastVehicle_->SetWheelFrictionSlip(w, wheelFriction_);
        raycastVehicle_->SetWheelRollInfluence(w, rollInfluence_);

        StaticModel* pWheel{ wheelNode->CreateComponent<StaticModel>() };
        pWheel->SetModel(CACHE->GetResource<Model>("Models/Wheel.mdl"));
        pWheel->SetMaterial(CACHE->GetResource<Material>("Materials/Asphalt.xml"));
        pWheel->SetCastShadows(true);
    }
    raycastVehicle_->ResetWheels();
}

void Vehicle::FixedUpdate(float timeStep)
{
    float newSteering{};
    float accelerator{};

    newSteering = move_.x_ * Pow(0.9f, rigidBody_->GetLinearVelocity().Length());
    accelerator = move_.z_ > 0.0f ? move_.z_
                                  : move_.z_ * 0.666f;

    if (Player* p{ GetPlayer() }) {

        if (JoystickState* joystick{ INPUT->GetJoystick(p->GetPlayerId() - 1) }) {

            accelerator = Clamp(joystick->GetAxisPosition(13), 0.0f, 1.0f) - Clamp(joystick->GetAxisPosition(12) * 0.5f, 0.0f, 1.0f);
        }
    }

    if (!functional_)
        newSteering = accelerator = 0.0f;

    // When steering, wake up the wheel rigidbodies so that their orientation is updated
    if ( newSteering != 0.0f ) {

        steering_ = steering_ * 0.95f + newSteering * 0.05f;

    } else {

        steering_ = steering_ * 0.8f + newSteering * 0.2f;
    }

    // Set front wheel angles
    vehicleSteering_ = steering_;

    if ( newSteering != 0.0f || accelerator != 0.0f )
    {
        rigidBody_->Activate();
    }

    // apply forces
    float handbrakingForce_ = 23.0f * actions_[HANDBREAK];
    bool brake = Sign(accelerator) == 0.0f ? 0.0f
                                           : Sign(accelerator) * (rigidBody_->GetRotation().Inverse() * rigidBody_->GetLinearVelocity()).z_ < -0.1f;
    brakingForce_ = brake ? 10.0f
                          : 0.0f;
    engineForce_ = brakingForce_ == 0.0f ? maxEngineForce_ * accelerator
                                         : 0.0f;

    for ( int wheelIndex{0}; wheelIndex < raycastVehicle_->GetNumWheels(); ++wheelIndex) {

        float brakingForce{ brakingForce_ };
        brakingForce *= raycastVehicle_->GetWheelSkidInfoCumulative(wheelIndex) > 0.9f;

        if (steerWheels_.Contains(wheelIndex)) {

            raycastVehicle_->SetSteeringValue(wheelIndex, vehicleSteering_);
        }
        if (driveWheels_.Contains(wheelIndex)) {

            raycastVehicle_->SetEngineForce(wheelIndex, engineForce_);
            brakingForce += handbrakingForce_;
        }


        raycastVehicle_->SetBrake(wheelIndex, brakingForce);
    }
}

void Vehicle::SetupLights(int front, int rear, BoundingBox box)
{
    if (front) {
        for (int f{0}; f < front; ++f) {

            Pair<SharedPtr<Node>, SharedPtr<Light>> nodeLightPair;
            nodeLightPair.first_ = node_->CreateChild("HeadLight");
            nodeLightPair.first_->SetDirection(Vector3(0.0f, -0.23f, 0.666f));

            if (front == 1) {

                nodeLightPair.first_->SetPosition(Vector3(0.5f * (box.min_.x_ + box.max_.x_),
                                                  box.min_.y_,
                                                  box.max_.z_));
            } else {

                nodeLightPair.first_->SetPosition(Vector3(box.min_.x_ + f * (box.Size().x_ / (front - 1)),
                                                  box.min_.y_,
                                                  box.max_.z_));
            }

            nodeLightPair.second_ = nodeLightPair.first_->CreateComponent<Light>();
            nodeLightPair.second_->SetLightType(LIGHT_SPOT);
            nodeLightPair.second_->SetColor(Color(1.0f, 0.9f, 0.8f));
            nodeLightPair.second_->SetRange(8.0f);
            nodeLightPair.second_->SetFov(60.0f);
            nodeLightPair.second_->SetBrightness(5.0f);
            nodeLightPair.second_->SetCastShadows(true);
            nodeLightPair.second_->SetShadowResolution(0.25f);

            headLights_.Push(nodeLightPair);
        }
    }
    if (rear) {
        for (int r{0}; r < rear; ++r) {

            Pair<SharedPtr<Node>, SharedPtr<Light>> light;
            light.first_ = node_->CreateChild("TailLight");
            light.first_->SetDirection(Vector3(0.0f, -0.6f, -0.5f));

            if (front == 1) {

                light.first_->SetPosition(Vector3(0.5f * (box.min_.x_ + box.max_.x_),
                                                  box.max_.y_,
                                                  box.min_.z_));
            } else {

                light.first_->SetPosition(Vector3(box.min_.x_ + r * (box.Size().x_ / (rear - 1)),
                                                  box.max_.y_,
                                                  box.min_.z_));
                light.first_->Rotate(Quaternion(30.0f - r * (60.0f / (rear - 1)), Vector3::UP), TS_WORLD);
            }

            light.second_ = light.first_->CreateComponent<Light>();
            light.second_->SetLightType(LIGHT_SPOT);
            light.second_->SetColor(Color::RED);
            light.second_->SetRange(3.0f);
            light.second_->SetFov(120.0f);
            light.second_->SetBrightness(2.0f);
            light.second_->SetCastShadows(true);
            light.second_->SetShadowResolution(0.25f);

            tailLights_.Push(light);
        }
    }
}

void Vehicle::SetLightsEnabled(bool enabled)
{
    for (Pair<SharedPtr<Node>, SharedPtr<Light>> headLight : headLights_) {
        headLight.second_->SetEnabled(enabled);
    }

    for (Pair<SharedPtr<Node>, SharedPtr<Light>> tailLight : tailLights_) {
        tailLight.second_->SetEnabled(enabled);
    }
}

void Vehicle::Hit(float damage)
{
    if (durability_ == 0.0f)
        return;

    durability_ -= damage;

    if (durability_ < initialDurability_ * 0.25f)
        flameEmitter_->SetEmitting(true);

    if (durability_ <= 0.0f) {
        Destroy();
    }
}

void Vehicle::Destroy()
{
    SPAWN->Create<Explosion>()->Set(node_->GetPosition() + Vector3::UP * model_->GetBoundingBox().Size().y_);

    model_->SetShadowMask(2);

    for (unsigned i{0}; i < model_->GetNumGeometries(); ++i){
        model_->SetMaterial(i, MC->GetMaterial("Darkness"));
    }
    flameEmitter_->SetEmitting(true);
    SetLightsEnabled(false);

    durability_ = 0.0f;
    functional_ = false;
}
