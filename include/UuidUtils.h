#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace uuid_utils {
    void seed(int seed);
    std::string generate_uuid_v4();
}

#endif