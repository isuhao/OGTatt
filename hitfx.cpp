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

HitFX::HitFX(Vector3 position, Substance substance):
    Effect(position, "HitFX")
{
    rootNode_->SetPosition(position);
    switch (substance){
    default: case Substance::Flesh: {
        particleEmitter_ = rootNode_->CreateComponent<ParticleEmitter>();
        ParticleEffect* particleEffect{MC->cache_->GetResource<ParticleEffect>("Particles/FleshHit.xml")};
        particleEmitter_->SetEffect(particleEffect);

        hit_sfx = MC->cache_->GetResource<Sound>("Samples/Thud.ogg");
        hit_sfx->SetLooped(false);
    } break;
    case Substance::Rock: {
        particleEmitter_ = rootNode_->CreateComponent<ParticleEmitter>();
        ParticleEffect* particleEffect{MC->cache_->GetResource<ParticleEffect>("Particles/RockHit.xml")};
        particleEmitter_->SetEffect(particleEffect);

        hit_sfx = MC->cache_->GetResource<Sound>("Samples/Thud.ogg");
        hit_sfx->SetLooped(false);
    } break;
    case Substance::Metal: {
        particleEmitter_ = rootNode_->CreateComponent<ParticleEmitter>();
        ParticleEffect* particleEffect{MC->cache_->GetResource<ParticleEffect>("Particles/MetalHit.xml")};
        particleEmitter_->SetEffect(particleEffect);

        hit_sfx = MC->cache_->GetResource<Sound>("Samples/Thud.ogg");
        hit_sfx->SetLooped(false);
    } break;
    }

    PlaySample(hit_sfx, 0.13f);
}
