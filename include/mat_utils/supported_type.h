#pragma once

#include <concepts>

namespace mat_utils {

template <typename T>
concept SupportedType = std::same_as<T, double> || std::same_as<T, float>;

}
