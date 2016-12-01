/* KO
// Copyright (C) 2016 LucKey Productions (luckeyproductions.nl)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// Commercial licenses are available through frode@lindeijer.nl
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

#ifndef CONTROLLABLE_H
#define CONTROLLABLE_H

#include <Urho3D/Urho3D.h>
#include <bitset>

#include "sceneobject.h"

#define INPUTMASTER GetSubsystem<InputMaster>()

class Controllable : public SceneObject
{
    friend class InputMaster;
    URHO3D_OBJECT(Controllable, SceneObject);
public:
    Controllable(Context* context);
    virtual void OnNodeSet(Node* node);
    virtual void Update(float timeStep);

    void SetMove(Vector3 move);
    void SetAim(Vector3 aim);
    virtual void ClearControl();

    virtual void EnterPlay() {}
    virtual void Think(float timeStep) {}

    Vector3 GetLinearVelocity() const { return rigidBody_->GetLinearVelocity(); }

    Player* GetPlayer();
protected:
    bool controlled_;
    Vector3 move_;
    Vector3 aim_;
    float thrust_;
    float maxSpeed_;
    float maxPitch_;
    float minPitch_;

    std::bitset<4> actions_;
    HashMap<int, float> actionSince_;
    HashMap<int, float> actionInterval_;

    AnimatedModel* model_;
    RigidBody* rigidBody_;
    CollisionShape* collisionShape_;
    AnimationController* animCtrl_;

    void ResetInput() { move_ = aim_ = Vector3::ZERO; actions_.reset(); }
    void SetActions(std::bitset<4> actions);
    void ClampPitch(Quaternion& rot);

    void AlignWithVelocity(float timeStep);
    void AlignWithMovement(float timeStep);

    virtual void HandleAction(int actionId) { (void)actionId; }
};

#endif // CONTROLLABLE_H
