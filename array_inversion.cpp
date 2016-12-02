
#include <random>
#include <algorithm>
#include <array>
#include <numeric>
#include <chrono>
#include <future>
#include <iostream>

class generator {
  long long seed_ = 0x123456789abcdef1;
public:
  size_t operator()() {
    seed_ = ((seed_ >> 63) & 0xfedcba987654321f) ^ (seed_ << 1);
    return (size_t)seed_;
  };
  typedef size_t result_type;
  size_t min() { return 0; }
  size_t max() { return (size_t)-1; }
};

size_t next_power_of_two(size_t v) {
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  return v + 1;
}
 
int main()
{
  size_t vsize = 100000000;
  std::vector<size_t> v(vsize);
  std::iota(v.begin(), v.end(), 0);

  generator g;
  std::shuffle(v.begin(), v.end(), g);

  std::vector<size_t> v2(vsize);

  {
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i != vsize; ++i) {
      v2[v[i]] = i;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration<double, std::nano>(end - start).count() / vsize << "\n";
  }

  {
    auto start = std::chrono::high_resolution_clock::now();

    std::array<std::thread, 8> threads;
    for (size_t core = 0; core != threads.size(); ++core) {
      threads[core] = std::thread([core, vsize, &v2, &v]{
        size_t imin = core*(vsize/8);
        size_t imax = std::min(vsize, imin+(vsize/8));
        for (size_t i = imin; i != imax; ++i) {
          v2[v[i]] = i;
        }
      });
    }
    for (size_t core = 0; core != threads.size(); ++core) {
      threads[core].join();
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration<double, std::nano>(end - start).count() / vsize << "\n";
  }

  {
    auto start = std::chrono::high_resolution_clock::now();
    size_t block_size = next_power_of_two(vsize / 0x200);
    std::vector<size_t> dest(vsize/block_size+1);
    std::vector<size_t> tmp(block_size);
    printf("%d\n", (int)dest.size());

    for (size_t i = 0; i != dest.size(); ++i) {
      dest[i] = i * block_size;
    }
    
    for (size_t i = 0; i != vsize; ++i) {
      size_t value = v[i]; 
      v2[dest[value/block_size]++] = i * block_size + value % block_size;
    }

    auto mid = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < vsize; i += block_size) {
      size_t jmax = std::min(vsize - i, block_size);
      for (size_t j = 0; j != jmax; ++j) {
        size_t value = v2[i + j]; 
        tmp[value % block_size] = value / block_size;
      }
      for (size_t j = 0; j != jmax; ++j) {
        v2[i + j] = tmp[j];
      }
    }
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << std::chrono::duration<double, std::nano>(mid - start).count() / vsize << "\n";
    std::cout << std::chrono::duration<double, std::nano>(end - start).count() / vsize << "\n";
  }
}

