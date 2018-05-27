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

#ifndef OGTATTCAM_H
#define OGTATTCAM_H

#include <Urho3D/Urho3D.h>

#include "mastercontrol.h"
#include "level.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
class Viewport;
class RenderPath;
class Camera;
}

using namespace Urho3D;

class OGTattCam : public LogicComponent
{
    URHO3D_OBJECT(OGTattCam, LogicComponent);
    friend class MasterControl;
    friend class InputMaster;
    friend class Player;
public:
    OGTattCam(Context *context);
    static void RegisterObject(Context *context);
    virtual void OnNodeSet(Node *node);
    virtual void Set(Vector3 position, int playerId);

    SharedPtr<Camera> camera_;
    SharedPtr<Viewport> viewport_;
    SharedPtr<RenderPath> effectRenderPath;

    Vector3 GetWorldPosition();
    Quaternion GetRotation();

    int GetPlayerId() const { return playerId_; }
private:
    void Update(float timeStep);
    SharedPtr<RigidBody> rigidBody_;

    int playerId_;
    float altitude_;
    float forceMultiplier;

    Vector3 smoothTargetPosition_;
    Vector3 smoothTargetVelocity_;
    float smoothTargetSpeed_;

    void SetupViewport();
    void Lock(SharedPtr<Node> node);
};

#endif
