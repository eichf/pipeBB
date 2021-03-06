//
// Copyright 2018- Florian Eich <florian.eich@gmail.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifndef PIPEBB_MOVING_AVERAGE_H_
#define PIPEBB_MOVING_AVERAGE_H_

#include <numeric>

#include "ringbuffer.h"


namespace pipebb {


template <class I, std::size_t N>
class moving_average
{
  using self_t   = moving_average<I, N>;
  using input_t  = I;
  using buffer_t = ringbuffer<typename input_t::value_t, N>;

public:
  using value_t = typename input_t::value_t;

public:
  moving_average(input_t & input, bool use_zeros = true) noexcept
   : _input(input), _use_zeros(use_zeros) {}

  moving_average(self_t && other) = default;

  void reset() noexcept { _buffer.fill(value_t()); }

  value_t report() noexcept {
    return std::accumulate(_buffer.begin(), _buffer.end(), value_t()) / N;
  }

  value_t operator()() noexcept {
    value_t val = _input();
    if (val || _use_zeros) { _buffer << val; }
    /* --------------------------------------------------------------------- *
     * std::reduce is a C++17 feature; use when compiler is updated
     *
     * That is, libstdc++ has to implement execution_policy support, described
     * in proposal P0024R2 (called "The Parallelism TS should be Standardized")
     * first, the implement std::reduce.
     *
     * libstdc++ C++17 progress:
     * https://gcc.gnu.org/onlinedocs/libstdc++/manual/status.html#status.iso.201z
     *
     * If this has happened, uncomment the return call to std::reduce below and
     * get rid of the return call to std::accumulate below that. Afterwards,
     * this comment can be removed.
     * --------------------------------------------------------------------- */
    //
    // return std::reduce(std::execution::par, _buffer.begin(), _buffer.end());
    //

    return std::accumulate(_buffer.begin(), _buffer.end(), value_t()) / N;
  }

private:
  input_t & _input;
  bool      _use_zeros;
  buffer_t  _buffer;
};


template <std::size_t N, class I>
inline moving_average<I, N> make_moving_average(
  I & input, bool use_zeros = false) noexcept {
  return {input, use_zeros};
}

}  // namespace pipebb

#endif  // PIPEBB_MOVING_AVERAGE_H_
