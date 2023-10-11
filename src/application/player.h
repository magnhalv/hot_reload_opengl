#ifndef HOT_RELOAD_OPENGL_PLAYER_H
#define HOT_RELOAD_OPENGL_PLAYER_H

#include "application.h"

auto update_player(AppState *state, ApplicationInput *app_input) {
    if (app_input->input->move_left.ended_down) {
        state->mesh.transform.position.x -= 5.0f * app_input->dt;
    }
    if (app_input->input->move_right.ended_down) {
        state->mesh.transform.position.x += 5.0f * app_input->dt;
    }
    if (app_input->input->move_up.ended_down) {
        state->mesh.transform.position.z += -5.0f * app_input->dt;
    }
    if (app_input->input->move_down.ended_down) {
        state->mesh.transform.position.z += 5.0f * app_input->dt;
    }
}

#endif //HOT_RELOAD_OPENGL_PLAYER_H
