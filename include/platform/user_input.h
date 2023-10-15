#ifndef HOT_RELOAD_OPENGL_USER_INPUT_H
#define HOT_RELOAD_OPENGL_USER_INPUT_H

#include <cassert>

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

struct ButtonState {
    ButtonState(): half_transition_count(0), ended_down(false) {}

    i32 half_transition_count; // How many times it flipped between up and down
    bool ended_down;

    /// @brief Is pressed this frame
    [[nodiscard]] auto is_pressed() const -> bool {
        return ended_down && half_transition_count == 1;
    }
};

struct MouseInput {
    i32 x = 0;
    i32 y = 0;
    i32 dx = 0;
    i32 dy = 0;
    ButtonState buttons[2];
};

const i32 NUM_BUTTONS = 7;

struct UserInput {
    MouseInput mouse;
    union
    {
        ButtonState buttons[NUM_BUTTONS + 1];
        struct {
            ButtonState move_up;
            ButtonState move_down;
            ButtonState move_left;
            ButtonState move_right;

            ButtonState space;
            ButtonState r;
            ButtonState p;

            //Note: All buttons must be added above this line
            ButtonState terminator;
        };
    };

    void frame_clear(const UserInput &prev_frame_input) {
        mouse.dx = 0;
        mouse.dy = 0;

        assert(NUM_BUTTONS+1 == ArrayCount(buttons));

        for (auto i = 0; i < NUM_BUTTONS; i++) {
            buttons[i].half_transition_count = 0;
            buttons[i].ended_down = prev_frame_input.buttons[i].ended_down;
        }
    }
};

#endif //HOT_RELOAD_OPENGL_USER_INPUT_H
