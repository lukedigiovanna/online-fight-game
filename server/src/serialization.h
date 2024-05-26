#pragma once

#include <cstdlib>
#include <cstddef>

template <typename T>
void write(std::byte* dataStream, T val, size_t& bytesWritten) {
    *(reinterpret_cast<T*>(dataStream + bytesWritten)) = val;
    bytesWritten += sizeof(T);
}
