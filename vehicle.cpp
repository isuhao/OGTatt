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

#include <Bullet/BulletDynamics/Dynamics/btDynamicsWorld.h>
#include "explosion.h"

#include "vehicle.h"
//=============================================================================
//=============================================================================
#define CUBE_HALF_EXTENTS   0.42
#define DELETE_NULL(x)      { if (x) delete x; x = NULL; }
//=============================================================================
//=============================================================================
Vehicle::Vehicle(Context* context):
    Controllable(context),
    steering_{0.0f},
    functional_{true}
{
    SetUpdateEventMask( USE_FIXEDUPDATE | USE_POSTUPDATE);
    engineForce_ = 0.0f;
    breakingForce_ = 0.0f;

    maxEngineForce_ = 235.f;//this should be engine/velocity dependent
    maxBreakingForce_ = 100.f;

    vehicleSteering_ = 0.0f;
    steeringIncrement_ = 0.04f;
    steeringClamp_ = 0.3f;
    wheelRadius_ = 0.2f;
    wheelWidth_ = 0.13f;
    wheelFriction_ = 2.0f;//BT_LARGE_FLOAT;
    suspensionStiffness_ = 23.0f;//20.f;
    suspensionDamping_ = 2.0f;//2.3f;
    suspensionCompression_ = 2.0f;//4.4f;
    rollInfluence_ = 0.01f;//1.0f;
    suspensionRestLength_ = 0.42f;//0.6

    vehicleRayCaster_ = NULL;
    raycastVehicle_ = NULL;

    wheelNodes_.Clear();
}

Vehicle::~Vehicle()
{
    DELETE_NULL( vehicleRayCaster_ );
    DELETE_NULL( raycastVehicle_ );
    wheelNodes_.Clear();
}

void Vehicle::OnNodeSet(Node *node)
{
    particleNode_ = node_->CreateChild("Fire");
    flameEmitter_ = particleNode_->CreateComponent<ParticleEmitter>();
    flameEmitter_->SetEffect(CACHE->GetResource<ParticleEffect>("Particles/fire1.xml"));
    flameEmitter_->SetEmitting(false);

    chassisModel_ = node_->CreateComponent<AnimatedModel>();
    chassisModel_->SetCastShadows(true);

    rigidBody_ = node_->CreateComponent<RigidBody>();
    chassisCollisionShape_ = node_->CreateComponent<CollisionShape>();

    rigidBody_->SetFriction(0.42f);
    rigidBody_->SetLinearDamping(0.2f);
    rigidBody_->SetAngularDamping(0.1f);
    rigidBody_->SetCollisionLayer(1);

    int rightIndex = 0;
    int upIndex = 1;
    int forwardIndex = 2;
    PhysicsWorld *pPhysWorld = MC->world.scene->GetComponent<PhysicsWorld>();
    btDynamicsWorld *pbtDynWorld = reinterpret_cast<btDynamicsWorld*>(pPhysWorld->GetWorld());

    vehicleRayCaster_ = new btDefaultVehicleRaycaster( pbtDynWorld );
    raycastVehicle_ = new btRaycastVehicle( tuning_, rigidBody_->GetBody(), vehicleRayCaster_ );
    pbtDynWorld->addVehicle( raycastVehicle_ );

    raycastVehicle_->setCoordinateSystem( rightIndex, upIndex, forwardIndex );

    float connectionHeight = 0.23f;//1.2f;
    bool isFrontWheel=true;
    btVector3 wheelDirectionCS0(0, -1, 0);
    btVector3 wheelAxleCS(-0.5, 0, 0);

    btVector3 connectionPointCS0(CUBE_HALF_EXTENTS - (0.3f * wheelWidth_), connectionHeight, 2 * CUBE_HALF_EXTENTS - wheelRadius_);
    raycastVehicle_->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength_, wheelRadius_, tuning_, isFrontWheel);

    connectionPointCS0 = btVector3(-CUBE_HALF_EXTENTS + (0.3f * wheelWidth_), connectionHeight, 2 * CUBE_HALF_EXTENTS - wheelRadius_);
    raycastVehicle_->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength_, wheelRadius_, tuning_, isFrontWheel);

    isFrontWheel = false;
    connectionPointCS0 = btVector3(-CUBE_HALF_EXTENTS + (0.3f * wheelWidth_), connectionHeight,-2 * CUBE_HALF_EXTENTS + wheelRadius_);
    raycastVehicle_->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength_, wheelRadius_, tuning_, isFrontWheel);

    connectionPointCS0 = btVector3(CUBE_HALF_EXTENTS - (0.3f * wheelWidth_), connectionHeight, -2 * CUBE_HALF_EXTENTS + wheelRadius_);
    raycastVehicle_->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength_, wheelRadius_, tuning_, isFrontWheel);

    for ( int i{0}; i < raycastVehicle_->getNumWheels(); i++ )
    {
        btWheelInfo& wheel{ raycastVehicle_->getWheelInfo( i ) };
        wheel.m_suspensionStiffness = suspensionStiffness_;
        wheel.m_wheelsDampingRelaxation = suspensionDamping_;
        wheel.m_wheelsDampingCompression = suspensionCompression_;
        wheel.m_frictionSlip = wheelFriction_;
        wheel.m_rollInfluence = rollInfluence_;
    }

    if ( raycastVehicle_ )
    {
        raycastVehicle_->resetSuspension();

        for ( int i{0}; i < raycastVehicle_->getNumWheels(); i++ )
        {
            //synchronize the wheels with the (interpolated) chassis worldtransform
            raycastVehicle_->updateWheelTransform(i,true);

            btTransform transform = raycastVehicle_->getWheelTransformWS( i );
            Vector3 v3Origin( transform.getOrigin() );
            Quaternion( transform.getRotation() );

            // create wheel node
            Node *wheelNode = GetScene()->CreateChild();
            wheelNodes_.Push( wheelNode );

            wheelNode->SetPosition( v3Origin );
            btWheelInfo whInfo = raycastVehicle_->getWheelInfo( i );
            Vector3 v3PosLS( whInfo.m_chassisConnectionPointCS);

            wheelNode->SetRotation( v3PosLS.x_ >= 0.0 ? Quaternion(0.0f, 0.0f, -90.0f) : Quaternion(0.0f, 0.0f, 90.0f) );
//            wheelNode->SetScale(5.0f);

            StaticModel *pWheel = wheelNode->CreateComponent<StaticModel>();
            pWheel->SetModel(CACHE->GetResource<Model>("Models/Wheel.mdl"));
            pWheel->SetMaterial(CACHE->GetResource<Material>("Materials/Asphalt.xml"));
//            pWheel->SetCastShadows(true);
        }
    }
}

void Vehicle::FixedUpdate(float timeStep)
{

//    controls_.Set(CTRL_FORWARD, move_.z_ > 0.0f);
//    controls_.Set(CTRL_LEFT,    move_.x_ < 0.0f);
//    controls_.Set(CTRL_RIGHT,   move_.x_ > 0.0f);
//    controls_.Set(CTRL_BACK,    move_.z_ < 0.0f);

    float newSteering = move_.x_ * Pow(0.9f, rigidBody_->GetLinearVelocity().Length());
    float accelerator = move_.z_ > 0.0f ? move_.z_
                                        : move_.z_ * 0.666f;
    if (!functional_)
        newSteering = accelerator = 0.0f;

    // Read controls
//    if (controls_.buttons_ & CTRL_LEFT)
//        newSteering = -1.0f;
//    if (controls_.buttons_ & CTRL_RIGHT)
//        newSteering = 1.0f;
//    if (controls_.buttons_ & CTRL_FORWARD){
//        accelerator = 1.0f;
//        newSteering *= 0.5f;
//    } else if (controls_.buttons_ & CTRL_BACK)
//        accelerator = -0.5f;

    // When steering, wake up the wheel rigidbodies so that their orientation is updated
    if ( newSteering != 0.0f )
    {
        steering_ = steering_ * 0.95f + newSteering * 0.05f;
    }
    else
    {
        steering_ = steering_ * 0.8f + newSteering * 0.2f;
    }

    // Set front wheel angles
    vehicleSteering_ = steering_;
    for (int wheelIndex : { 0, 1 }){

        raycastVehicle_->setSteeringValue(vehicleSteering_, wheelIndex);
    }

    if ( newSteering != 0.0f || accelerator != 0.0f )
    {
        rigidBody_->Activate();
    }

    // apply forces
    breakingForce_ = 42.0f * actions_[HANDBREAK];
    engineForce_ = breakingForce_ == 0.0f ? maxEngineForce_ * accelerator
                                          : 0.0f;

    // 2x wheel drive
    for ( int wheelIndex : { 2, 3 } )
    {
            raycastVehicle_->applyEngineForce( engineForce_, wheelIndex );
            raycastVehicle_->setBrake( breakingForce_, wheelIndex );
    }
}
//=============================================================================
// sync wheels for rendering
//=============================================================================
void Vehicle::PostUpdate(float )
{/* Freaky wheels

    for ( int i{0}; i < raycastVehicle_->getNumWheels(); ++i )
    {
        raycastVehicle_->updateWheelTransform(i, true);
        btTransform transform = raycastVehicle_->getWheelTransformWS(i);
        Vector3 v3Origin{ ToVector3(transform.getOrigin()) };
        Quaternion qRot{ ToQuaternion(transform.getRotation()) };

        Node* pWheel{ wheelNodes_[i] };
        pWheel->SetPosition( v3Origin );

        btWheelInfo whInfo = raycastVehicle_->getWheelInfo( i );
        Vector3 v3PosLS( whInfo.m_chassisConnectionPointCS );
        Quaternion qRotator = ( v3PosLS.x_ >= 0.0 ? Quaternion(0.0f, 0.0f, -90.0f) : Quaternion(0.0f, 0.0f, 90.0f) );
        pWheel->SetRotation( qRot * qRotator );
//        pWheel->SetPosition(v3PosLS);
    }
    */
}

void Vehicle::SetupLights(int front, int rear, BoundingBox box)
{
    if (front) {
        for (int f{0}; f < front; ++f){
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
        for (int r{0}; r < rear; ++r){
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
    durability_ -= damage;

    if (durability_ < initialDurability_ * 0.25f)
        flameEmitter_->SetEmitting(true);

    if (durability_ <= 0.0f)
        Destroy();
}

void Vehicle::Destroy()
{
//    new Explosion(node_->GetPosition(), 1.0f);
    for (unsigned i{0}; i < chassisModel_->GetNumGeometries(); ++i){
        chassisModel_->SetMaterial(i, MC->GetMaterial("Darkness"));
    }
    flameEmitter_->SetEmitting(true);

    functional_ = false;
    SetLightsEnabled(false);
}
