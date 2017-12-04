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

#include "tile.h"
#include "frop.h"
#include "grass.h"
#include "firepit.h"
#include "streetlight.h"

Tile::Tile(Context* context, const TileInfo& info, Level *level):
    Object(context),
    info_{info},
    level_{level}
{

    rootNode_ = level_->rootNode_->CreateChild("Tile");
    rootNode_->SetPosition(Vector3(info.coords_) * Vector3(1.0f, LAYER_THICKNESS, -1.0f));

    if (info_.obstacle_) {

        CollisionShape* collider{level_->rootNode_->CreateComponent<CollisionShape>()};
        collider->SetBox(Vector3::ONE, rootNode_->GetPosition());
    }
    if (info_.modelName_.Length()) {

        StaticModel* model{rootNode_->CreateComponent<StaticModel>()};
        model->SetModel(MC->GetModel(info.modelName_));
        model->SetCastShadows(true);

        if (info_.materialNames_.Size())
            for (unsigned s{0}; s < info_.materialNames_.Size(); ++s)
                model->SetMaterial(s, MC->GetMaterial(info.materialNames_[s]));
    }
}
