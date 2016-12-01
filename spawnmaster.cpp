/* heXon
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

#include "spawnmaster.h"

SpawnMaster::SpawnMaster(Context* context):
    Object(context),
    spawning_{false}
{
}

void SpawnMaster::Activate()
{
//    AUDIO->SetMasterGain(SOUND_EFFECT, 0.0f);
//    AUDIO->SetMasterGain(SOUND_EFFECT, 1.0f);

    SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(SpawnMaster, HandleSceneUpdate));
}
void SpawnMaster::Deactivate()
{
    UnsubscribeFromAllEvents();
}
void SpawnMaster::Clear()
{
}

void SpawnMaster::Restart()
{
    Clear();
    Activate();
}

Vector3 SpawnMaster::SpawnPoint()
{
    return Vector3(Random(-5.0f, 5.0f), -42.0f, Random(-5.0f, 5.0f));
}

void SpawnMaster::HandleSceneUpdate(StringHash eventType, VariantMap &eventData)
{
    const float timeStep{ eventData[SceneUpdate::P_TIMESTEP].GetFloat() };

}
