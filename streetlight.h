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

#ifndef STREETLIGHT_H
#define STREETLIGHT_H

#include <Urho3D/Urho3D.h>

#include "sceneobject.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class StreetLight : public SceneObject
{
    URHO3D_OBJECT(StreetLight, SceneObject);
public:
    StreetLight(Context *context);
    void HandleSceneUpdate(StringHash eventType, VariantMap &eventData);
    virtual void OnNodeSet(Node *node);
    static void RegisterObject(Context *context);
    void HandleNodeCollisionStart(StringHash eventType, VariantMap& eventData);
    void SwitchOff();

    Substance GetSubstance(Vector3 position) const { return Substance::Metal; }
private:
    StaticModel* model_;
    Node* lightNode_;
    Light* light_;
    float brightness_;
};

#endif // STREETLIGHT_H
