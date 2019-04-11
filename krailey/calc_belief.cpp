#include <iostream>
#include <string>
#include <math.h>
#include <random>

int main(){
  //input
  std::string label = "25";
  //Access map
  int num_detections=2; // = m_simple_map[label]
  int num_lawnmowers=2;
  int num_hazards=4;
  int num_requests=5;
  //Given
  double Pd = 0.69;
  double Pf = 0.227;
  double Pc = 0.63;
  double penalty_miss = 150;
  double penalty_fa = 30;
  //Detections-----
  // Assuming two passes
  //Given hazard:
  double prob_dd_given_H = pow(Pd,2);
  double prob_dn_given_H = Pd*(1-Pd);
  double prob_nd_given_H = Pd*(1-Pd);
  double prob_nn_given_H = pow((1-Pd),2);
  //Given benign:
  double prob_dd_given_B = pow(Pf,2);
  double prob_dn_given_B = Pd*(1-Pf);
  double prob_nd_given_B = Pd*(1-Pf);
  double prob_nn_given_B = pow((1-Pf),2);

  double decision_ratio_detection;
  bool decision_detection;
  //Cases for detection:
  if (num_detections >= num_lawnmowers){
    decision_ratio_detection = prob_dd_given_H/prob_dd_given_B;
  }
  else if (num_detections == 1){
    decision_ratio_detection = (prob_nd_given_H)/prob_nd_given_B;
  }
  else{
    //Zero detections
  }
  
  if ((penalty_miss/penalty_fa)>decision_ratio_detection){
    decision_detection = true;
  }
 else{
   decision_detection = false;
 }

  //Classifications----------
  bool decision_classification;
  double prob_hazards_in_requests = binom_distribution(Pc, num_hazards, num_requests);
  double decision_ratio_classification = prob_hazards_in_requests/(1-prob_hazards_in_requests);
  if ((penalty_miss/penalty_fa)>(decision_ratio_classification)){
      decision_classification = true;
    }
    else{
      decision_classificiation = false;
    }


  return 0;
}


double binom_distribution(double p,int k,int n){
  double binom_coeff = factorial(n)/(factorial(k)*factorial(n-k));
  double result = binom_coeff*(pow(p,k))*(pow((1-p),(n-k)));
 return result;
}

int factorial(int n){
  int result=1;
  for (int i=0; i<n;i++){
    result=result*(n-i);
  }
  return result;
}
