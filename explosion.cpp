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

#include "explosion.h"
#include "hitfx.h"

void Explosion::RegisterObject(Context *context)
{
    context->RegisterFactory<Explosion>();
}

Explosion::Explosion(Context* context):
    Effect(context),
    initialMass_{3.0f},
    initialBrightness_{8.0f}
{
}

void Explosion::OnNodeSet(Node *node)
{ if (!node) return;

    Effect::OnNodeSet(node);

    Color color{ 1.0f, 0.98f, 0.92f };

    emitTime_ = 0.20f;

    rigidBody_ = node_->CreateComponent<RigidBody>();
    rigidBody_->SetLinearFactor(Vector3::ONE - Vector3::UP);

    light_ = node_->CreateComponent<Light>();
    light_->SetRange(13.0f);
    light_->SetColor(color);

    ParticleEffect* particleEffect{ MC->CACHE->GetResource<ParticleEffect>("Particles/Explosion.xml") };
    Vector<ColorFrame> colorFrames;
    colorFrames.Push(ColorFrame(Color(0.0f, 0.0f, 0.0f, 0.0f), 0.0f));
    Color mixColor{ 0.5f * (color + particleEffect->GetColorFrame(1)->color_) };
    colorFrames.Push(ColorFrame(mixColor, 0.1f));
    colorFrames.Push(ColorFrame(mixColor * 0.1f, 0.35f));
    colorFrames.Push(ColorFrame(Color(0.0f, 0.0f, 0.0f, 0.0f), 0.0f));
    particleEffect->SetColorFrames(colorFrames);
    particleEmitter_->SetEffect(particleEffect);

    explode_sfx = MC->CACHE->GetResource<Sound>("Samples/Explode.ogg");
    explode_sfx->SetLooped(false);
    sampleSource_ = node_->CreateComponent<SoundSource>();
    sampleSource_->SetSoundType(SOUND_EFFECT);
    sampleSource_->SetGain(0.5f);

    SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(Explosion, UpdateExplosion));
}

void Explosion::Set(Vector3 position)
{
    Effect::Set(position);

    rigidBody_->SetMass(initialMass_);
    light_->SetBrightness(initialBrightness_);

    sampleSource_->Play(explode_sfx);
}

void Explosion::UpdateExplosion(StringHash eventType, VariantMap& eventData)
{ (void)eventType;

    float timeStep{ eventData[Update::P_TIMESTEP].GetFloat() };

    rigidBody_->SetMass(Max(initialMass_ * ((0.1f - age_)/0.1f),0.01f));
    light_->SetBrightness(Max(initialBrightness_*(0.32f - age_)/0.32f,0.0f));

    if (node_->IsEnabled()) {
        PODVector<RigidBody*> hitResults;
        float radius{ 2.0f * initialMass_ + age_ * 7.0f };
        if (MC->PhysicsSphereCast(hitResults,node_->GetPosition(), radius, M_MAX_UNSIGNED)){
            for (unsigned i{0}; i < hitResults.Size(); ++i){
                if (!hitResults[i]->IsTrigger()){
                    hitResults[i]->ApplyForce((hitResults[i]->GetNode()->GetWorldPosition() - node_->GetWorldPosition()) * sqrt(radius-LucKey::Distance(node_->GetWorldPosition(), hitResults[i]->GetNode()->GetWorldPosition()))*timeStep*500.0f*rigidBody_->GetMass()
                                );
                    //Deal damage
//                    unsigned hitID = hitResults[i]->GetNode()->GetID();
//                    float damage = rigidBody_->GetMass()*timeStep;
                }
            }
        }
    }
}
