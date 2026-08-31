#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

struct CanFrame {
    std::uint32_t id;
    std::array<std::uint8_t, 8> data{};
    std::size_t dlc = 0;
};