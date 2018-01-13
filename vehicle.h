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
#include "animatedbillboardset.h"
//#include <Bullet/BulletDynamics/Vehicle/btRaycastVehicle.h>

#include "controllable.h"

namespace Urho3D {
class Constraint;
class Node;
class RigidBody;
}

enum VehicleActions{ NITRO, HANDBRAKE, HORN };

using namespace Urho3D;

class Vehicle : public Controllable
{
    URHO3D_OBJECT(Vehicle, Controllable);
public:
    Vehicle(Context *context);
    void Hit(float damage);
    virtual void OnNodeSet(Node *node);

    virtual void Update(float timeStep);
    virtual void FixedUpdate(float timeStep);

    void SetLightsEnabled(bool enabled);
protected:
    HashSet<int> steerWheels_;
    HashSet<int> driveWheels_;
    HashSet<int> brakeWheels_;

    float initialDurability_;
    float durability_;
    bool functional_;

    RaycastVehicle* raycastVehicle_;

    float steering_;
    float engineForce_;
    float brakingForce_;

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

    SharedPtr<Material> decalMaterial_;
    Node* particleNode_;
    ParticleEmitter* flameEmitter_;
    AnimatedBillboardSet* flames_;

    Vector<Pair<SharedPtr<Node>,
                SharedPtr<Light>>> headLights_;

    Vector<Pair<SharedPtr<Node>,
                SharedPtr<Light>>> tailLights_;

    void SetupLights(int front, int rear, BoundingBox box);
    void Destroy();
private:

    void AddLight(Vector3 pos, bool head = true);
};

#endif // VEHICLE_H
