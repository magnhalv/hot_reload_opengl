#ifndef HOT_RELOAD_OPENGL_USER_INPUT_H
#define HOT_RELOAD_OPENGL_USER_INPUT_H

struct ButtonState {
    ButtonState(): half_transition_count(0), ended_down(false) {}

    i32 half_transition_count; // How many times it flipped between up and down
    bool ended_down;
};

struct MouseInput {
    i32 x = 0;
    i32 y = 0;
    i32 dx = 0;
    i32 dy = 0;
    ButtonState buttons[2];
};

struct UserInput {
    MouseInput mouse;
    union
    {
        ButtonState buttons[6];
        struct {
            ButtonState move_up;
            ButtonState move_down;
            ButtonState move_left;
            ButtonState move_right;

            ButtonState space;

            //Note: All buttons must be added above this line
            ButtonState terminator;
        };
    };

    void frame_clear(const UserInput &prev_frame_input) {
        //mouse.x = prev_frame_input.mouse.x;
        //mouse.y = prev_frame_input.mouse.y;
        mouse.dx = 0;
        mouse.dy = 0;

        for (auto i = 0; i < 5; i++) {
            buttons[i].half_transition_count = 0;
            buttons[i].ended_down = prev_frame_input.buttons[i].ended_down;
        }
    }
};

#endif //HOT_RELOAD_OPENGL_USER_INPUT_H
