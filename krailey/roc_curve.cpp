/* ROC Curve*/

//Given sensor_config = width=25, exp=4, pclass=0.80  


#include <algorithm>
#include <iostream>
#include <vector>
 
int f()
{ 
  static double i = 0.0;
  return (i+0.01);
}
 
int main()
{
  std::vector<int> v(100);
  std::generate(v.begin(), v.end(), f);
 
  std::cout << "v: ";
  for (auto iv: v) {
    std::cout << iv << " ";
  }
  std::cout << "\n";
 
  // Initialize with default values 0,1,2,3,4 from a lambda function
  // Equivalent to std::iota(v.begin(), v.end(), 0);
  /* std::generate(v.begin(), v.end(), [n = 0] () mutable { return n++; });
 
  std::cout << "v: ";
  for (auto iv: v) {
    std::cout << iv << " ";
  }
  std::cout << "\n";*/
}

double calcSearchTime(sensor_width,search_area_width,search_area_height){
  double lane_width = sensor_width/2.0;
  double num_lanes = search_area_width/
  double total_dist = search_area_height*(2*num_lanes)+search_area_width;
  return total_dist/2.0;

}
