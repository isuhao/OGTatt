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

#include "muzzle.h"


void Muzzle::RegisterObject(Context *context)
{
    context->RegisterFactory<Muzzle>();
}

Muzzle::Muzzle(Context* context):
    Effect(context)
{
}

void Muzzle::OnNodeSet(Node *node)
{
    Effect::OnNodeSet(node_);
//    node_->SetPosition(position);
//    particleEmitter_ = node_->CreateComponent<ParticleEmitter>();
    ParticleEffect* particleEffect = CACHE->GetResource<ParticleEffect>("Particles/Muzzle.xml");
    particleEmitter_->SetEffect(particleEffect);

    Node* lightNode_ = node_->CreateChild("LightNode");
    lightNode_->SetPosition(Vector3(0.0f, 0.0f, 0.2f));
    light_ = lightNode_->CreateComponent<Light>();
    light_->SetRange(2.3f);
    light_->SetColor(Color(1.0f, 1.0f, 0.666f));
    light_->SetCastShadows(true);
}

void Muzzle::Set(Vector3 position, Vector3 direction)
{
    Effect::Set(position);

    node_->LookAt(node_->GetPosition() + direction);

}

void Muzzle::Update(float timeStep)
{
    Effect::Update(timeStep);

    light_->SetBrightness(Max(1.0f-age_*10.0f,0.0f));
}

