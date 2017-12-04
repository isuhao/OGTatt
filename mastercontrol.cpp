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
#include "spawnmaster.h"
#include "ogtattcam.h"
#include "level.h"
#include "player.h"
#include "bullet.h"
#include "muzzle.h"
#include "hitfx.h"
#include "cookiejar.h"
#include "honti.h"
#include "pedestrian.h"
#include "character.h"
#include "streetlight.h"
#include "cookiejar.h"
#include "honti.h"
#include "frop.h"
#include "explosion.h"

#include "mastercontrol.h"

URHO3D_DEFINE_APPLICATION_MAIN(MasterControl);

MasterControl::MasterControl(Context *context):
    Application(context),
    paused_(false)
{
}

void MasterControl::Setup()
{
    SetRandomSeed(GetSubsystem<Time>()->GetSystemTime());
    // Modify engine startup parameters.
    //Set custom window title and icon.
    engineParameters_[EP_WINDOW_TITLE] = "OG Tatt";
    engineParameters_[EP_WINDOW_ICON] = "icon.png";
    engineParameters_[EP_LOG_NAME] = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs") + "ogtatt.log";

    //Add resource paths
    String resourcePaths{};

//    if (FILES->DirExists(FILES->GetAppPreferencesDir("luckey", "ogtatt")))
//        resourcePaths = FILES->GetAppPreferencesDir("luckey", "ogtatt");
    /*else*/ if (FILES->DirExists("Resources"))
        resourcePaths = "Resources";
    else if (FILES->DirExists("../OGTatt/Resources"))
        resourcePaths = "../OGTatt/Resources";

    resourceFolder_ = resourcePaths;
    resourcePaths += ";";

    if (FILES->DirExists("Data"))
        resourcePaths += "Data;";
    if (FILES->DirExists("CoreData"))
        resourcePaths += "CoreData;";

    engineParameters_[EP_RESOURCE_PATHS] = resourcePaths;

//    engineParameters_["FullScreen"] = false;
//    engineParameters_["Headless"] = false;
//    engineParameters_["WindowWidth"] = 960;
//    engineParameters_["WindowHeight"] = 540;
}
void MasterControl::Start()
{

    context_->RegisterSubsystem(this);
    context_->RegisterSubsystem(new InputMaster(context_));
    context_->RegisterSubsystem(new SpawnMaster(context_));

    OGTattCam::RegisterObject(context_);
    Bullet::RegisterObject(context_);
    Muzzle::RegisterObject(context_);
    HitFX::RegisterObject(context_);
    Pedestrian::RegisterObject(context_);
    Character::RegisterObject(context_);
    StreetLight::RegisterObject(context_);
    Cookiejar::RegisterObject(context_);
    Honti::RegisterObject(context_);
    Frop::RegisterObject(context_);
    Explosion::RegisterObject(context_);


    // Get default style
    defaultStyle_ = CACHE->GetResource<XMLFile>("UI/DefaultStyle.xml");
    //Create console and debug HUD.
    CreateConsoleAndDebugHud();
    //Create the scene content
    CreateScene();
    //Create the UI content
    CreateUI();
    //Hook up to the frame update and render post-update events
    SubscribeToEvents();

    Node* musicNode{ world.scene->CreateChild("Music") };
    musicSource_ = musicNode->CreateComponent<SoundSource>();
    musicSource_->SetSoundType(SOUND_MUSIC);
    musicSource_->SetGain(0.23f);
//    musicSource_->Play(GetMusic("Hans Atom - Waste of TV"));

//    GetSubsystem<Audio>()->Stop(); ///////////////////////////////////////////////////////////////////////
}
void MasterControl::Stop()
{
    engine_->DumpResources(true);
}

void MasterControl::SubscribeToEvents()
{
    SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(MasterControl, HandleSceneUpdate));
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(MasterControl, HandleUpdate));
    SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(MasterControl, HandleSceneUpdate));
    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(MasterControl, HandlePostRenderUpdate));

}

void MasterControl::CreateConsoleAndDebugHud()
{
    // Create console
    Console* console{ engine_->CreateConsole() };
    console->SetDefaultStyle(defaultStyle_);
    console->GetBackground()->SetOpacity(0.8f);

    // Create debug HUD.
    DebugHud* debugHud{ engine_->CreateDebugHud() };
    debugHud->SetDefaultStyle(defaultStyle_);
}

void MasterControl::CreateUI()
{
    ResourceCache* cache{ GetSubsystem<ResourceCache>() };
    UI* ui{ GetSubsystem<UI>() };

    //Create a Cursor UI element because we want to be able to hide and show it at will. When hidden, the mouse cursor will control the camera
    world.cursor.uiCursor = new Cursor(context_);
    world.cursor.uiCursor->SetVisible(false);
    ui->SetCursor(world.cursor.uiCursor);

    //Set starting position of the cursor at the rendering window center
    world.cursor.uiCursor->SetPosition(GRAPHICS->GetWidth()/2, GRAPHICS->GetHeight()/2);

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
    world.cursor.sceneCursor->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
    //StaticModel* cursorObject = world.cursor.sceneCursor->CreateComponent<StaticModel>();
    //cursorObject->SetModel(GetModel("Cursor"));
    //cursorObject->SetMaterial(cache_->GetResource<Material>("Materials/glow.xml"));

    //Create an invisible plane for mouse raycasting
    world.voidNode = world.scene->CreateChild("Void");
    //Location is set in update since the plane moves with the camera.
    world.voidNode->SetScale(Vector3(1000.0f, 1.0f, 1000.0f));
    StaticModel* planeObject{ world.voidNode->CreateComponent<StaticModel>() };
    planeObject->SetModel(GetModel("Plane"));
    planeObject->SetMaterial(GetMaterial("Invisible"));

    //Create a directional sky light.
    Node* lightNode{ world.scene->CreateChild("DirectionalLight") };
    lightNode->SetDirection(Vector3(0.1f, -1.0f, -0.3f));
    Light* light{ lightNode->CreateComponent<Light>() };
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetBrightness(0.1f);
    light->SetColor(Color(0.666f, 0.9f, 1.0f));
    light->SetSpecularIntensity(0.0f);
//    light->SetCastShadows(true);
//    light->SetShadowIntensity(0.8f);
//    light->SetShadowBias(BiasParameters(0.00025f, 0.5f));

    //Set cascade splits at 10, 50, 200 world units, fade shadows at 80% of maximum shadow distance
//    light->SetShadowCascade(CascadeParameters(7.0f, 23.0f, 42.0f, 500.0f, 0.8f));

    //Create camera


    for (int p : {1}){
        players_.Push(SharedPtr<Player>(new Player(p, context_)));

        OGTattCam* cam{ SPAWN->Create<OGTattCam>(false) };
        cam->Set(Vector3(0.0f + 5.0f * p, 23.0f, -0.5f), p);
        cameras_.Push(SharedPtr<OGTattCam>(cam));
    }

    new Level(context_);

    for (int p{0}; p < 90; ++p){

        Character* character{ GetSubsystem<SpawnMaster>()->Create<Character>() };
        character->Set(Vector3(Random(-23.0f, 23.0f), 0.0f, Random(-23.0f, 23.0f)));

        if (p == 0)
            GetSubsystem<InputMaster>()->SetPlayerControl(GetPlayer(1), character);
//        else if (p == 1)
//            GetSubsystem<InputMaster>()->SetPlayerControl(GetPlayer(2), character);
    }
//    for (int c{1}; c <= 5 ; ++c) new Cookiejar(context_, this, 2.3f*Vector3::RIGHT*c);
//    for (int h{0}; h < 5  ; ++h) new Honti(context_, this, 3.0f * Vector3::LEFT + Quaternion((360.0f * 0.2f) * h, Vector3::UP) * Vector3::RIGHT);
}

void MasterControl::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    //Temporary character switching
    if (INPUT->GetKeyPress(KEY_TAB)) {
        PODVector<Node*> characters{};
        world.scene->GetChildrenWithComponent<Character>(characters, true);

        PODVector<Node*> availableCharacters{};

        for (Node* n : characters)
            if (n->IsEnabled() && !n->GetComponent<Character>()->GetPlayer())
                availableCharacters.Push(n);

        if (availableCharacters.Size())
        GetSubsystem<InputMaster>()->SetPlayerControl(GetPlayer(1),
                                                      availableCharacters[Random(static_cast<int>(availableCharacters.Size()))]->GetComponent<Character>());
//        if (availableCharacters.Size() > 1)
//        GetSubsystem<InputMaster>()->SetPlayerControl(GetPlayer(2),
//                                                      availableCharacters[Random(static_cast<int>(availableCharacters.Size()))]->GetComponent<Character>());
    }
}

void MasterControl::HandleSceneUpdate(StringHash eventType, VariantMap &eventData)
{
//    float timeStep{ eventData[Update::P_TIMESTEP].GetFloat() };
//    world.voidNode->SetPosition(LucKey::Scale(world.camera->GetWorldPosition(), Vector3::ONE - Vector3::UP));
//    UpdateCursor(timeStep);
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
    Ray cameraRay{ world.camera->camera_->GetScreenRay(0.5f,0.5f) };
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
    float phase{ freq * world.scene->GetElapsedTime() + shift };
    float add{ 0.5f * (min + max) };
    return LucKey::Sine(phase) * 0.5f * (max - min) + add;
}
float MasterControl::Cosine(const float freq, const float min, const float max, const float shift)
{
    float phase{ freq * world.scene->GetElapsedTime() + shift };
    float add{ 0.5f * (min + max) };
    return LucKey::Cosine(phase) * 0.5f * (max - min) + add;
}

Vector<SharedPtr<Player> > MasterControl::GetPlayers() const {
    return players_;
}
Player* MasterControl::GetPlayer(int playerId) const
{
    for (Player* p : players_) {

        if (p->GetPlayerId() == playerId)
            return p;
    }
    return nullptr;
}

Vector<SharedPtr<OGTattCam> > MasterControl::GetCameras() const {
    return cameras_;
}
OGTattCam* MasterControl::GetCamera(int playerId) const
{
    for (OGTattCam* c : cameras_) {

        if (c->GetPlayerId() == playerId)
            return c;
    }
    return nullptr;
}

Material* MasterControl::GetMaterial(String name) const
{
    return CACHE->GetResource<Material>("Materials/" + name + ".xml");
}
Model* MasterControl::GetModel(String name) const
{
    return CACHE->GetResource<Model>("Models/" + name + ".mdl");
}
Texture* MasterControl::GetTexture(String name) const
{
    return CACHE->GetResource<Texture>("Textures/" + name + ".png");
}
Sound* MasterControl::GetMusic(String name) const {
    Sound* song{ CACHE->GetResource<Sound>("Music/"+name+".ogg") };
    song->SetLooped(true);
    return song;
}
Sound* MasterControl::GetSample(String name) const {
    Sound* sample{ CACHE->GetResource<Sound>("Samples/"+name+".ogg") };
    sample->SetLooped(false);
    return sample;
}

void MasterControl::HandlePostRenderUpdate(StringHash eventType, VariantMap &eventData)
{ (void)eventType; (void)eventData;
    return;

    physicsWorld_->DrawDebugGeometry(true);
}
