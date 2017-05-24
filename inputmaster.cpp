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

#include "ogtattcam.h"
#include "player.h"

#include "inputmaster.h"

using namespace LucKey;

InputMaster::InputMaster(Context* context):
    Object(context)
{
    keyBindingsMaster_[KEY_UP]     = buttonBindingsMaster_[static_cast<int>(SixaxisButton::SB_DPAD_UP)]    = MasterAction::UP;
    keyBindingsMaster_[KEY_DOWN]   = buttonBindingsMaster_[static_cast<int>(SixaxisButton::SB_DPAD_DOWN)]  = MasterAction::DOWN;
    keyBindingsMaster_[KEY_LEFT]   = buttonBindingsMaster_[static_cast<int>(SixaxisButton::SB_DPAD_LEFT)]  = MasterAction::LEFT;
    keyBindingsMaster_[KEY_RIGHT]  = buttonBindingsMaster_[static_cast<int>(SixaxisButton::SB_DPAD_RIGHT)] = MasterAction::RIGHT;
    keyBindingsMaster_[KEY_RETURN] = buttonBindingsMaster_[static_cast<int>(SixaxisButton::SB_CROSS)]      = MasterAction::CONFIRM;
    keyBindingsMaster_[KEY_ESCAPE] = buttonBindingsMaster_[static_cast<int>(SixaxisButton::SB_CIRCLE)]     = MasterAction::CANCEL;
    keyBindingsMaster_[KEY_P]      = buttonBindingsMaster_[static_cast<int>(SixaxisButton::SB_START)]      = MasterAction::PAUSE;
    keyBindingsMaster_[KEY_ESCAPE] = MasterAction::MENU;

    keyBindingsPlayer_[1][KEY_W]      = PlayerAction::MOVE_UP;
    keyBindingsPlayer_[1][KEY_S]      = PlayerAction::MOVE_DOWN;
    keyBindingsPlayer_[1][KEY_A]      = PlayerAction::MOVE_LEFT;
    keyBindingsPlayer_[1][KEY_D]      = PlayerAction::MOVE_RIGHT;
    keyBindingsPlayer_[1][KEY_LSHIFT] = PlayerAction::ACTION0;
    keyBindingsPlayer_[1][KEY_SPACE]  = PlayerAction::ACTION1;
    keyBindingsPlayer_[1][KEY_RETURN] = PlayerAction::ACTION2;
    keyBindingsPlayer_[1][KEY_TAB]    = PlayerAction::ACTION3;
    keyBindingsPlayer_[1][KEY_KP_8]   = PlayerAction::FIRE_N;
    keyBindingsPlayer_[1][KEY_KP_5]   = PlayerAction::FIRE_S;
    keyBindingsPlayer_[1][KEY_KP_2]   = PlayerAction::FIRE_S;
    keyBindingsPlayer_[1][KEY_KP_4]   = PlayerAction::FIRE_W;
    keyBindingsPlayer_[1][KEY_KP_6]   = PlayerAction::FIRE_E;
    keyBindingsPlayer_[1][KEY_KP_9]   = PlayerAction::FIRE_NE;
    keyBindingsPlayer_[1][KEY_KP_3]   = PlayerAction::FIRE_SE;
    keyBindingsPlayer_[1][KEY_KP_1]   = PlayerAction::FIRE_SW;
    keyBindingsPlayer_[1][KEY_KP_7]   = PlayerAction::FIRE_NW;

    keyBindingsPlayer_[2][KEY_UP]     = PlayerAction::MOVE_UP;
    keyBindingsPlayer_[2][KEY_DOWN]   = PlayerAction::MOVE_DOWN;
    keyBindingsPlayer_[2][KEY_LEFT]   = PlayerAction::MOVE_LEFT;
    keyBindingsPlayer_[2][KEY_RIGHT]  = PlayerAction::MOVE_RIGHT;
    keyBindingsPlayer_[2][KEY_RSHIFT] = PlayerAction::ACTION0;

    for (int p{1}; p <= 4; ++p) {

        buttonBindingsPlayer_[p][SB_CROSS]    = PlayerAction::ACTION0; //Run / Nitro
        buttonBindingsPlayer_[p][SB_CIRCLE]   = PlayerAction::ACTION1; //Jump / Handbrake
        buttonBindingsPlayer_[p][SB_SQUARE]   = PlayerAction::ACTION2; //Enter/Leave vehicle
        buttonBindingsPlayer_[p][SB_TRIANGLE] = PlayerAction::ACTION3; //Burp/Fart/Horn
    }

    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(InputMaster, HandleKeyDown));
    SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(InputMaster, HandleKeyUp));
    SubscribeToEvent(E_JOYSTICKBUTTONDOWN, URHO3D_HANDLER(InputMaster, HandleJoystickButtonDown));
    SubscribeToEvent(E_JOYSTICKBUTTONUP, URHO3D_HANDLER(InputMaster, HandleJoystickButtonUp));
    SubscribeToEvent(E_JOYSTICKAXISMOVE, URHO3D_HANDLER(InputMaster, HandleJoystickAxisMove));
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(InputMaster, HandleUpdate));
}

void InputMaster::HandleUpdate(StringHash eventType, VariantMap &eventData)
{ (void)eventType; (void)eventData;

    InputActions activeActions{};
    for (Player* p : MC->GetPlayers()){

        int pId{ p->GetPlayerId() };
        Vector<PlayerAction> emptyActions{};
        activeActions.player_[pId] = emptyActions;
    }

    //Convert key presses to actions
    for (int key : pressedKeys_){
        //Check for master key presses
        if (keyBindingsMaster_.Contains(key)){
            MasterAction action{keyBindingsMaster_[key]};
            if (!activeActions.master_.Contains(action))
                activeActions.master_.Push(action);
        }
        //Check for player key presses
        for (Player* p : MC->GetPlayers()){

            int pId{ p->GetPlayerId() };
            if (keyBindingsPlayer_[pId].Contains(key)){
                PlayerAction action{keyBindingsPlayer_[pId][key]};
                if (!activeActions.player_[pId].Contains(action))
                    activeActions.player_[pId].Push(action);
            }
        }
    }
    //Check for joystick button presses
    for (Player* p : MC->GetPlayers()){

        int pId{ p->GetPlayerId() };
        for (int button : pressedJoystickButtons_[pId-1])
            if (buttonBindingsPlayer_[pId].Contains(button)){
                PlayerAction action{ buttonBindingsPlayer_[pId][button]};
                if (!activeActions.player_[pId].Contains(action))
                    activeActions.player_[pId].Push(action);
            }
    }

    //Handle the registered actions
    HandleActions(activeActions);
}

void InputMaster::HandleActions(const InputActions& actions)
{
    //Handle master actions
    for (MasterAction action : actions.master_){
        switch (action){
        case MasterAction::UP:                 break;
        case MasterAction::DOWN:               break;
        case MasterAction::LEFT:               break;
        case MasterAction::RIGHT:              break;
        case MasterAction::CONFIRM:            break;
        case MasterAction::CANCEL:             break;
        case MasterAction::PAUSE:              break;
        case MasterAction::MENU:               break;
        default: break;
        }
    }

    //Handle player actions
    for (Player* p : MC->GetPlayers()){

        int pId{ p->GetPlayerId() };
        auto playerInputActions = actions.player_[pId];

        Controllable* controlled{ controlledByPlayer_[pId] };
        if (controlled){

            Vector3 stickMove{ Vector3(axesPosition_[pId-1][0], 0.0f, -axesPosition_[pId-1][1]) };
            Vector3 stickAim{  Vector3(axesPosition_[pId-1][2], 0.0f, -axesPosition_[pId-1][3]) };

            controlled->SetMove(GetMoveFromActions(playerInputActions) + stickMove);
            controlled->SetAim(GetAimFromActions(playerInputActions) + stickAim);

            std::bitset<4>restActions{};
            for (int a{0}; a < static_cast<int>(PlayerAction::RESTACTIONS); ++a) {

                restActions[a] = playerInputActions->Contains(static_cast<PlayerAction>(a));
            }

            controlled->SetActions(restActions);
        }
    }
}

void InputMaster::Screenshot()
{
    Image screenshot(context_);
    Graphics* graphics{ GetSubsystem<Graphics>() };
    graphics->TakeScreenShot(screenshot);
    //Here we save in the Data folder with date and time appended
    String fileName{ GetSubsystem<FileSystem>()->GetProgramDir() + "Screenshots/Screenshot_" +
            Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_')+".png" };
    //Log::Write(1, fileName);
    screenshot.SavePNG(fileName);
}

void InputMaster::HandleKeyDown(StringHash eventType, VariantMap &eventData)
{
    int key{ eventData[KeyDown::P_KEY].GetInt() };
    if (!pressedKeys_.Contains(key)) pressedKeys_.Push(key);

    switch (key){
    case KEY_ESCAPE:{
        MC->Exit();
    } break;
    case KEY_9:{
        Screenshot();
    } break;
    default: break;
    }
}

void InputMaster::HandleKeyUp(StringHash eventType, VariantMap &eventData)
{ (void)eventType;

    int key{ eventData[KeyUp::P_KEY].GetInt() };
    if (pressedKeys_.Contains(key)) pressedKeys_.Remove(key);
}

void InputMaster::HandleJoystickButtonDown(StringHash eventType, VariantMap &eventData)
{ (void)eventType;

    int joystickId{ eventData[JoystickButtonDown::P_JOYSTICKID].GetInt() };
    SixaxisButton button{ static_cast<SixaxisButton>(eventData[JoystickButtonDown::P_BUTTON].GetInt()) };
    if (!pressedJoystickButtons_[joystickId].Contains(button)) pressedJoystickButtons_[joystickId].Push(button);
}
void InputMaster::HandleJoystickButtonUp(StringHash eventType, VariantMap &eventData)
{ (void)eventType;

    int joystickId{ eventData[JoystickButtonDown::P_JOYSTICKID].GetInt() };
    SixaxisButton button{ static_cast<SixaxisButton>(eventData[JoystickButtonUp::P_BUTTON].GetInt()) };
    if (pressedJoystickButtons_[joystickId].Contains(button)) pressedJoystickButtons_[joystickId].Remove(button);
}

void InputMaster::HandleJoystickAxisMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{ (void)eventType;

    int joystickId{ eventData[JoystickAxisMove::P_JOYSTICKID].GetInt() };
    int axis{ eventData[JoystickAxisMove::P_AXIS].GetInt() };
    float position{ eventData[JoystickAxisMove::P_POSITION].GetFloat() };

    axesPosition_[joystickId][axis] = position;
}

Vector3 InputMaster::GetMoveFromActions(Vector<PlayerAction>* actions)
{
    return Vector3{Vector3::RIGHT *
                (actions->Contains(PlayerAction::MOVE_RIGHT) -
                 actions->Contains(PlayerAction::MOVE_LEFT))

                + Vector3::FORWARD *
                (actions->Contains(PlayerAction::MOVE_UP) -
                 actions->Contains(PlayerAction::MOVE_DOWN))};
}
Vector3 InputMaster::GetAimFromActions(Vector<PlayerAction>* actions)
{
    return Vector3{ Vector3::RIGHT *
                (actions->Contains(PlayerAction::FIRE_E) -
                 actions->Contains(PlayerAction::FIRE_W))

                + Vector3::FORWARD *
                (actions->Contains(PlayerAction::FIRE_N) -
                 actions->Contains(PlayerAction::FIRE_S))
                + Quaternion(45.0f, Vector3::UP) * (Vector3::RIGHT *
                (actions->Contains(PlayerAction::FIRE_SE) -
                 actions->Contains(PlayerAction::FIRE_NW))

                + Vector3::FORWARD *
                (actions->Contains(PlayerAction::FIRE_NE) -
                 actions->Contains(PlayerAction::FIRE_SW)))};
}

void InputMaster::SetPlayerControl(Player* player, Controllable* controllable)
{
    int playerId{ player->GetPlayerId() };

    if (controlledByPlayer_.Contains(playerId)){
        if (controlledByPlayer_[playerId] == controllable)
            return;
        controlledByPlayer_[playerId]->ClearControl();
    }
    controlledByPlayer_[playerId] = controllable;
}

Player* InputMaster::GetPlayerByControllable(Controllable* controllable)
{
    for (int k : controlledByPlayer_.Keys())
    {
        if (controlledByPlayer_[k] == controllable)
            return MC->GetPlayer(k);
    }
    return nullptr;
}
Controllable* InputMaster::GetControllableByPlayer(int playerId)
{

    return controlledByPlayer_[playerId];
}
