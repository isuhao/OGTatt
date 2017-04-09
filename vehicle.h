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

#ifndef VEHICLE_H
#define VEHICLE_H

#include <Urho3D/Urho3D.h>
#include <Bullet/BulletDynamics/Vehicle/btRaycastVehicle.h>

#include "controllable.h"

namespace Urho3D {
class Constraint;
class Node;
class RigidBody;
}

using namespace Urho3D;
//=============================================================================
//=============================================================================
const int CTRL_FORWARD = 1;
const int CTRL_BACK = 2;
const int CTRL_LEFT = 4;
const int CTRL_RIGHT = 8;
const float YAW_SENSITIVITY = 0.1f;
const float ENGINE_POWER = 1.0f;
const float DOWN_FORCE = 10.0f;
const float MAX_WHEEL_ANGLE = 22.5f;
//=============================================================================
// Vehicle component, responsible for physical movement according to controls.
//=============================================================================
class Vehicle : public Controllable
{
    URHO3D_OBJECT(Vehicle, Controllable);
public:
    Vehicle(Context *context);
    ~Vehicle();
    void Hit(float damage);
    virtual void OnNodeSet(Node *node);

    // Initialize the vehicle. Create rendering and physics components. Called by the application.
//    void Init();

    // Handle physics world update. Called by LogicComponent base class.
    virtual void FixedUpdate(float timeStep);
    virtual void PostUpdate(float timeStep);

    // Movement controls.
    Controls controls_;
protected:
    float steering_;
    // raycast vehicle
    btRaycastVehicle::btVehicleTuning  tuning_;
    btVehicleRaycaster*                vehicleRayCaster_;
    btRaycastVehicle*                  raycastVehicle_;

    Vector<Node*> wheelNodes_;

    float engineForce_;
    float breakingForce_;

    float maxEngineForce_;
    float maxBreakingForce_;

    float vehicleSteering_;
    float steeringIncrement_;
    float steeringClamp_;
    float wheelRadius_;
    float wheelWidth_;
    float wheelFriction_;
    float suspensionStiffness_;
    float suspensionDamping_;
    float suspensionCompression_;
    float rollInfluence_;
    float suspensionRestLength_;

    AnimatedModel* chassisModel_;
    CollisionShape* chassisCollisionShape_;
    Node* particleNode_;
    ParticleEmitter* flameEmitter_;

    Vector<Pair<SharedPtr<Node>,
                SharedPtr<Light>>> headLights_;

    Vector<Pair<SharedPtr<Node>,
                SharedPtr<Light>>> tailLights_;

    void SetupLights(int front, int rear, BoundingBox box);
private:
    void Destroy();

    void AddLight(Vector3 pos, bool head = true);
};

#endif // VEHICLE_H
