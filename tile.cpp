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
#include "hemp.h"
#include "grass.h"
#include "firepit.h"
#include "streetlight.h"

Tile::Tile(Context *context, const IntVector2 coords, Level *level):
Object(context)
{
    masterControl_ = level->masterControl_;
    dungeon_ = level;
    coords_ = coords;

    rootNode_ = dungeon_->rootNode_->CreateChild("Tile");
    rootNode_->SetPosition(Vector3(static_cast<float>(coords_.x_), 2.0f, static_cast<float>(-coords_.y_)));
    rootNode_->SetScale(Vector3(1.0f, 4.0f, 1.0f));

        StaticModel* model{rootNode_->CreateComponent<StaticModel>()};
        model->SetModel(masterControl_->cache_->GetResource<Model>("Models/Box.mdl"));
//        model->SetMaterial(masterControl_->);
        model->SetCastShadows(true);
}
