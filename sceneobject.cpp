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

#include "sceneobject.h"

SceneObject::SceneObject(Context *context):
    LogicComponent(context),
    randomizer_{Random()}
{
}

void SceneObject::OnNodeSet(Node *node)
{ if (!node) return;

    for (int i{0}; i < 5; ++i) {

        sampleSources_.Push(SharedPtr<SoundSource3D>(node_->CreateComponent<SoundSource3D>()));
        sampleSources_[i]->SetGain(0.3f);
        sampleSources_[i]->SetSoundType(SOUND_EFFECT);
        sampleSources_[i]->SetDistanceAttenuation(23.0f, 88.0f, 0.5f);
    }
}

void SceneObject::Set(Vector3 position)
{
    node_->SetPosition(position);
    node_->SetEnabledRecursive(true);
}

void SceneObject::Disable()
{
    node_->SetEnabledRecursive(false);
}

void SceneObject::PlaySample(Sound* sample, float gain)
{
    for (SoundSource3D* s : sampleSources_) {

        if (!s->IsPlaying()) {

            s->SetGain(gain);
            s->Play(sample);

            return;
        }
    }
}

bool SceneObject::IsSilent() const
{
    for (SoundSource* s : sampleSources_) {

        if (s->IsPlaying()) {

            return false;
        }
    }

    return true;
}
