#pragma once

enum opcodes {
    JOIN, // join game
    JOIN_SUCCESS, // response to successful join

    SERVER_EVENT,
};

enum server_events {
    JUMP,
    MOVE_LEFT,
    MOVE_RIGHT,
};