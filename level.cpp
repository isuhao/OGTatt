#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Math/Vector2.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Physics/RigidBody.h>

#include "level.h"
#include "tile.h"
#include "wallcollider.h"
#include "ogtattcam.h"

//Todo:
//- boxObject->SetOccluder(true); ???

Level::Level(Context *context, Vector3 position, MasterControl* masterControl):
Object(context)
{
    masterControl_ = masterControl;
    SubscribeToEvent(E_UPDATE, HANDLER(Level, HandleUpdate));
    rootNode_ = masterControl_->world.scene->CreateChild("World");
//    masterControl_->platformMap_[rootNode_->GetID()] = WeakPtr<Dungeon>(this);

    rootNode_->SetPosition(position);
    rigidBody_ = rootNode_->CreateComponent<RigidBody>();

    // Add base tile
    IntVector2 firstCoordPair = IntVector2(0,0);
    tileMap_[firstCoordPair] = new Tile(context_, firstCoordPair, this);
    // Add random tiles
    int addedTiles = 1;
    int worldSize = 512;//Random(32, 128);

    while (addedTiles < worldSize){
        //Pick a random exsisting tile from a list.
        Vector<IntVector2> coordsVector = tileMap_.Keys();
        IntVector2 randomTileCoords = coordsVector[Random((int)coordsVector.Length())];

        //Create a vector of numbers 1 to 4
        /*Vector<int> directions;
        for (int i = 1; i <= 4; i++){directions.Push(i);*/
        //Check neighbours in random orer
        char startDir = Random(1,4);
        for (int direction = startDir; direction < startDir+4; direction++){
            int clampedDir = direction;
            if (clampedDir > 4) clampedDir -= 4;
            if (CheckEmptyNeighbour(randomTileCoords, (TileElement)clampedDir, true))
            {
                IntVector2 newTileCoords = GetNeighbourCoords(randomTileCoords, (TileElement)clampedDir);
                AddTile(newTileCoords);
                addedTiles++;
                /*if (newTileCoords.x_ != 0) {
                    IntVector2 mirrorTileCoords = newTileCoords * IntVector2(-1,1);
                    tileMap_[mirrorTileCoords] = new Tile(context_, mirrorTileCoords, this);
                    addedTiles++;
                }
                if (newTileCoords.y_ != 0) {
                    IntVector2 mirrorTileCoords = newTileCoords * IntVector2(1,-1);
                    tileMap_[mirrorTileCoords] = new Tile(context_, mirrorTileCoords, this);
                    addedTiles++;
                }
                if (newTileCoords.x_ != 0 && newTileCoords.y_ != 0) {
                    IntVector2 mirrorTileCoords = newTileCoords * IntVector2(-1,-1);
                    tileMap_[mirrorTileCoords] = new Tile(context_, mirrorTileCoords, this);
                    addedTiles++;
                }*/
            }
        }
    }

    //Add slots
    AddMissingColliders();
    FixFringe();

    Deselect();
}


void Level::Start()
{
}

void Level::Stop()
{
}

bool Level::EnableSlot(IntVector2 coords)
{
    //collisionMap_[coords]->rootNode_->SetEnabled(true);
}
void Level::EnableSlots()
{
    /*for (int i = 0; i < collisionMap_.Values().Length(); i++)
    {
        if (GetTileType(collisionMap_.Values()[i]->coords_) <= TT_EMPTY)
            EnableSlot(collisionMap_.Values()[i]->coords_);
    }*/
}

bool Level::DisableSlot(IntVector2 coords)
{
    //collisionMap_[coords]->rootNode_->SetEnabled(false);
}
void Level::DisableSlots()
{
    /*
    for (int i = 0; i < collisionMap_.Values().Length(); i++)
    {
        DisableSlot(collisionMap_.Values()[i]->coords_);
    }
    */
}

void Level::Select()
{
    selected_ = true;
    EnableSlots();
}

void Level::Deselect()
{
    selected_ = false;
    DisableSlots();
}

void Level::SetSelected(bool selected)
{
    if (selected == true) Select();
    if (selected == false) Deselect();
}

bool Level::IsSelected() const
{
    return selected_;
}



void Level::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
}

void Level::AddTile(IntVector2 newTileCoords)
{
    tileMap_[newTileCoords] = new Tile(context_, newTileCoords, this);
}

void Level::AddMissingColliders()
{
   Vector<IntVector2> tileCoords = tileMap_.Keys();
    for (uint nthTile = 0; nthTile < tileCoords.Size(); nthTile++){
        for (int element = 0; element <= 4; element++){
            IntVector2 checkCoords = GetNeighbourCoords(tileCoords[nthTile], (TileElement)element);
            if (CheckEmpty(checkCoords, false) && CheckEmpty(checkCoords, true))
                collisionMap_[checkCoords] = new WallCollider(context_, this, checkCoords);
        }
    }
}

void Level::FixFringe()
{
    Vector<SharedPtr<Tile> > tiles = tileMap_.Values();
    for (int tile = 0; tile < tiles.Length(); tile++)
    {
        tiles[tile]->FixFringe();
    }
}

void Level::FixFringe(IntVector2 coords)
{
    for (int coordsOffset = 0; coordsOffset < TE_LENGTH; coordsOffset++)
    {
        IntVector2 neighbourCoords = GetNeighbourCoords(coords, (TileElement)coordsOffset);
        if (!CheckEmpty(neighbourCoords, true)) tileMap_[neighbourCoords]->FixFringe();
    }
}

void Level::SetTileType(IntVector2 coords, TileType type)
{
    tileMap_[coords]->SetBuilding(type);
    FixFringe(coords);
}

bool Level::CheckEmpty(IntVector2 coords, bool checkTiles = true) const
{
    if (checkTiles) return (!tileMap_.Keys().Contains(coords));
    else return (!collisionMap_.Keys().Contains(coords));
}


bool Level::CheckEmptyNeighbour(IntVector2 coords, TileElement element, bool checkTiles = true) const
{
    return CheckEmpty(GetNeighbourCoords(coords, element), checkTiles);
}



IntVector2 Level::GetNeighbourCoords(IntVector2 coords, TileElement element) const
{
    IntVector2 shift = IntVector2::ZERO;
    switch (element){
    case TE_NORTH: shift.y_ =  1; break;
    case TE_EAST:  shift.x_ =  1; break;
    case TE_SOUTH: shift.y_ = -1; break;
    case TE_WEST:  shift.x_ = -1; break;
    case TE_NORTHEAST: shift.x_ =  1; shift.y_ =  1; break;
    case TE_SOUTHEAST: shift.x_ =  1; shift.y_ = -1; break;
    case TE_SOUTHWEST: shift.x_ = -1; shift.y_ = -1; break;
    case TE_NORTHWEST: shift.x_ = -1; shift.y_ =  1; break;
    default: case TE_CENTER: break;
    }
    return coords + shift;
}

TileType Level::GetTileType(IntVector2 coords)
{
    if (!CheckEmpty(coords))
    {
        return tileMap_[coords]->buildingType_;
    }
    else return TT_SPACE;
}

TileType Level::GetNeighbourType(IntVector2 coords, TileElement element)
{
    return GetTileType(GetNeighbourCoords(coords, element));
}

CornerType Level::PickCornerType(IntVector2 tileCoords, TileElement element) const
{
    bool emptyCheck[3] = {false, false, false};
    switch (element){
    case TE_NORTHEAST: {
        emptyCheck[0] = CheckEmptyNeighbour(tileCoords, TE_NORTH);
        emptyCheck[1] = CheckEmptyNeighbour(tileCoords, TE_NORTHEAST);
        emptyCheck[2] = CheckEmptyNeighbour(tileCoords, TE_EAST);
    }
    break;
    case TE_SOUTHEAST: {
        emptyCheck[0] = CheckEmptyNeighbour(tileCoords, TE_EAST);
        emptyCheck[1] = CheckEmptyNeighbour(tileCoords, TE_SOUTHEAST);
        emptyCheck[2] = CheckEmptyNeighbour(tileCoords, TE_SOUTH);
    }break;
    case TE_SOUTHWEST: {
        emptyCheck[0] = CheckEmptyNeighbour(tileCoords, TE_SOUTH);
        emptyCheck[1] = CheckEmptyNeighbour(tileCoords, TE_SOUTHWEST);
        emptyCheck[2] = CheckEmptyNeighbour(tileCoords, TE_WEST);
    }break;
    case TE_NORTHWEST: {
        emptyCheck[0] = CheckEmptyNeighbour(tileCoords, TE_WEST);
        emptyCheck[1] = CheckEmptyNeighbour(tileCoords, TE_NORTHWEST);
        emptyCheck[2] = CheckEmptyNeighbour(tileCoords, TE_NORTH);
    }break;
    default: break;
    }

    int neighbourMask = 0;
    for (int i = 2; i >= 0; i--){
        neighbourMask += !emptyCheck[i] << i;
    }
    switch (neighbourMask){
    case 0: return CT_IN; break;
    case 1: return CT_TWEEN; break;
    case 2: return CT_DOUBLE; break;
    case 3: return CT_NONE; break;
    case 4: return CT_NONE; break;
    case 5: return CT_OUT; break;
    case 6: return CT_NONE; break;
    case 7: return (element == TE_SOUTHEAST) ? CT_FILL : CT_NONE; break;
    default: return CT_NONE; break;
    }
}
