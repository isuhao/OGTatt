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

#ifndef PICKUP_H
#define PICKUP_H

#include <Urho3D/Urho3D.h>

#include "sceneobject.h"

namespace Urho3D {
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class Pickup : public SceneObject
{
    URHO3D_OBJECT(Pickup, SceneObject);
public:
    Pickup(Context *context);
protected:
    SharedPtr<RigidBody> rigidBody_;
    SharedPtr<RigidBody> triggerBody_;
    SharedPtr<StaticModel> model_;
    SharedPtr<ParticleEmitter> particleEmitter_;
    SharedPtr<Sound> shot_sfx;
    SharedPtr<SoundSource> sampleSource_;

    void HandleTriggerStart(StringHash otherNode, VariantMap &eventData);
    void HandleSceneUpdate(StringHash eventType, VariantMap &eventData);
private:

};

#endif //PICKUP_H
