/* OG Tatt
// Copyright (C) 2015 LucKey Productions (luckeyproductions.nl)
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

#ifndef MASTERCONTROL_H
#define MASTERCONTROL_H

#include <Urho3D/Urho3D.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource.h>
#include <Urho3D/Container/HashMap.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/OctreeQuery.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/RenderPath.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Math/MathDefs.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/Resource.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>


#include "helper.h"

#include <Urho3D/DebugNew.h> // always (!) should be last

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class OGTattCam;
class InputMaster;
class Level;
class Player;

typedef struct GameWorld
{
    SharedPtr<OGTattCam> camera;
    SharedPtr<Scene> scene;
    SharedPtr<Node> voidNode;
    SharedPtr<Player> player_;
    struct {
        SharedPtr<Node> sceneCursor;
        SharedPtr<Cursor> uiCursor;
        PODVector<RayQueryResult> hitResults;
    } cursor;
} GameWorld;

typedef struct Resources
{
    struct {
        struct {
            SharedPtr<Model> male;
            SharedPtr<Model> female;
        } characters;
        struct {
            SharedPtr<Model> firePit;
            SharedPtr<Model> streetLight;
        } doodads;
        struct {
            SharedPtr<Model> cookieJar;
        } vehicles;
        struct {
            SharedPtr<Model> blockCenter;
            SharedPtr<Model> blockSide;
            SharedPtr<Model> blockTween;
            SharedPtr<Model> blockTweenCorner;
            SharedPtr<Model> blockInCorner;
            SharedPtr<Model> blockOutCorner;
            SharedPtr<Model> blockFillCorner;
            SharedPtr<Model> blockDoubleCorner;
        } tileParts;
    } models;
    struct {
        SharedPtr<Material> darkness;
        SharedPtr<Material> cloth;
        SharedPtr<Material> skin;
        SharedPtr<Material> metal;
        SharedPtr<Material> leather;
        SharedPtr<Material> floor;
        SharedPtr<Material> wall;
        SharedPtr<Material> hair;
        SharedPtr<Material> pants;
        SharedPtr<Material> paint;
    } materials;
} Resources;

typedef struct HitInfo
{
    Vector3 position_;
    Vector3 hitNormal_;
    Node* hitNode_;
    Drawable* drawable_;
} HitInfo;

namespace {
StringHash const N_PLAYER = StringHash("Player");
StringHash const N_VOID = StringHash("Void");
StringHash const N_CURSOR = StringHash("Cursor");
StringHash const N_TILEPART = StringHash("TilePart");
StringHash const N_SLOT = StringHash("Slot");
}


enum JoyStickButton {JB_SELECT, JB_LEFTSTICK, JB_RIGHTSTICK, JB_START, JB_DPAD_UP, JB_DPAD_RIGHT, JB_DPAD_DOWN, JB_DPAD_LEFT, JB_L2, JB_R2, JB_L1, JB_R1, JB_TRIANGLE, JB_CIRCLE, JB_CROSS, JB_SQUARE};

class MasterControl : public Application
{
    /// Enable type information.
    OBJECT(MasterControl);
    friend class InputMaster;
    friend class OGTattCam;
public:
    /// Constructor.
    MasterControl(Context* context);
    GameWorld world;
    Resources resources;
    SharedPtr<ResourceCache> cache_;
    SharedPtr<Graphics> graphics_;

    /// Setup before engine initialization. Modifies the engine paramaters.
    virtual void Setup();
    /// Setup after engine initialization.
    virtual void Start();
    /// Cleanup after the main loop. Called by Application.
    virtual void Stop();
    void Exit();

    void CreateSineLookupTable();
    float Sine(float x);
    float Sine(float freq, float min, float max, float shift = 0.0f);
    ///Physics
    bool PhysicsRayCast(PODVector<PhysicsRaycastResult> &hitResults, Ray ray, float distance, unsigned collisionMask = M_MAX_UNSIGNED);
    bool PhysicsSphereCast(PODVector<RigidBody *> &hitResults, Vector3 center, float radius, unsigned collisionMask = M_MAX_UNSIGNED);

private:
    SharedPtr<UI> ui_;
    SharedPtr<Renderer> renderer_;
    SharedPtr<XMLFile> defaultStyle_;
    SharedPtr<PhysicsWorld> physicsWorld_;

    /// Set custom window title and icon
    void SetWindowTitleAndIcon();
    /// Create console and debug HUD
    void CreateConsoleAndDebugHud();

    /// Construct the scene content.
    void CreateScene();
    /// Construct user interface elements.
    void CreateUI();
    /// Subscribe to application-wide logic update and post-render update events.
    void SubscribeToEvents();

    /// Handle scene update event to control camera's pitch and yaw.
    void HandleSceneUpdate(StringHash eventType, VariantMap& eventData);
    /// Handle the logic update event.
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    /// Handle the post-render update event.
    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData);

    /// Create a mushroom object at position.
    void CreateDungeon(const Vector3 pos);
    void UpdateCursor(float timeStep);
    /// Utility function to raycast to the cursor position. Return true if hit.
    bool CursorRayCast(double maxDistance, PODVector<RayQueryResult> &hitResults);

    /// Pause flag
    bool paused_;

    ///Sine lookup table
    Vector<double> sine_;
    void LoadResources();
};

#endif // MASTERCONTROL_H