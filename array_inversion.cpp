
#include <random>
#include <algorithm>
#include <array>
#include <numeric>
#include <chrono>
#include <future>
#include <iostream>
 
int main()
{
  size_t vsize = 100000000;
  std::vector<size_t> v(vsize);
  std::iota(v.begin(), v.end(), 0);

  std::random_device rd;
  std::ranlux24_base g(rd());
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

    {
      std::array<std::future<void>, 8> asyncs;
      
      for (size_t core = 0; core != asyncs.size(); ++core) {
        asyncs[core] = std::async(std::launch::async, [core, vsize, &v2, &v]{
          size_t imin = core*(vsize/8);
          size_t imax = std::min(vsize, imin+(vsize/8));
          //printf("%x..%x\n", imin, imax);
          for (size_t i = imin; i != imax; ++i) {
            v2[v[i]] = i;
          }
        });
      }
      for (size_t core = 0; core != asyncs.size(); ++core) {
        asyncs[core].wait();
      }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration<double, std::nano>(end - start).count() / vsize << "\n";
  }

  {
    auto start = std::chrono::high_resolution_clock::now();
    size_t block_size = 0x100000;
    std::vector<size_t> dest(vsize/block_size+1);
    std::vector<size_t> tmp(block_size);

    for (size_t i = 0; i != dest.size(); ++i) {
      dest[i] = i * block_size;
    }
    
    for (size_t i = 0; i != vsize; ++i) {
      size_t value = v[i]; 
      v2[dest[value/block_size]++] = i * block_size + value % block_size;
    }

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

    /*for (size_t i = 0; i != vsize; ++i) {
      std::cout << v[i] << " ";
    }
    std::cout << "\n";

    for (size_t i = 0; i != vsize; ++i) {
      std::cout << v2[i] << " ";
    }
    std::cout << "\n";*/

    std::cout << std::chrono::duration<double, std::nano>(end - start).count() / vsize << "\n";
  }
}

