/* ROC Curve*/

//Given sensor_config = width=25, exp=4, pclass=0.80  


#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>
/*double f(i)
{ 
  static double i = 0.0;
  return ++i;
}
*/
int main()
{
  std::vector<double> v(100);
  //std::generate(v.begin(), v.end(), f(0.0));
  double k = 0.0;
  std::cout << "v: ";
  for (int i=0;i<v.size();i++) {
    v[i]=k;
    k = k+0.01;
    std::cout << v[i] << " ";
  }
  std::cout << "\n";
  double dist = 0.0;
  double exp = 20.0;
  double Pd,Pfa;
  double current_min = 1.0;
  double best_Pd, best_Pfa;
  for (int ii = 0; ii<v.size();ii++){
    Pd = v[ii];
    Pfa = pow(Pd,exp);
    dist = pow( pow((Pfa-0.0),2)+pow((Pd-1.0),2) ,0.5);
    if (dist<current_min){
      current_min = dist;
      best_Pd = Pd;
      best_Pfa = Pfa;
    }
  }
  std::cout<<"exp: "<<exp<<", Pd: "<<best_Pd<<", Pfa: "<<best_Pfa<<std::endl;
  // Initialize with default values 0,1,2,3,4 from a lambda function
  // Equivalent to std::iota(v.begin(), v.end(), 0);
  /* std::generate(v.begin(), v.end(), [n = 0] () mutable { return n++; });
 
  std::cout << "v: ";
  for (auto iv: v) {
    std::cout << iv << " ";
  }
  std::cout << "\n";*/
}
/*
double calcSearchTime(sensor_width,search_area_width,search_area_height){
  double lane_width = sensor_width/2.0;
  double num_lanes = search_area_width/
  double total_dist = search_area_height*(2*num_lanes)+search_area_width;
  return total_dist/2.0;

}*/
