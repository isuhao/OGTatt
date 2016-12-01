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

#include "hitfx.h"

void HitFX::RegisterObject(Context *context)
{
    context->RegisterFactory<HitFX>();
}

HitFX::HitFX(Context* context):
    Effect(context)
//    Effect(position, "HitFX")
{

}

void HitFX::OnNodeSet(Node* node)
{
    Effect::OnNodeSet(node_);
}

void HitFX::Set(Vector3 position, Substance substance)
{

    Effect::Set(position);

    switch (substance){
    default: case Substance::Flesh: {
        ParticleEffect* particleEffect{ CACHE->GetResource<ParticleEffect>("Particles/FleshHit.xml") };
        particleEmitter_->SetEffect(particleEffect);

        hit_sfx = MC->CACHE->GetResource<Sound>("Samples/Thud.ogg");
        hit_sfx->SetLooped(false);
    } break;
    case Substance::Rock: {
        ParticleEffect* particleEffect{ CACHE->GetResource<ParticleEffect>("Particles/RockHit.xml") };
        particleEmitter_->SetEffect(particleEffect);

        hit_sfx = MC->CACHE->GetResource<Sound>("Samples/Thud.ogg");
        hit_sfx->SetLooped(false);
    } break;
    case Substance::Metal: {
        ParticleEffect* particleEffect{ CACHE->GetResource<ParticleEffect>("Particles/MetalHit.xml") };
        particleEmitter_->SetEffect(particleEffect);

        hit_sfx = MC->CACHE->GetResource<Sound>("Samples/Thud.ogg");
        hit_sfx->SetLooped(false);
    } break;
    }

    PlaySample(hit_sfx, 0.13f);
}

void HitFX::Update(float timeStep)
{
    Effect::Update(timeStep);
}
