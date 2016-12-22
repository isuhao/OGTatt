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
#include "controllable.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

enum Hair{HAIR_BALD, HAIR_SHORT, HAIR_MOHAWK, HAIR_SEAGULL, HAIR_MUSTAIN, HAIR_FROTOAD, HAIR_FLATTOP, HAIR_ALL};

class Character : public Controllable
{
    URHO3D_OBJECT(Character, Controllable);
public:
    Character(Context *context);
    static void RegisterObject(Context *context);
    virtual void OnNodeSet(Node *node);
    virtual void Set(Vector3 position);
    virtual void Update(float timeStep);

    float GetHealth(){return health_;}
    void Hit(float damage);
    Substance GetSubstance();

    virtual void Think(float timeStep);
protected:
    AnimatedModel* hairModel_;

    AnimationController* animCtrl_;
    bool male_;
    Hair hairStyle_;
    Vector<Color> colors_;

    bool alive_;
    float health_;
    float maxHealth_;
    int firstHitBy_ = 0;
    int lastHitBy_ = 0;
    int score_ = 0;

    const float shotInterval_ = 0.1f;
    float sinceLastShot_ = 0.0f;
    Sound* shot_sfx;

    float sinceLastTurn_;
    float turnInterval_;
private:
    void CreateBody();
    void CreateRagdoll();
    void Die();
    void CreateRagdollBone(const String &boneName, ShapeType type, const Vector3 &size, const Vector3 &position, const Quaternion &rotation);
    void CreateRagdollConstraint(const String &boneName, const String &parentName, ConstraintType type, const Vector3 &axis, const Vector3 &parentAxis, const Vector2 &highLimit, const Vector2 &lowLimit, bool disableCollision);
};

#endif // NPC_H
