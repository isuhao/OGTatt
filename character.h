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

#ifndef NPC_H
#define NPC_H

#include <Urho3D/Urho3D.h>

#include "mastercontrol.h"
#include "sceneobject.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

enum Hair{HAIR_BALD, HAIR_SHORT, HAIR_MOHAWK, HAIR_SEAGULL, HAIR_MUSTAIN, HAIR_FROTOAD, HAIR_FLATTOP, HAIRSTYLES};

class Character : public SceneObject
{
    URHO3D_OBJECT(Character, SceneObject);
public:
    Character(Vector3 pos);
    Substance GetSubstance();
protected:
    AnimatedModel* bodyModel_;
    AnimatedModel* hairModel_;

    AnimationController* animCtrl_;
    RigidBody* rigidBody_;
    CollisionShape* collisionShape_;
    bool male_;
    Hair hairStyle_;
    Vector<Color> colors_;

    float maxHealth_;
    float health_;
};

#endif // NPC_H
