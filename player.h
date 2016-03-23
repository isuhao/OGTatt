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

#ifndef PLAYER_H
#define PLAYER_H

#include <Urho3D/Urho3D.h>

#include "character.h"

class Player : public Character
{
    URHO3D_OBJECT(Player, SceneObject);
    friend class OGTattCam;
public:
    Player(Context* context, MasterControl* masterControl);
    //SharedPtr<Text> scoreText_;

    float GetHealth(){return health_;}
    void Hit(float damage, int ownerID);
    void AddScore(int points);
    Vector3 GetLinearVelocity() {return rigidBody_->GetLinearVelocity();}
private:
    float health_ = 1.0f;
    float initialHealth_ = 1.0f;
    int firstHitBy_ = 0;
    int lastHitBy_ = 0;
    int score_ = 0;    

    const float shotInterval_ = 0.1f;
    float sinceLastShot_ = 0.0f;
    Sound* shot_sfx;

    void HandleSceneUpdate(StringHash eventType, VariantMap &eventData);
};

#endif // PLAYER_H
