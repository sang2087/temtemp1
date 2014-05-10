#include <fstream>
#include "BD.h"
using namespace std;

struct Gene {
  int number;
  float x;
  float y;
};

struct Chromosome {
  Gene* gene;//Memory 동적 할당 해야함.
  float fitness_score;
};

class GA{
  public:
    GA();


  private:
    bool reset_button;
    Chromosome* parent;
    Chromosome* offspring;
    int offspring_counter;
    int population_number;

    Chromosome* stage_parent;
    Chromosome total_best;
    int stage_counter;

    //input setting
    int gene_number;
    Gene* input_gene;
    float limit_time;

    //setting value
    int rulletwheel_k;
    float mutation_ratio;
    int population_multify;
    int crossover_point_number;
    int limit_generation;
    int elite_number;
    float elite_ratio;
    float worst_elite_ratio;
    float new_add_ratio;
    int output_number;

    int generation;
    int running_time;
    float* dominant_fitness_arr;
    int* dominant_number_arr;
    int* different_number_arr;

    //rulletwheel
    float worst_fitness;
    float best_fitness;
    float* worst_fitness_arr;
    float* best_fitness_arr;

    BD mutation_distribution;
    BD player_distribution;
    float tournament_ratio;
    float limit_dominant_ratio;

    void set_value(int p, int c, float m, float t, float e, float w, float n, float l);
    float dominant_ratio(float best_fitness_score);
    float* make_rulletwheel();
    int point_rulletwheel(float* rulletwheel);
    int rulletwheel_counter;

    int tournament();

    void get_input();
    void init();
    void selection();
    void crossover(int parent1_index, int parent2_index);
    void mutation();
    void replacement();

    void set_output();
    float fitness_score(int chromosome_index);
    Gene* repair(Gene* damaged_gene);

    void reset_all_offspring();

    void diversity();
};
