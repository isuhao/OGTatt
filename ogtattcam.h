#pragma once

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

class OGTattCam : public Object
{
    OBJECT(OGTattCam);
    friend class MasterControl;
    friend class InputMaster;
    friend class Player;
public:
    OGTattCam(Context *context, MasterControl* masterControl);

    virtual void Start();
    virtual void Stop();

    SharedPtr<Camera> camera_;
    SharedPtr<Viewport> viewport_;
    SharedPtr<RenderPath> effectRenderPath;

    Vector3 GetWorldPosition();
    Quaternion GetRotation();
private:
    MasterControl* masterControl_;
    void HandleUpdate(StringHash eventType, VariantMap &eventData);
    SharedPtr<Node> rootNode_;
    Vector3 smoothTargetPosition_;
    Vector3 smoothTargetVelocity_;

    SharedPtr<RigidBody> rigidBody_;
    float yaw_;
    float pitch_;
    //double roll_ = 0.0;
    float yawDelta_ = 0.0;
    float pitchDelta_ = 0.0;
    float forceMultiplier = 1.0;
    void SetupViewport();
    void Lock(SharedPtr<Level> platform);
};
