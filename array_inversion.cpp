
#include <random>
#include <algorithm>
#include <numeric>
 
int main()
{
  std::vector<int> v(1000000);
  std::iota(v.begin(), v.end(), 0);

  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(v.begin(), v.end(), g);


}

