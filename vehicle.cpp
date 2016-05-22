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

#include "vehicle.h"
#include "explosion.h"

Vehicle::Vehicle(Vector3 position, Quaternion rotation):
    SceneObject()
{
    rootNode_->SetName("Vehicle");
    rootNode_->SetPosition(position);
    rootNode_->SetRotation(rotation);
    chassisModel_ = rootNode_->CreateComponent<AnimatedModel>();
    chassisModel_->SetCastShadows(true);

    rigidBody_ = rootNode_->CreateComponent<RigidBody>();

    collisionShape_ = rootNode_->CreateComponent<CollisionShape>();

    particleNode_ = rootNode_->CreateChild("Fire");
    flameEmitter_ = particleNode_->CreateComponent<ParticleEmitter>();
    flameEmitter_->SetEffect(MC->cache_->GetResource<ParticleEffect>("Particles/fire1.xml"));
    flameEmitter_->SetEmitting(false);

//    decal_ = rootNode_->CreateComponent<DecalSet>();
//    decal_->SetMaterial(MC->GetMaterial("Decal"));
//    Quaternion decalRotation = rootNode_->GetRotation();
//    decalRotation  = decalRotation * Quaternion(90.0f, rootNode_->GetRight());
//    decal_->AddDecal(chassisModel_, rootNode_->GetWorldPosition()-0.23f*rootNode_->GetDirection(), decalRotation, 0.666f, 1.0f, 2.3f, Vector2::ZERO, Vector2::ONE);

}

void Vehicle::Hit(float damage)
{
}

void Vehicle::SetupLights(int front, int rear, BoundingBox box)
{
    if (front) {
        for (int f{0}; f < front; ++f){
            Pair<SharedPtr<Node>, SharedPtr<Light>> light;
            light.first_ = rootNode_->CreateChild("HeadLight");
            light.first_->SetDirection(Vector3(0.0f, -0.23f, 0.666f));
            if (front == 1) {
                light.first_->SetPosition(Vector3(0.5f * (box.min_.x_ + box.max_.x_),
                                                  box.min_.y_,
                                                  box.max_.z_));
            } else {
                light.first_->SetPosition(Vector3(box.min_.x_ + f * (box.Size().x_ / (front - 1)),
                                                  box.min_.y_,
                                                  box.max_.z_));
            }
            light.second_ = light.first_->CreateComponent<Light>();
            light.second_->SetLightType(LIGHT_SPOT);
            light.second_->SetColor(Color(1.0f, 0.9f, 0.8f));
            light.second_->SetRange(8.0f);
            light.second_->SetFov(60.0f);
            light.second_->SetBrightness(5.0f);
            light.second_->SetCastShadows(true);
            light.second_->SetShadowResolution(0.25f);
            light.second_->SetShadowBias(BiasParameters(0.00001f, 0.5f));
            light.second_->SetShadowCascade(CascadeParameters(0.23f, 2.0f, 3.0f, 5.0f, 0.5f));

            headLights_.Push(light);
        }
    }
    if (rear) {
        for (int r{0}; r < rear; ++r){
            Pair<SharedPtr<Node>, SharedPtr<Light>> light;
            light.first_ = rootNode_->CreateChild("TailLight");
            light.first_->SetDirection(Vector3(0.0f, -0.6f, -0.5f));
            if (front == 1) {
                light.first_->SetPosition(Vector3(0.5f * (box.min_.x_ + box.max_.x_),
                                                  box.max_.y_,
                                                  box.min_.z_));
            } else {
                light.first_->SetPosition(Vector3(box.min_.x_ + r * (box.Size().x_ / (rear - 1)),
                                                  box.max_.y_,
                                                  box.min_.z_));
                light.first_->Rotate(Quaternion(30.0f - r * (60.0f / (rear - 1)), Vector3::UP), TS_WORLD);
            }
            light.second_ = light.first_->CreateComponent<Light>();
            light.second_->SetLightType(LIGHT_SPOT);
            light.second_->SetColor(Color::RED);
            light.second_->SetRange(3.0f);
            light.second_->SetFov(120.0f);
            light.second_->SetBrightness(2.0f);
            light.second_->SetCastShadows(true);
            light.second_->SetShadowResolution(0.25f);

            tailLights_.Push(light);
        }
    }
}

void Vehicle::Destroy()
{
    new Explosion(rootNode_->GetPosition(), 1.0f);
    for (unsigned i{0}; i < chassisModel_->GetNumGeometries(); ++i){
        chassisModel_->SetMaterial(i, MC->GetMaterial("Darkness"));
    }
    flameEmitter_->SetEmitting(true);
}
