#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/Graphics/ParticleEffect.h>FirePit
#include <Urho3D/Resource/ResourceCache.h>

#include "streetlight.h"

StreetLight::StreetLight(Context* context, MasterControl* masterControl, Tile* tile):
    Deco(context, masterControl, tile, "StreetLight")
{
    rootNode_->SetRotation(Quaternion(0.0f, tile->coords_.y_%2 * 180.0f, 0.0f));
    rootNode_->SetPosition(0.125f - (Random(10)%2)*0.25f, 0.0f, 0.125f - (Random(10)%2)*0.25f);

    StaticModel* model_ = rootNode_->CreateComponent<StaticModel>();
    model_->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/StreetLight.mdl"));
    model_->SetMaterial(0, masterControl_->cache_->GetResource<Material>("Resources/Materials/Metal.xml"));
    model_->SetCastShadows(true);

    rootNode_->CreateComponent<RigidBody>();
    CollisionShape* collider = rootNode_->CreateComponent<CollisionShape>();
    collider->SetCylinder(0.1f, 2.0f);

    lightNode_ = rootNode_->CreateChild("LightNode");
    lightNode_->SetPosition(0.0f, 2.3f, 0.5f);
    light_ = lightNode_->CreateComponent<Light>();
    light_->SetBrightness(1.8f);
    light_->SetColor(Color(1.0f, 0.6f, 0.4f));
    light_->SetRange(4.0f);
    light_->SetCastShadows(true);
    light_->SetShadowBias(BiasParameters(0.00023f, 0.5f));
    light_->SetShadowCascade(CascadeParameters(1.0f, 2.0f, 3.0f, 5.0f, 0.5f));
    light_->SetShadowResolution(0.25f);

    SubscribeToEvent(E_SCENEUPDATE, HANDLER(StreetLight, HandleSceneUpdate));
}

void StreetLight::HandleSceneUpdate(StringHash eventType, VariantMap& eventData)
{
    //float brightness = masterControl_->Sine(50.0f, 0.95f, 1.0f);
    //light_->SetBrightness(brightness);
}
