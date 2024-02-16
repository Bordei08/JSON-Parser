#pragma once
#include <iostream>
#include <variant>
#include <vector>
#include <string>
#include <map>

struct jlist;
struct jobject;

using json_value = std::variant<int, float, bool, std::string, jlist*, jobject*>;

enum tag { list, object };

struct jlist {
    tag type;
    std::vector<json_value*> vector_value;
    jlist() = default;
    ~jlist() {
        for (auto it : vector_value) {
            delete it;
        }
    }
};

struct jobject {
    tag type;
    std::map<std::string, json_value*> map_value;
    jobject() = default;
    ~jobject() {
        for (auto& pair : map_value) {
            delete pair.second;
        }
    }
};