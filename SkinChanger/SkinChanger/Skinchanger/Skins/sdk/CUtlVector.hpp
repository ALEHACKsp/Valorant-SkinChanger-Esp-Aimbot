/*
 * CUtlVector.hpp
 *
 *  Created on: Aug 30, 2017
 *      Author: nullifiedcat
 */

#pragma once

#include "memory.hpp"

namespace sdk
{

template <class T, class I = int> class CUtlMemory
{
public:
    memory::remote_pointer<T> m_pMemory;
    int m_nAllocationCount;
    int m_nGrowSize;
};

template <class T, class A = CUtlMemory<T> > class CUtlVector
{
public:
    A m_Memory;
    int m_Size;
    memory::remote_pointer<T> m_pElements;
};
}
