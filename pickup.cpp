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

#include "pickup.h"

Pickup::Pickup(Context* context):
    SceneObject(context)
{
    node_->SetName("Pickup");
    model_ = node_->CreateComponent<StaticModel>();
    node_->SetScale(0.8);


    rigidBody_ = node_->CreateComponent<RigidBody>();
    rigidBody_->SetRestitution(0.666f);
    rigidBody_->SetMass(0.5f);
    rigidBody_->SetLinearFactor(Vector3::ONE - Vector3::UP);
    rigidBody_->SetLinearDamping(0.5f);
    rigidBody_->SetFriction(0.0f);
    rigidBody_->SetAngularDamping(0.0f);
    rigidBody_->ApplyTorque(Vector3::UP * 32.0);
    rigidBody_->SetLinearRestThreshold(0.0f);
    rigidBody_->SetAngularRestThreshold(0.0f);

    CollisionShape* collisionShape = node_->CreateComponent<CollisionShape>();
    collisionShape->SetSphere(1.5f);

    Node* triggerNode = node_->CreateChild("PickupTrigger");
    triggerBody_ = triggerNode->CreateComponent<RigidBody>();
    triggerBody_->SetTrigger(true);
    triggerBody_->SetMass(0.0f);
    triggerBody_->SetLinearFactor(Vector3::ZERO);
    CollisionShape* triggerShape = triggerNode->CreateComponent<CollisionShape>();
    triggerShape->SetSphere(2.5f);

    particleEmitter_ = node_->CreateComponent<ParticleEmitter>();

    particleEmitter_->SetEffect(CACHE->GetTempResource<ParticleEffect>("Particles/Shine.xml"));

    shot_sfx = CACHE->GetResource<Sound>("Samples/Pickup.ogg");
    shot_sfx->SetLooped(false);
    sampleSource_ = node_->CreateComponent<SoundSource>();
    sampleSource_->SetGain(0.6f);
    sampleSource_->SetSoundType(SOUND_EFFECT);

    SubscribeToEvent(triggerNode, E_NODECOLLISIONSTART, URHO3D_HANDLER(Pickup, HandleTriggerStart));
    SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(Pickup, HandleSceneUpdate));
}

void Pickup::HandleTriggerStart(StringHash eventType, VariantMap &eventData)
{
    using namespace NodeCollisionStart;

    PODVector<RigidBody*> collidingBodies;
    triggerBody_->GetCollidingBodies(collidingBodies);

    for (int i = 0; i < collidingBodies.Size(); i++) {
        RigidBody* collider = collidingBodies[i];
        if (collider->GetNode()->HasTag("Player")) {
            node_->SetEnabled(false);
        }
    }
}

void Pickup::HandleSceneUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    //Emerge
    if (node_->GetPosition().y_ < -0.1f) {
        node_->Translate(Vector3::UP * timeStep * (0.25f - node_->GetPosition().y_));
        //topRenderer.materials[1].color = randomColor * 0.25f;
    }
    triggerBody_->SetPosition(node_->GetPosition());
    /*Vector3 pos = rootNode_->GetPosition();
    if (pos.y_ < 0.0) rootNode_->SetPosition(Vector3::UP*pow(pos.y_,2.0f)*timeStep+pos);
    if (pos.y_ > 0.0) rootNode_->SetPosition(Vector3::Scale(Vector3::ONE - Vector3::UP, pos));*/
}

