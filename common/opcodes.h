#pragma once

enum opcodes {
    JOIN, // join game
    JOIN_SUCCESS, // response to successful join

    SERVER_EVENT,
};

enum server_event {
    JUMP,
    START_MOVE_LEFT,
    START_MOVE_RIGHT,
    END_MOVE_LEFT,
    END_MOVE_RIGHT
};