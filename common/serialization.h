#pragma once

#include <cstdlib>
#include <cstddef>
namespace serializer {
template <typename T>
void write(std::byte* dataStream, T val, size_t& bytesWritten) {
    *(reinterpret_cast<T*>(dataStream + bytesWritten)) = val;
    bytesWritten += sizeof(T);
}

template <typename T>
T read(const std::byte* data, size_t& pos) {
  T value = *reinterpret_cast<const T*>(data + pos);
  pos += sizeof(T);
  return value;
}
}
