#pragma once 

enum class CommandType {
    Null,
    Play,
    Setting,
    Intro,
} ;

struct Command {
    CommandType type;
};

