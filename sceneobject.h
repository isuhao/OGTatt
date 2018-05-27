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

//Used for footsteps and bullet hits
enum class Substance{Rock, Metal, Flesh};

class SceneObject : public LogicComponent
{
    URHO3D_OBJECT(SceneObject, LogicComponent);
public:
    SceneObject(Context* context);

    Vector3 GetWorldPosition() {return node_->GetWorldPosition();}
    virtual Substance GetSubstance(Vector3 position = Vector3::ZERO) const {}
    virtual void Set(Vector3 position);
    virtual void OnNodeSet(Node *node);
protected:
    Vector<SharedPtr<SoundSource3D> > sampleSources_;
    float randomizer_;

    void Disable();
    void Destroy();

    void PlaySample(Sound *sample, float gain = 0.3f);
    bool IsSilent() const;


    void HandleSceneUpdate(StringHash eventType, VariantMap &eventData);
};

#endif // SCENEOBJECT_H
