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

#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <Urho3D/Urho3D.h>

#include "mastercontrol.h"

class SceneObject : public Object
{
    URHO3D_OBJECT(SceneObject, Object);
public:
    SceneObject(Context *context, MasterControl* masterControl);

    Vector3 GetWorldPosition() {return rootNode_->GetWorldPosition();}
protected:
    MasterControl* masterControl_;
    SharedPtr<Node> rootNode_;
    Vector<SharedPtr<SoundSource> > sampleSources_;
    bool destructable_;
    float randomizer_;

    void Disable();
    void Destroy();
    void Set(Vector3 position);

    void PlaySample(Sound *sample, float gain = 0.3f);

    void HandleSceneUpdate(StringHash eventType, VariantMap &eventData);
};

#endif // SCENEOBJECT_H
