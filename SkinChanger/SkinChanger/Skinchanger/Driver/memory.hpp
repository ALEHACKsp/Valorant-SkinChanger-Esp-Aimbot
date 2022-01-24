/*
 * memory.hpp
 *
 *  Created on: May 27, 2017
 *      Author: nullifiedcat
 */

#pragma once

#include <sys/types.h>
#include <stdint.h>
#include <errno.h>
#include <sys/uio.h>
#include <stdio.h>

#include <string>
#include <memory>

namespace memory
{

extern pid_t active_process;

inline void set_active_process(pid_t process)
{
    active_process = process;
}

inline void read_buffer(uintptr_t address, unsigned char *out, size_t size)
{
    iovec local_v{ out, size };
    iovec remote_v{ reinterpret_cast<void *>(address), size };
    auto s_read =
        process_vm_readv(active_process, &local_v, 1, &remote_v, 1, 0);
    if (s_read != size)
    {
        printf(
            "WARNING: read %08x bytes from %08x instead of %08x! errno: %d\n",
            s_read, address, size, errno);
    }
}

template <typename T> T read(uintptr_t address)
{
    // tricky allocating on stack - without calling a constructor or anything
    T *result = (T *) alloca(sizeof(T));
    iovec local_v{ result, sizeof(T) };
    iovec remote_v{ reinterpret_cast<void *>(address), sizeof(T) };
    auto s_read =
        process_vm_readv(active_process, &local_v, 1, &remote_v, 1, 0);
    if (s_read != sizeof(T))
    {
        printf(
            "WARNING: read %08x bytes from %08x instead of %08x! errno: %d\n",
            s_read, address, sizeof(T), errno);
    }
    return std::move(*result);
}

template <typename T> void write(uintptr_t address, const T &value)
{
    iovec local_v{ (void *) &value, sizeof(T) };
    iovec remote_v{ reinterpret_cast<void *>(address), sizeof(T) };
    auto s_read =
        process_vm_writev(active_process, &local_v, 1, &remote_v, 1, 0);
    if (s_read != sizeof(T))
    {
        printf("WARNING: written %08x bytes instead of %08x! errno: %d\n",
               s_read, sizeof(T), errno);
    }
}

template <typename T> struct remote_pointer
{
    uintptr_t pointer;

    inline remote_pointer() : pointer(0)
    {
    }
    inline remote_pointer(uintptr_t ptr) : pointer(ptr)
    {
    }

    inline T read() const
    {
        return memory::read<T>(pointer);
    }
    inline void write(const T &data)
    {
        memory::write<T>(pointer, data);
    }
    template <typename U> inline remote_pointer<U> offset(uintptr_t off) const
    {
        return remote_pointer<U>(pointer + off);
    }
    inline remote_pointer<T> point_to_element(size_t index) const
    {
        return remote_pointer<T>(pointer + sizeof(T) * index);
    }
    inline T operator[](size_t index) const
    {
        return memory::read<T>(pointer + sizeof(T) * index);
    }
};
}
