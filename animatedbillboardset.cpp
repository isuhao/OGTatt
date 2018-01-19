/* OG Tatt
// Copyright (C) 2018 LucKey Productions (luckeyproductions.nl)
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



#include "animatedbillboardset.h"

void AnimatedBillboardSet::RegisterObject(Context* context)
{
    context->RegisterFactory<AnimatedBillboardSet>();
}

AnimatedBillboardSet::AnimatedBillboardSet(Context* context) : BillboardSet(context),
    textureFrames_{},
    texIndices_{},
    animationTimers_{}
{
}

void AnimatedBillboardSet::LoadFrames(XMLFile* file)
{
    textureFrames_.Clear();

    XMLElement root{ file->GetRoot() };
    XMLElement anim = root.GetChild("texanim");
    while (anim = anim.GetNext("texanim")) {

        TextureFrame frame{};
        frame.uv_ = anim.GetRect("uv");
        frame.time_ = anim.GetFloat("time");
        textureFrames_.Push(frame);
    }

    if (textureFrames_.Empty())
        return;

    for (unsigned b{0}; b < GetNumBillboards(); ++b) {

        texIndices_[b] = Random(int(textureFrames_.Size()));
        animationTimers_[b] = textureFrames_.At(texIndices_[b]).time_;

        GetBillboard(b)->uv_ = textureFrames_[texIndices_[b]].uv_;
    }
    Commit();
}

void AnimatedBillboardSet::UpdateGeometry(const FrameInfo &frame)
{
    BillboardSet::UpdateGeometry(frame);

    if (!textureFrames_.Size())
        return;

    for (unsigned b{0}; b < GetNumBillboards(); ++b) {

        if (!GetBillboard(b)->enabled_)
            continue;

        // Texture animation
        unsigned& texIndex = texIndices_[b];
        float& time = animationTimers_[b];
        time += frame.timeStep_;

        if (time >= textureFrames_[texIndex].time_)
        {
            ++texIndex;
            if (texIndex >= textureFrames_.Size()) {
                texIndex = 0;
                time = 0.0f;
            }
            GetBillboard(b)->uv_ = textureFrames_[texIndex].uv_;
        }
    }
    Commit();
}
