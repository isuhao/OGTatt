#ifndef PLAYER_H
#define PLAYER_H

#include <person.h>

class Player : public Person
{
public:
    Player(int number);
    int m_number;
    int GetNumber() const noexcept;

    bool m_keys[n_keys];
    void KeyPress(int key) noexcept;
    void KeyRelease(int key) noexcept;
    void KeyAction() noexcept;
};

#endif // PLAYER_H
