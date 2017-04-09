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
#define CUBE_HALF_EXTENTS   0.5
#define DELETE_NULL(x)      { if (x) delete x; x = NULL; }
//=============================================================================
//=============================================================================
Vehicle::Vehicle(Context* context):
    SceneObject(context),
    steering_( 0.0f )
{
      // fixed update() for inputs and post update() to sync wheels for rendering
      SetUpdateEventMask( USE_FIXEDUPDATE | USE_POSTUPDATE | USE_UPDATE);
      m_fEngineForce = 0.0f;
  m_fBreakingForce = 0.0f;

  m_fmaxEngineForce = 250.f;//this should be engine/velocity dependent
  m_fmaxBreakingForce = 100.f;

  m_fVehicleSteering = 0.0f;
  m_fsteeringIncrement = 0.04f;
  m_fsteeringClamp = 0.3f;
  m_fwheelRadius = 0.1f;
  m_fwheelWidth = 0.13f;
  m_fwheelFriction = 1000;//BT_LARGE_FLOAT;
  m_fsuspensionStiffness = 14.0f;//20.f;
  m_fsuspensionDamping = 2.0f;//2.3f;
  m_fsuspensionCompression = 4.0f;//4.4f;
  m_frollInfluence = 0.01f;//1.0f;
  m_fsuspensionRestLength = 0.6f;//0.6

  m_vehicleRayCaster = NULL;
  m_vehicle = NULL;

  m_vpNodeWheel.Clear();
}

Vehicle::~Vehicle()
{
    DELETE_NULL( m_vehicleRayCaster );
    DELETE_NULL( m_vehicle );
    m_vpNodeWheel.Clear();
}

void Vehicle::OnNodeSet(Node *node)
{
//    node_->SetPosition(position);
//    node_->SetRotation(rotation);
//    chassisModel_ = node_->CreateComponent<AnimatedModel>();
//    chassisModel_->SetCastShadows(true);

//    chassisBody_ = node_->CreateComponent<RigidBody>();

    particleNode_ = node_->CreateChild("Fire");
    flameEmitter_ = particleNode_->CreateComponent<ParticleEmitter>();
    flameEmitter_->SetEffect(CACHE->GetResource<ParticleEffect>("Particles/fire1.xml"));
    flameEmitter_->SetEmitting(false);

//    decal_ = rootNode_->CreateComponent<DecalSet>();
//    decal_->SetMaterial(MC->GetMaterial("Decal"));
//    Quaternion decalRotation = rootNode_->GetRotation();
//    decalRotation  = decalRotation * Quaternion(90.0f, rootNode_->GetRight());
//    decal_->AddDecal(chassisModel_, rootNode_->GetWorldPosition()-0.23f*rootNode_->GetDirection(), decalRotation, 0.666f, 1.0f, 2.3f, Vector2::ZERO, Vector2::ONE);

    // This function is called only from the main program when initially creating the vehicle, not on scene load
    chassisModel_ = node_->CreateComponent<AnimatedModel>();
    chassisBody_ = node_->CreateComponent<RigidBody>();
    chassisCollisionShape_ = node_->CreateComponent<CollisionShape>();
//    chassisBody_->SetMass(800.0f);
    chassisBody_->SetLinearDamping(0.2f); // Some air resistance
    chassisBody_->SetAngularDamping(0.5f);
    chassisBody_->SetCollisionLayer(1);

    int rightIndex = 0;
    int upIndex = 1;
    int forwardIndex = 2;
    Scene* scene = GetScene();
    PhysicsWorld *pPhysWorld = scene->GetComponent<PhysicsWorld>();
    btDynamicsWorld *pbtDynWorld = (btDynamicsWorld*)pPhysWorld->GetWorld();

    m_vehicleRayCaster = new btDefaultVehicleRaycaster( pbtDynWorld );
    m_vehicle = new btRaycastVehicle( m_tuning, chassisBody_->GetBody(), m_vehicleRayCaster );
    pbtDynWorld->addVehicle( m_vehicle );

    m_vehicle->setCoordinateSystem( rightIndex, upIndex, forwardIndex );

//    node_->SetScale( Vector3(1.5f, 1.0f, 3.5f) );
    Vector3 v3BoxExtents = Vector3::ONE;//Vector3(1.5f, 1.0f, 3.0f);
    chassisCollisionShape_->SetBox( v3BoxExtents );

//    chassisModel_->SetModel(CACHE->GetResource<Model>("Models/Box.mdl"));
//    chassisModel_->SetMaterial(CACHE->GetResource<Material>("Materials/Stone.xml"));
    chassisModel_->SetCastShadows(true);

    float connectionHeight = 0.2f;//1.2f;
    bool isFrontWheel=true;
    btVector3 wheelDirectionCS0(0, -1, 0);
    btVector3 wheelAxleCS(-0.5, 0, 0);

    btVector3 connectionPointCS0(CUBE_HALF_EXTENTS - (0.3f * m_fwheelWidth), connectionHeight, 2 * CUBE_HALF_EXTENTS - m_fwheelRadius);
    m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, m_fsuspensionRestLength, m_fwheelRadius, m_tuning, isFrontWheel);

    connectionPointCS0 = btVector3(-CUBE_HALF_EXTENTS + (0.3f * m_fwheelWidth), connectionHeight, 2 * CUBE_HALF_EXTENTS - m_fwheelRadius);
    m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, m_fsuspensionRestLength, m_fwheelRadius, m_tuning, isFrontWheel);

    isFrontWheel = false;
    connectionPointCS0 = btVector3(-CUBE_HALF_EXTENTS + (0.3f * m_fwheelWidth), connectionHeight,-2 * CUBE_HALF_EXTENTS + m_fwheelRadius);
    m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, m_fsuspensionRestLength, m_fwheelRadius, m_tuning, isFrontWheel);

    connectionPointCS0 = btVector3(CUBE_HALF_EXTENTS - (0.3f * m_fwheelWidth), connectionHeight, -2 * CUBE_HALF_EXTENTS + m_fwheelRadius);
    m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, m_fsuspensionRestLength, m_fwheelRadius, m_tuning, isFrontWheel);

    for ( int i{0}; i < m_vehicle->getNumWheels(); i++ )
    {
        btWheelInfo& wheel{ m_vehicle->getWheelInfo( i ) };
        wheel.m_suspensionStiffness = m_fsuspensionStiffness;
        wheel.m_wheelsDampingRelaxation = m_fsuspensionDamping;
        wheel.m_wheelsDampingCompression = m_fsuspensionCompression;
        wheel.m_frictionSlip = m_fwheelFriction;
        wheel.m_rollInfluence = m_frollInfluence;
    }

    if ( m_vehicle )
    {
        m_vehicle->resetSuspension();

        for ( int i{0}; i < m_vehicle->getNumWheels(); i++ )
        {
            //synchronize the wheels with the (interpolated) chassis worldtransform
            m_vehicle->updateWheelTransform(i,true);

            btTransform transform = m_vehicle->getWheelTransformWS( i );
            Vector3 v3Origin( transform.getOrigin() );
            Quaternion( transform.getRotation() );

            // create wheel node
            Node *wheelNode = GetScene()->CreateChild();
            m_vpNodeWheel.Push( wheelNode );

            wheelNode->SetPosition( v3Origin );
            btWheelInfo whInfo = m_vehicle->getWheelInfo( i );
            Vector3 v3PosLS( whInfo.m_chassisConnectionPointCS );

            wheelNode->SetRotation( v3PosLS.x_ >= 0.0 ? Quaternion(0.0f, 0.0f, -90.0f) : Quaternion(0.0f, 0.0f, 90.0f) );
            wheelNode->SetScale(Vector3(1.0f, 0.65f, 1.0f));

            StaticModel *pWheel = wheelNode->CreateComponent<StaticModel>();
            pWheel->SetModel(CACHE->GetResource<Model>("Models/Cylinder.mdl"));
            pWheel->SetMaterial(CACHE->GetResource<Material>("Materials/Stone.xml"));
            pWheel->SetCastShadows(true);
        }
    }
}
//=============================================================================
//=============================================================================
void Vehicle::FixedUpdate(float timeStep)
{
    controls_.Set(CTRL_FORWARD, !Random(5));
    controls_.Set(CTRL_LEFT,    !Random(5));
    controls_.Set(CTRL_RIGHT,   !Random(5));
    controls_.Set(CTRL_BACK,    !Random(5));

    float newSteering = 0.0f;
    float accelerator = 0.0f;
    // Read controls
    if (controls_.buttons_ & CTRL_LEFT)
        newSteering = -1.0f;
    if (controls_.buttons_ & CTRL_RIGHT)
        newSteering = 1.0f;
    if (controls_.buttons_ & CTRL_FORWARD)
        accelerator = 1.0f;
    if (controls_.buttons_ & CTRL_BACK)
        accelerator = -0.5f;

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
    m_fVehicleSteering = steering_;
    int wheelIndex = 0;
    m_vehicle->setSteeringValue(m_fVehicleSteering,wheelIndex);
    wheelIndex = 1;
    m_vehicle->setSteeringValue(m_fVehicleSteering,wheelIndex);

    if ( newSteering != 0.0f || accelerator != 0.0f )
    {
        chassisBody_->Activate();
    }

    // apply forces
    m_fEngineForce = m_fmaxEngineForce * accelerator;
    m_fBreakingForce = 0.f;

    // 2x wheel drive
    for ( int i{2}; i < 4; ++i )
    {
        m_vehicle->applyEngineForce( m_fEngineForce, i );
//        m_vehicle->setBrake( m_fBreakingForce, i );
    }
}
//=============================================================================
// sync wheels for rendering
//=============================================================================
void Vehicle::PostUpdate(float )
{
    for ( int i{0}; i < m_vehicle->getNumWheels(); i++ )
    {
        m_vehicle->updateWheelTransform( i, true );
        btTransform transform = m_vehicle->getWheelTransformWS( i );
        Vector3 v3Origin( transform.getOrigin() );
        Quaternion qRot( transform.getRotation() );

        Node *pWheel = m_vpNodeWheel[ i ];
        pWheel->SetPosition( v3Origin );

        btWheelInfo whInfo = m_vehicle->getWheelInfo( i );
        Vector3 v3PosLS( whInfo.m_chassisConnectionPointCS );
        Quaternion qRotator = ( v3PosLS.x_ >= 0.0 ? Quaternion(0.0f, 0.0f, -90.0f) : Quaternion(0.0f, 0.0f, 90.0f) );
        pWheel->SetRotation( qRot * qRotator );
    }
}

void Vehicle::Hit(float damage)
{
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

void Vehicle::Destroy()
{
//    new Explosion(node_->GetPosition(), 1.0f);
    for (unsigned i{0}; i < chassisModel_->GetNumGeometries(); ++i){
        chassisModel_->SetMaterial(i, MC->GetMaterial("Darkness"));
    }
    flameEmitter_->SetEmitting(true);
}
