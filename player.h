#ifndef PLAYER_H
#define PLAYER_H

#include <person.h>

class Player : public Person
{
public:
    Player(int number);
    int m_number;
    int GetNumber() const noexcept;
    enum Key{Up, Down, Left, Right, Jump, n_keys};
    bool m_keys[n_keys];
    void KeyPress(int key) noexcept;
    void KeyRelease(int key) noexcept;
    void KeyAction() noexcept;
};

#endif // PLAYER_H
