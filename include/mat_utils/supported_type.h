#pragma once

#include <concepts>

template <typename T>
concept SupportedType = std::same_as<T, double> || std::same_as<T, float>;
