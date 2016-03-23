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

HitFX::HitFX(Context *context, MasterControl *masterControl, Vector3 position):
    Effect(context, masterControl, position, "HitFX")
{
    rootNode_->SetPosition(position);
    particleEmitter_ = rootNode_->CreateComponent<ParticleEmitter>();
    ParticleEffect* particleEffect = masterControl_->cache_->GetResource<ParticleEffect>("Resources/Particles/BloodFX.xml");
    particleEmitter_->SetEffect(particleEffect);

    hit_sfx = masterControl_->cache_->GetResource<Sound>("Resources/Samples/Hit.ogg");
    hit_sfx->SetLooped(false);
    PlaySample(hit_sfx);
}
