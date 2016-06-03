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

#include "inputmaster.h"
#include "ogtattcam.h"
#include "level.h"
#include "player.h"
#include "cookiejar.h"
#include "honti.h"
#include "pedestrian.h"

#include "mastercontrol.h"

URHO3D_DEFINE_APPLICATION_MAIN(MasterControl);

MasterControl* MasterControl::instance_ = NULL;

MasterControl* MasterControl::GetInstance()
{
    return MasterControl::instance_;
}

MasterControl::MasterControl(Context *context):
    Application(context),
    paused_(false)
{
    instance_ = this;
}

void MasterControl::Setup()
{
    SetRandomSeed(GetSubsystem<Time>()->GetSystemTime());
    // Modify engine startup parameters.
    //Set custom window title and icon.
    engineParameters_["WindowTitle"] = "OG Tatt";
    engineParameters_["LogName"] = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs")+"OGTatt.log";
    engineParameters_["ResourcePaths"] = "Data;CoreData;Resources";
    engineParameters_["WindowIcon"] = "icon.png";
//    engineParameters_["FullScreen"] = false;
//    engineParameters_["Headless"] = false;
//    engineParameters_["WindowWidth"] = 960;
//    engineParameters_["WindowHeight"] = 540;
}
void MasterControl::Start()
{
    new InputMaster();
    cache_ = GetSubsystem<ResourceCache>();
    graphics_ = GetSubsystem<Graphics>();
    renderer_ = GetSubsystem<Renderer>();

    // Get default style
    defaultStyle_ = cache_->GetResource<XMLFile>("UI/DefaultStyle.xml");
    //Create console and debug HUD.
    CreateConsoleAndDebugHud();
    //Create the scene content
    CreateScene();
    //Create the UI content
    CreateUI();
    //Hook up to the frame update and render post-update events
    SubscribeToEvents();

    Sound* music = cache_->GetResource<Sound>("Music/Huilende Rappers - Loop als een Piraatje.ogg");
    music->SetLooped(true);
    Node* musicNode = world.scene->CreateChild("Music");
    musicSource_ = musicNode->CreateComponent<SoundSource>();
    musicSource_->SetSoundType(SOUND_MUSIC);
    musicSource_->SetGain(0.23f);
//    musicSource_->Play(music);
}
void MasterControl::Stop()
{
    engine_->DumpResources(true);
}

void MasterControl::SubscribeToEvents()
{
    //Subscribe scene update event.
    SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(MasterControl, HandleSceneUpdate));
    //Subscribe HandleUpdate() function for processing update events
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(MasterControl, HandleUpdate));
    //Subscribe scene update event.
    SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(MasterControl, HandleSceneUpdate));
}

void MasterControl::CreateConsoleAndDebugHud()
{
    // Create console
    Console* console{engine_->CreateConsole()};
    console->SetDefaultStyle(defaultStyle_);
    console->GetBackground()->SetOpacity(0.8f);

    // Create debug HUD.
    DebugHud* debugHud = engine_->CreateDebugHud();
    debugHud->SetDefaultStyle(defaultStyle_);
}

void MasterControl::CreateUI()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    UI* ui = GetSubsystem<UI>();

    //Create a Cursor UI element because we want to be able to hide and show it at will. When hidden, the mouse cursor will control the camera
    world.cursor.uiCursor = new Cursor(context_);
    world.cursor.uiCursor->SetVisible(false);
    ui->SetCursor(world.cursor.uiCursor);

    //Set starting position of the cursor at the rendering window center
    world.cursor.uiCursor->SetPosition(graphics_->GetWidth()/2, graphics_->GetHeight()/2);

    //Construct new Text object, set string to display and font to use
//    Text* instructionText = ui->GetRoot()->CreateChild<Text>();
//    instructionText->SetText("OG Tatt");
//    instructionText->SetFont(cache->GetResource<Font>("Fonts/infinite.ttf"), 23);
//    instructionText->SetColor(Color(0.666f, 1.0f, 0.23f, 0.42f));
//    //The text has multiple rows. Center them in relation to each other
//    instructionText->SetHorizontalAlignment(HA_CENTER);
//    instructionText->SetVerticalAlignment(VA_CENTER);
//    instructionText->SetPosition(0, ui->GetRoot()->GetHeight()/2.1);
}

void MasterControl::CreateScene()
{
    world.scene = new Scene(context_);

    //Create octree, use default volume (-1000, -1000, -1000) to (1000,1000,1000)
    /*Octree* octree = */world.scene->CreateComponent<Octree>();
//    octree->SetSize(BoundingBox(Vector3(-10000, -100, -10000), Vector3(10000, 1000, 10000)), 1024);
    physicsWorld_ = world.scene->CreateComponent<PhysicsWorld>();
    world.scene->CreateComponent<DebugRenderer>();

    //Create cursor
    world.cursor.sceneCursor = world.scene->CreateChild("Cursor");
    world.cursor.sceneCursor->SetPosition(Vector3(0.0f,0.0f,0.0f));
    //StaticModel* cursorObject = world.cursor.sceneCursor->CreateComponent<StaticModel>();
    //cursorObject->SetModel(GetModel("Cursor"));
    //cursorObject->SetMaterial(cache_->GetResource<Material>("Materials/glow.xml"));

    //Create an invisible plane for mouse raycasting
    world.voidNode = world.scene->CreateChild("Void");
    //Location is set in update since the plane moves with the camera.
    world.voidNode->SetScale(Vector3(1000.0f, 1.0f, 1000.0f));
    StaticModel* planeObject{world.voidNode->CreateComponent<StaticModel>()};
    planeObject->SetModel(GetModel("Plane"));
    planeObject->SetMaterial(GetMaterial("Invisible"));

    //Create a directional light.
    Node* lightNode{world.scene->CreateChild("DirectionalLight")};
    lightNode->SetDirection(Vector3(0.1f, -1.0f, -0.3f));
    Light* light{lightNode->CreateComponent<Light>()};
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetBrightness(0.1f);
    light->SetColor(Color(1.0f, 0.9f, 0.666f));
    light->SetSpecularIntensity(0.0f);
//    light->SetCastShadows(true);
//    light->SetShadowIntensity(0.8f);
//    light->SetShadowBias(BiasParameters(0.00025f, 0.5f));

    //Set cascade splits at 10, 50, 200 world units, fade shadows at 80% of maximum shadow distance
//    light->SetShadowCascade(CascadeParameters(7.0f, 23.0f, 42.0f, 500.0f, 0.8f));

    //Create camera
    world.camera = new OGTattCam();

    new Level();
    world.player_ = new Player();
    for (int p{0}; p < 100; ++p) new Pedestrian(Vector3(Random(-13.0f, 13.0f), 0.0f, Random(-13.0f, 13.0f)));
//    for (int c{1}; c <= 5 ; ++c) new Cookiejar(context_, this, 2.3f*Vector3::RIGHT*c);
//    for (int h{0}; h < 5  ; ++h) new Honti(context_, this, 3.0f * Vector3::LEFT + Quaternion((360.0f * 0.2f) * h, Vector3::UP) * Vector3::RIGHT);
}

void MasterControl::HandleUpdate(StringHash eventType, VariantMap &eventData)
{

}

void MasterControl::HandleSceneUpdate(StringHash eventType, VariantMap &eventData)
{
    using namespace Update;
    float timeStep{eventData[P_TIMESTEP].GetFloat()};
    world.voidNode->SetPosition(LucKey::Scale(world.camera->GetWorldPosition(), Vector3::ONE - Vector3::UP));
    UpdateCursor(timeStep);
}

void MasterControl::UpdateCursor(float timeStep)
{
    /*world.cursor.sceneCursor->Rotate(Quaternion(0.0f,100.0f*timeStep,0.0f));
    world.cursor.sceneCursor->SetScale((world.cursor.sceneCursor->GetWorldPosition() - world.camera->GetWorldPosition()).Length()*0.05f);
    if (CursorRayCast(250.0f, world.cursor.hitResults))
    {
        for (int i = 0; i < world.cursor.hitResults.Size(); i++)
        {
            if (world.cursor.hitResults[i].node_->GetNameHash() == N_VOID)
            {
                Vector3 camHitDifference = world.camera->translationNode_->GetWorldPosition() - world.cursor.hitResults[i].position_;
                camHitDifference /= world.camera->translationNode_->GetWorldPosition().y_ - world.voidNode->GetPosition().y_;
                camHitDifference *= world.camera->translationNode_->GetWorldPosition().y_;
                world.cursor.sceneCursor->SetWorldPosition(world.camera->translationNode_->GetWorldPosition()-camHitDifference);
            }
        }
    }
    */
}

bool MasterControl::CursorRayCast(double maxDistance, PODVector<RayQueryResult> &hitResults)
{
    Ray cameraRay = world.camera->camera_->GetScreenRay(0.5f,0.5f);
    RayOctreeQuery query(hitResults, cameraRay, RAY_TRIANGLE, maxDistance, DRAWABLE_GEOMETRY);
    world.scene->GetComponent<Octree>()->Raycast(query);
    if (hitResults.Size()) return true;
    else return false;
}

bool MasterControl::PhysicsRayCast(PODVector<PhysicsRaycastResult> &hitResults, Ray ray, float distance, unsigned collisionMask)
{
    physicsWorld_->Raycast(hitResults, ray, distance, collisionMask);
    if (hitResults.Size()) return true;
    else return false;
}

bool MasterControl::PhysicsSphereCast(PODVector<RigidBody*> &hitResults, Vector3 center, float radius, unsigned collisionMask)
{
    physicsWorld_->GetRigidBodies(hitResults, Sphere(center, radius), collisionMask);
    if (hitResults.Size()) return true;
    else return false;
}

void MasterControl::Exit()
{
//    File file(context_, "Resources/Test.xml", FILE_WRITE);
//    world.scene->SaveXML(file);

    engine_->Exit();
}

float MasterControl::Sine(const float freq, const float min, const float max, const float shift)
{
    float phase{freq * world.scene->GetElapsedTime() + shift};
    float add{0.5f * (min + max)};
    return LucKey::Sine(phase) * 0.5f * (max - min) + add;
}
float MasterControl::Cosine(const float freq, const float min, const float max, const float shift)
{
    float phase{freq * world.scene->GetElapsedTime() + shift};
    float add{0.5f * (min + max)};
    return LucKey::Cosine(phase) * 0.5f * (max - min) + add;
}

void MasterControl::HandlePostRenderUpdate(StringHash eventType, VariantMap &eventData)
{
    //world.scene->GetComponent<PhysicsWorld>()->DrawDebugGeometry(true);
}
