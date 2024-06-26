#pragma once

#include <cstddef>

#define CT_COUNTER_READ_NS(NS, TAG) \
  decltype(NS::ct_counter_counter(ct_counter::CIndexMax{}, TAG{}))::value

#define CT_COUNTER_READ(TAG) \
  decltype(ct_counter_counter(ct_counter::CIndexMax{}, TAG{}))::value

#define CT_COUNTER_INC(TAG)                                           \
  template<typename...>                                               \
  static constexpr ct_counter::CIndex<CT_COUNTER_READ(TAG)+1>         \
  ct_counter_counter(ct_counter::CIndex<CT_COUNTER_READ(TAG)+1>, TAG)

namespace ct_counter
{

template<std::size_t I>
struct CIndex : CIndex<I-1>
{
  static constexpr std::size_t value = I;
};

template<>
struct CIndex<0>
{
  static constexpr std::size_t value = 0;
};

using CIndexMax = CIndex<255>;

} //namespace ct_counter

template<typename Tag>
static constexpr ct_counter::CIndex<0> ct_counter_counter(ct_counter::CIndex<0>, Tag);
