/* KO
// Copyright (C) 2015 LucKey Productions (luckeyproductions.nl)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// Commercial licenses are available through frode@lindeijer.nl
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

#ifndef INPUTMASTER_H
#define INPUTMASTER_H

#include <Urho3D/Urho3D.h>
#include "mastercontrol.h"
#include "controllable.h"

enum class MasterAction { UP, RIGHT, DOWN, LEFT, CONFIRM, CANCEL, PAUSE, MENU, SCREENSHOT };
enum class PlayerAction { ACTION0, ACTION1, ACTION2, ACTION3, ACTION4, RESTACTIONS,
                          MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT,
                          FIRE_N, FIRE_NE, FIRE_E, FIRE_SE,
                          FIRE_S, FIRE_SW, FIRE_W, FIRE_NW };

struct InputActions {
    Vector<MasterAction> master_;
    HashMap<int, Vector<PlayerAction>> player_;
};

class Player;

class InputMaster : public Object
{
    URHO3D_OBJECT(InputMaster, Object);
public:
    InputMaster(Context* context);
    void SetPlayerControl(Player *player, Controllable* controllable);
    Player *GetPlayerByControllable(Controllable* controllable);
    Controllable*  GetControllableByPlayer(int playerId);
    Vector<Controllable*>  GetControllables() { return controlledByPlayer_.Values(); }

private:
    HashMap<int, MasterAction> keyBindingsMaster_;
    HashMap<int, MasterAction> buttonBindingsMaster_;
    HashMap<int, HashMap<int, PlayerAction> > keyBindingsPlayer_;
    HashMap<int, HashMap<int, PlayerAction> > buttonBindingsPlayer_;

    Vector<int> pressedKeys_;
    HashMap<int, Vector<LucKey::SixaxisButton> > pressedJoystickButtons_;
    HashMap<int, HashMap<int, float> > axesPosition_;

    HashMap<int, Controllable*> controlledByPlayer_;

    void HandleUpdate(StringHash eventType, VariantMap &eventData);
    void HandleKeyDown(StringHash eventType, VariantMap &eventData);
    void HandleKeyUp(StringHash eventType, VariantMap &eventData);
    void HandleJoystickButtonDown(StringHash eventType, VariantMap &eventData);
    void HandleJoystickButtonUp(StringHash eventType, VariantMap &eventData);
    void HandleJoystickAxisMove(StringHash eventType, VariantMap& eventData);

    void HandleActions(const InputActions &actions);
    void HandlePlayerAction(PlayerAction action, int playerId);
    Vector3 GetMoveFromActions(Vector<PlayerAction>* actions);
    Vector3 GetAimFromActions(Vector<PlayerAction>* actions);
    void Screenshot();

    void PauseButtonPressed();
};

#endif // INPUTMASTER_H
