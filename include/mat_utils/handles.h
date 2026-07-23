#pragma once

#include <mat.h>
#include <matrix.h>

#include <memory>

namespace mat_utils::handles {

inline void destroy_mxArray(mxArray *ptr) { mxDestroyArray(ptr); }
using mxArrayPtr = std::unique_ptr<mxArray, decltype(&destroy_mxArray)>;

inline void close_mat_file(MATFile *ptr) { matClose(ptr); }
using MATFilePtr = std::unique_ptr<MATFile, decltype(&close_mat_file)>;

} // namespace mat_utils::handles
