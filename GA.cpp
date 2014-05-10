#include <iostream>
#include <cstdlib>
#include <ctime>
#include <math.h>
#include <string.h>
#include <algorithm>
#include "GA.h"

GA::GA(){
  time_t start_t = time(0);
  get_input();
  reset_button = false;

  srand(time(NULL)); // random 초기화

  time_t end_t = time(0);
  int worst_term = 0;
  Chromosome best;
  GA::init();
  total_best.fitness_score = 2147483647;
  for(int i=0;i<limit_generation;i++){
    //cout << "GENERATION" << i << endl;
    generation = i;
    //cout << "replace" << endl;
    GA::replacement();
    //cout << "selection" << endl;
    GA::selection(); //selection 안에 crossover이 있음.
    //cout << "mutaion" << endl;
    GA::mutation();
    //cout << "change" << endl;
    parent = offspring;
    //cout << offspring_counter<<endl;
    //cout << parent[0].fitness_score<<endl;

    time_t temp  = end_t;
    end_t = time(0);
    int term = end_t - temp; 

    if(term > worst_term){
      worst_term = term;
    }

    if(end_t - start_t > limit_time * 0.90 || generation >= limit_generation-1){

      break;
    }
  }
  end_t = time(0);
   //cout << total_best.fitness_score<< endl;
  // for(int i=0;i<gene_number;i++){
  //   cout << total_best.gene[i].number << " ";
  // }
  // cout << endl;
  //
  set_output();
  delete[] parent;
  delete[] stage_parent;

  delete[] best_fitness_arr;
  delete[] worst_fitness_arr;
  delete[] dominant_number_arr;
  delete[] dominant_fitness_arr;
  delete[] different_number_arr;


}

void GA::set_value(int p, int c, float m, float t, float e, float w, float n, float l){
  population_multify = p;// TODO change this value, population에 곱할 수
  crossover_point_number = c;// TODO change this value, population에 곱할 수
  mutation_ratio = m;// TODO change this value, 변이 비율
  tournament_ratio = t;// TODO change this value, 토너먼트 확률
  elite_ratio = e;//elitism
  worst_elite_ratio = w;//worst elitism
  new_add_ratio = n; // new add ratio
  limit_dominant_ratio = l;
  mutation_distribution.set_value(gene_number, mutation_ratio);
  player_distribution.set_value(10, tournament_ratio);
}
void GA::get_input(){
  ifstream fin("grading1/cycle.in.3");

  fin >> gene_number;

  //set value
  if(gene_number < 25){
    //set_value(100, 3, 0.02, 0.3, 0.001, 0.2, 0, 0.5);
    set_value(100, 3, 0.02, 0.3, 0.0, 0.2, 0, 0.4);
  }else if(gene_number < 55){
    //667    set_value(50, 3, 0.03, 0.1, 0.2, 0.01, 0.00, 1);
    //671 set_value(100, 3, 0.03, 0.1, 0.2, 0.01, 0.00, 1);
    set_value(3, 5, 0.018, 0.1, 0.00, 0.35, 0.00, 1.9);
    //649 set_value(200, 4, 0.01, 0.1, 0.1, 0.01, 0.00, 0.77);
    //set_value(200, 4, 0.01, 0.1, 0.1, 0.01, 0.00, 1);
    //set_value(20, 4, 0.005, 0.1, 0.00, 0.2, 0.0, 1);
  }else if(gene_number < 105){
    //945 set_value(100, 4, 0.01, 0.1, 0.1, 0.01, 0.00, 0.77);
    //set_value(100, 4, 0.01, 0.1, 0.001, 0.01, 0.00, 1);
    //set_value(3, 1, 0.080, 0.6, 0.02, 0.03, 0, 1.1);
    set_value(30, 4, 0.011, 0.1, 0.00, 0.015, 0.00, 1.9);
    //set_value(30, 4, 0.01, 0.1, 0.01, 0.01, 0.00, 1);
    ///set_value(100, 4, 0.01, 0.1, 0.001, 0.01, 0.00, 1);
    //set_value(30, 1, 0.021, 0.1, 0.3, 0.2, 0.00, 1);

  }else if(gene_number < 400){
    set_value(1, 10, 0.011, 0.1, 0.00, 0.15, 0.00, 1.9);
    //set_value(1, 3, 0.03, 0.1, 0.3, 0.1, 0.00, 1);
    //set_value(20, 3, 0.03, 0.3, 0.4, 0.01, 0, 0.7);
  }else{
    set_value(20, 3, 0.03, 0.3, 0.4, 0.01, 0, 0.7);
  }

  input_gene = new Gene[gene_number];

  for(int i = 0; i < gene_number; i++){
    input_gene[i].number = i + 1;
    fin >> input_gene[i].x;
    fin >> input_gene[i].y;
  }
  fin >> limit_time;

  fin.close();
}

void GA::init(){
  population_number = gene_number * population_multify;//TODO change this value if you want to test.
  //parent = new Chromosome[population_number];
  offspring = new Chromosome[population_number];
 for(int i=0;i<population_number;i++){
    offspring[i].gene = new Gene[gene_number];
 }

  limit_generation = 5000000;
  stage_parent  = new Chromosome[limit_generation];

  best_fitness_arr = new float[limit_generation];
  worst_fitness_arr = new float[limit_generation];
  dominant_number_arr = new int[limit_generation];
  dominant_fitness_arr = new float[limit_generation];
  different_number_arr  = new int[limit_generation];

  stage_counter = 0;

  reset_all_offspring();
}

void GA::selection(){
  while(offspring_counter < population_number){
    int parent1_index = tournament();
    int parent2_index = tournament();
    crossover(parent1_index, parent2_index);
  }
}

void GA::crossover(int parent1_index, int parent2_index){
  Gene* offspring_gene = new Gene[gene_number];

  int cut_point[crossover_point_number];
  for(int i=0; i<crossover_point_number;i++){
    int r = (rand() % (gene_number - 1)) ;
    cut_point[i] = r;
  }

  sort(cut_point, cut_point+crossover_point_number);

  int k = 0;
  bool parent_change = true;
  for(int i=0;i<gene_number;i++){
    if(i == cut_point[k]){
      parent_change = !parent_change;
      k++;
    }
    if(parent_change){
      offspring_gene[i] = parent[parent1_index].gene[i];
    }else{
      offspring_gene[i] = parent[parent2_index].gene[i];
    }
  }

  offspring_gene = repair(offspring_gene);

  delete[] offspring[offspring_counter].gene;
  offspring[offspring_counter].gene = offspring_gene;
  //
  
  offspring_counter++;

}

void GA::mutation(){//TODO mutation의 효율을 높일 수 있음. 일단은 정석의 방법대로 했음.
  for(int i = elite_number;i<population_number;i++){
    int mutation_number =mutation_distribution.get_binary_distribution();
    if(mutation_number != 0 && offspring[i].fitness_score != -1){
      int offspring_index = i;
      for(int j=0;j<mutation_number;j++){
         int gene_index1 = (rand() % (gene_number - 1)) + 1 ;
         int gene_index2 = (rand() % (gene_number - 1)) + 1 ;
         Gene temp = offspring[offspring_index].gene[gene_index1];
         offspring[offspring_index].gene[gene_index1] = offspring[offspring_index].gene[gene_index2];
         offspring[offspring_index].gene[gene_index2] = temp;
      }
    }
  }
}
void GA::replacement(){
  parent = offspring;
  elite_number = (int)(population_number * elite_ratio);
  worst_fitness = 0;
  best_fitness = 2147483647;

  int best_chromosome_index[elite_number];
  float best_chromosome_fitness[elite_number];
  int worst_elite_number = (int)(population_number * worst_elite_ratio);

  int worst_chromosome_index[worst_elite_number];
  float worst_chromosome_fitness[worst_elite_number];

  for(int i=0;i<elite_number;i++){
    best_chromosome_index[i] = 0;
    best_chromosome_fitness[i] = 2147483647;
  }

  for(int i=0;i<worst_elite_number;i++){
    worst_chromosome_index[i] = 0;
    worst_chromosome_fitness[i] = 0;
  }


  for(int i=0;i<population_number;i++){
    parent[i].fitness_score = fitness_score(i);

    if(parent[i].fitness_score > worst_fitness){
      worst_fitness = parent[i].fitness_score;
    }
    if(parent[i].fitness_score < best_fitness){
      best_fitness = parent[i].fitness_score;
    }
    if(parent[i].fitness_score < total_best.fitness_score){
      total_best = parent[i];
    }

    if(parent[i].fitness_score <=  best_chromosome_fitness[elite_number-1]){
      int k = elite_number;
      while(k > 1 && parent[i].fitness_score <=  best_chromosome_fitness[k-1]){
        k--;
      }

      for(int j = elite_number-1;j > k;j--){
        best_chromosome_index[j] = best_chromosome_index[j-1];
        best_chromosome_fitness[j] = best_chromosome_fitness[j-1];
      }
      best_chromosome_index[k] = i;
      best_chromosome_fitness[k] = parent[i].fitness_score;
    }


    if(parent[i].fitness_score >= worst_chromosome_fitness[worst_elite_number-1]){
      int k = worst_elite_number;
      while(k > 1 && parent[i].fitness_score >=worst_chromosome_fitness[k-1]){
        k--;
      }

      for(int j = worst_elite_number - 1; j > k;j--){
        worst_chromosome_index[j] = worst_chromosome_index[j-1];
        worst_chromosome_fitness[j] = worst_chromosome_fitness[j-1];
      }

      worst_chromosome_index[k] = i;
      worst_chromosome_fitness[k] = parent[i].fitness_score;
    }
  }


  //offspring = new Chromosome[population_number];
  if(elite_number >0)
    offspring[0] = parent[best_chromosome_index[1]];

  for(int i=1;i<elite_number;i++){
    offspring[i] = parent[best_chromosome_index[i]];
    // cout << i << " : " << best_chromosome_index[i] << endl;
    // cout << best_chromosome_fitness[i] << endl;
    // cout << parent[best_chromosome_index[i]].fitness_score << endl;
    // cout << "-0" << endl;

  }


  for(int i=elite_number;i<elite_number+population_number * new_add_ratio && i<population_number;i++){
    //parent[i].gene = new Gene[gene_number];
    parent[i].gene[0] = input_gene[0]; //첫 Gene은 1로 고정

    for(int j=1;j<gene_number;j++){
      int r = (rand() % (gene_number - j)) + 1;
      parent[i].gene[j] = input_gene[r-1];
    }
  }


  for(int i=1;i<worst_elite_number;i++){
    offspring[worst_chromosome_index[i]].fitness_score = -1;
  }
  offspring_counter = elite_number; //TODO 이걸 주석 시키면 elite가 안먹음
  //offspring_counter = elite_number + population_number * new_add_ratio;

  best_fitness_arr[generation - 1] = best_fitness;
  worst_fitness_arr[generation - 1] = worst_fitness;
//
  //cout  << "best_fitness  : " <<  best_fitness <<endl;
//   cout  << "best_fitness  : " <<  offspring[0].fitness_score <<endl;
//   for(int i=0;i<gene_number;i++){
//     cout  << parent[best_chromosome_index[0]].gene[i].number << " ";
//   }
  //cout << endl;
  //cout  << "worst_fitness : " <<  worst_fitness <<endl;
  float domi_ratio = dominant_ratio(best_fitness);
  //cout << domi_ratio << endl;
  if(domi_ratio > limit_dominant_ratio ||(generation > gene_number*3 && best_fitness_arr[generation-1] == best_fitness_arr[generation-gene_number*3])){
    stage_parent[stage_counter] = parent[0];
    stage_counter++;
    reset_all_offspring();
  }

}
float GA::dominant_ratio(float best_fitness_score){
  int counter = 0;
  for(int i=0;i<population_number;i++){
    if(best_fitness_score == parent[i].fitness_score){
      counter++;
    }
  }
  return (1.0*counter/population_number);
}

Gene* GA::repair(Gene* damaged_gene){
  bool check_gene[gene_number+1];
  damaged_gene[0] = input_gene[0];
  for(int i=1;i<=gene_number;i++){
    check_gene[i] = false;
  }

  int disappeared_gene[gene_number];
  int disappeared_index = 0;
  int duplicated_gene[gene_number];
  int duplicated_index = 0;

  for(int i=0;i<gene_number;i++){
    if(check_gene[damaged_gene[i].number] == false){
      check_gene[damaged_gene[i].number] = true;
    }else{ //중복이 일어나면
      duplicated_gene[duplicated_index] = i; //index i인 곳이 중복되어 있음
      duplicated_index++;
    }
  }
  for(int i=1;i<=gene_number;i++){
    if(check_gene[i] == false){
      disappeared_gene[disappeared_index] = i; //number가 i인 애가 비어있음
      disappeared_index++;
    }
  }
  for(int i=0;i<duplicated_index;i++){
    damaged_gene[duplicated_gene[i]] = input_gene[disappeared_gene[i]-1];
  }

  return damaged_gene;
}
int GA::tournament(){
  int n = 8;
  int player_index[n];
  float best_fitness=0;
  for(int i=0;i<n;i++){
    do{
      player_index[i] = (rand() % population_number);
    }while(parent[player_index[i]].fitness_score == -1);
  }

  for(int i=n/2;i>=1;i=i/2){
    for(int j=0;j<i;j++){
      int large;
      int small;
      if(parent[player_index[j*2]].fitness_score > parent[player_index[j*2+1]].fitness_score){
        large = player_index[j*2];
        small = player_index[j*2+1];
      }else{
        large = player_index[j*2+1];
        small = player_index[j*2];
      }

      if(rand() % 100 > 65){
        player_index[j] = large;
      }else{
        player_index[j] = small;
      }
    }
  }

  // for(int i=0;i<n;i++){
  //   for(int j=0;j<i;j++){
  //     if(parent[player_index[i]].fitness_score < parent[player_index[j]].fitness_score){
  //       int temp = player_index[i];
  //       player_index[i] = player_index[j];
  //       player_index[j] = temp;
  //     }
  //   }
  // }

  //int k = player_distribution.get_binary_distribution();
  return player_index[0];
}

float GA::fitness_score(int chromosome_index){
  float fitness_score = 0;
  for(int i=0;i<gene_number;i++){
    int j = i + 1;
    if(i == gene_number-1){
      j = 0;
    }
    float dx = pow(parent[chromosome_index].gene[i].x - parent[chromosome_index].gene[j].x, 2);
    float dy = pow(parent[chromosome_index].gene[i].y - parent[chromosome_index].gene[j].y, 2);
    fitness_score += sqrt(dx+dy);
  }
  return fitness_score;
}
void GA::set_output(){
  ofstream fout("cycle.out");
  cout << total_best.fitness_score<< endl;
  for(int i=0;i<gene_number;i++){
    fout << total_best.gene[i].number << " ";
  }
  fout << endl;
  fout.close();
}
void GA::reset_all_offspring(){
  int genes[gene_number];
  for(int i=0;i<gene_number;i++){
    genes[i] = i+1;
  }

  for(int i=0;i<population_number;i++){
 //   offspring[i].gene = new Gene[gene_number];
    offspring[i].gene[0] = input_gene[0]; //첫 Gene은 1로 고정

    for(int j=1;j<gene_number;j++){
      int r = (rand() % (gene_number - j)) + 1;
      offspring[i].gene[j] = input_gene[genes[r]-1];
      //swap
      int temp = genes[r];
      genes[r] = genes[gene_number-j];
      genes[gene_number-j] = temp;
    }
  }
  parent = offspring;
}

