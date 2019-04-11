#include <iostream>
#include <string>
#include <math.h>
#include <random>

// This will fail if n>20
unsigned long long int factorial(long int n){
 
  unsigned long long int result=1;
  for (long int i=0; i<n;i++){
    result=result*(n-i);
  }
  return result;
}

double binom_distribution(double p,int k,int n){
  double binom_coeff = factorial(n)/(factorial(k)*factorial(n-k));
  double result = binom_coeff*(pow(p,k))*(pow((1-p),(n-k)));
  return result;
}


int main(){
  //input
  //std::cout<<"factorial: 20: "<<factorial(20)<<std::endl;
  //std::cout<<" n = 4, k = 2, p =0.7: "<<binom_distribution(0.7,2,4)<<std::endl;

  std::string label = "25";
  //Access map
  int num_detections=1; // = m_simple_map[label]
  int num_lawnmowers=2;
  int num_hazards=1;
  int num_requests=5;
  //Given
  double Pd = 0.7;
  double Pf = 0.3;
  double Pc = 0.6;
  double penalty_miss = 50; //Normalized
  double penalty_fa = 50;
  //Detections-----
  // Assuming two passes
  //Given hazard:
  double prob_dd_given_H = pow(Pd,2);
  double prob_dn_given_H = Pd*(1-Pd);
  double prob_nd_given_H = Pd*(1-Pd);
  double prob_nn_given_H = pow((1-Pd),2);
  //Given benign:
  double prob_dd_given_B = pow(Pf,2);
  double prob_dn_given_B = Pf*(1-Pf);
  double prob_nd_given_B = Pf*(1-Pf);
  double prob_nn_given_B = pow((1-Pf),2);
  std::cout<<"Results: "<<prob_dd_given_H<<","<<prob_nd_given_H<<","<<prob_nn_given_H<<std::endl;
  std::cout<<"Results: "<<prob_dd_given_B<<","<<prob_nd_given_B<<","<<prob_nn_given_B<<std::endl;

  //double ratio_penalty = penalty_miss/(penalty_fa);
  double decision_ratio_detection;
  bool decision_detection;
  //Cases for detection:
   if (num_detections >= num_lawnmowers){
    decision_ratio_detection = prob_dd_given_H;
  }
  else if (num_detections == 1){
    decision_ratio_detection = (prob_nd_given_H)*2.0;
  }
  else{
    //Zero detections
  }
  
  if (decision_ratio_detection>0.3){
    decision_detection = true;
  }
 else{
   decision_detection = false;
   } //COMMENT HERE

  //std::cout<<"ratio penalty: "<<ratio_penalty<<", decision ratio: "<<decision_ratio_detection<<", decision: "<<decision_detection<<std::endl;
  //Classifications----------
  bool decision_classification;
  //Add check if n>20, then binom will fail
  if (num_requests>20){
    if (num_hazards/num_requests>0.3){
      decision_classification = true;
    }
    else{
      decision_classification = false;
    } 
  }
  else if (num_requests<20){

    double prob_hazards_in_requests = binom_distribution(Pc, num_hazards, num_requests);
    std::cout<<"prob of hazards in requests: "<<prob_hazards_in_requests<<std::endl;
    double decision_ratio_classification = prob_hazards_in_requests/(1-prob_hazards_in_requests); //normalized
    std::cout<<"decision ratio classification: "<<decision_ratio_classification<<std::endl;
    if (decision_ratio_classification>0.3){ //try normalizing
      decision_classification = true;
    }
    else{
      decision_classification = false;
    }
  }

  bool decision_final;
  std::cout<<"Decision: "<<decision_detection<<", "<<decision_classification<<std::endl;
  if (num_requests>2){
    decision_final = decision_classification;
  }
  else{
    decision_final = decision_detection;
  }
  std::cout<<"final decision: "<<decision_final<<std::endl;
  return 0;
}


/*double binom_distribution(double p,int k,int n){
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
  }*/
