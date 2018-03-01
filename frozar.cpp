#include <bits/stdc++.h>

constexpr int MAX_T = 1000000000;
int R, C, F, B, T, N;

struct Point {
  int row, col;

  int distance_to(Point &o) {
    return abs(row-o.row) + abs(col-o.col);
  }
};

struct Ride {
  int id;
  Point start;
  Point end;
  int start_t, finish_t;
  int time_to_drive; // Peut etre precalcule, alors on va pas se gener

  void print() {
    std::cerr << "(" << start.row << "; " << start.col << "); ("
	      << "(" << end.row   << "; " << end.col << "); "
	      << start_t << " " << finish_t << std::endl;
  }
};

std::vector<Ride> rides;
using RideList = std::vector<int>;
std::vector<RideList> solution;

void print_solution() {
  for (auto &rl : solution) {
    std::cout << rl.size() << " ";
    for (auto &v : rl)
      std::cout << v << " ";
    std::cout << std::endl;
  }
}

int time_when_ride_finished(Ride &r, int time_start, Point &car_pos,
			    bool &on_time, int &waiting) {
  int time_to_start = time_start + car_pos.distance_to(r.start);
  if (time_to_start <= r.start_t) {
    waiting = r.start_t - time_to_start;
    time_to_start = r.start_t;
    on_time = true;
  }
  else {
    on_time = false;
    waiting = 0;
  }

  // std::cerr << "(time_to_start + r.time_to_drive): " << (time_to_start + r.time_to_drive) << std::endl;
  // std::cerr << "r.finish_t                       : " << r.finish_t << std::endl;

  if ((time_to_start + r.time_to_drive) > r.finish_t)
      return MAX_T;
  else
      return time_to_start + r.time_to_drive;
}

struct Car {
  int id;
  int time_available;
  Point pos;
};


void naive() {
  std::vector<Car> car_pool;
  for (int i=0; i < F; ++i) {
    car_pool.push_back(Car{i, 0, Point{0, 0}});
    solution.push_back(RideList());
  }

  // On trie les rides par ordre de temps de depart
  auto lambda = [&] (const Ride &a, const Ride &b) {
		  return a.start_t < b.start_t;
		};
  std::sort(rides.begin(), rides.end(), lambda);

  int score = 0;
  
  // Maintenant on gere chaque ride en associant la prochaine voiture dispo
  for (int i=0; i < N; ++i) {
    Ride &r = rides[i];
    
    // Avec quelle voiture on peut obtenir un bonus ?
    bool bonus;
    int finish_time, waiting;

    int  best_time   = MAX_T;
    int  best_car    = -1;
    bool best_bonus  = false;
    int best_waiting = MAX_T;
    //std::cerr << "Ride : " << r.id << std::endl;

    // On verifie chaque voiture
    for (int j=0; j < F; ++j) {
      Car &c = car_pool[j];
      
      // En combien de temps on finit ?
      finish_time = time_when_ride_finished(r, c.time_available, c.pos,
					    bonus, waiting);

      //std::cerr << " - Car " << c.id << " : " << finish_time
      //		<< " " << (bonus ? "on time" : "") << std::endl;

      // On garde le meilleur
      if (finish_time < best_time
	  || (finish_time == best_time && waiting < best_waiting)) {
	best_time  = finish_time;
	best_car   = j;
	best_bonus = bonus;
	best_waiting = waiting;
      }
    }

    if (best_time < r.finish_t) {
      score += r.time_to_drive;
      if (best_bonus)
	score += B;
      
      Car &c = car_pool[best_car];
      solution[c.id].push_back(r.id);
      c.time_available = best_time;
      c.pos = r.end;
    }
  }

  std::cerr << "Final score : " << score << std::endl;

  print_solution();
  std::cout << "Score " << score << std::endl;
}


void naive_2() {
  std::vector<Car> car_pool;
  for (int i=0; i < F; ++i) {
    car_pool.push_back(Car{i, 0, Point{0, 0}});
    solution.push_back(RideList());
  }

  // On trie les rides par ordre de temps de depart
  auto lambda = [&] (const Ride &a, const Ride &b) {
		  return a.start_t < b.start_t;
		};
  std::sort(rides.begin(), rides.end(), lambda);

  int score = 0;
  
  // On verifie chaque voiture
  for (int j=0; j < F; ++j) {
      // std::cerr << std::endl;
      // std::cerr << "car " << j << std::endl;
      Car &c = car_pool[j];

      bool ride_possible = true;
      while (ride_possible)
      {
          std::sort(rides.begin(), rides.end(), lambda);
          int  best_time   = MAX_T;
          int  best_ride   = -1;
          bool best_bonus  = false;
          int best_waiting = MAX_T;
          int  best_score  = -1;
          
          // Maintenant on gere chaque ride en associant la prochaine voiture dispo
          for (int i=0; i < N; ++i) {
              Ride &r = rides[i];
              // std::cerr << "wk ride " << r.id << std::endl;
              if (r.start_t == -1)
              {
                  // std::cerr << "skip " << r.id << std::endl;
                  // std::cerr << std::endl;
                  continue;
              }

              // Avec quelle voiture on peut obtenir un bonus ?
              bool bonus;
              int finish_time, waiting;

              //std::cerr << "Ride : " << r.id << std::endl;

      
              // En combien de temps on finit ?
              finish_time = time_when_ride_finished(r, c.time_available, c.pos,
                                                    bonus, waiting);

              //std::cerr << " - Car " << c.id << " : " << finish_time
              //		<< " " << (bonus ? "on time" : "") << std::endl;

              // On garde le meilleur
              // if (finish_time < best_time
              //     || (finish_time == best_time && waiting < best_waiting)) {
              // if (best_score < r.time_to_drive) {
              // std::cerr << "r.time_to_drive: " << r.time_to_drive << std::endl;
              // std::cerr << "r.finish_t     : " << r.finish_t << std::endl;

              // std::cerr << "finish_time    : " << finish_time << std::endl;
              // std::cerr << "best_time      : " << best_time << std::endl;
              // std::cerr << "waiting        : " << waiting << std::endl;
              // std::cerr << "best_waiting   : " << best_waiting << std::endl;

              // if ((best_score < r.time_to_drive) ||
              //     ((best_score <= r.time_to_drive) && (waiting < best_waiting)) ||
              //     ((best_score <= r.time_to_drive) && (waiting <= best_waiting) && (bonus))) {
              // if (((best_score <= r.time_to_drive) && (waiting < best_waiting)) ||
              //     ((best_score <= r.time_to_drive) && (waiting <= best_waiting) && (bonus))) {
              if ((finish_time < best_time) ||
                  ((finish_time <= best_time) && (waiting < best_waiting)) ||
                  ((finish_time <= best_time) && (waiting <= best_waiting) && (bonus))) {
                  best_time  = finish_time;
                  best_ride  = i;
                  best_bonus = bonus;
                  best_waiting = waiting;
                  best_score = r.time_to_drive;
                  // std::cerr << "best_ride : " << best_ride << std::endl << std::endl;
              }
          }

          if (best_time < T) {
              Ride &r = rides[best_ride];
              // std::cerr << "ride " << r.id << std::endl;
              score += r.time_to_drive;
              if (best_bonus)
                  score += B;
      
              // Car &c = car_pool[best_car];
              solution[c.id].push_back(r.id);
              c.time_available = best_time;
              c.pos = r.end;
              r.start_t = -1;
          }
          else
          {
              ride_possible = false;
          }
      }
  }

  std::cerr << "Final score : " << score << std::endl;

  print_solution();
  std::cout << "Score " << score << std::endl;
}

int main(int argc, char **argv) {
  std::ifstream input_file;
  // Si on a un argument passe sur la ligne de commande, on
  // assigne le buffer de cin sur ce fichier
  if (argc > 1) {
    input_file.open(argv[1]);
    std::cin.rdbuf(input_file.rdbuf());
  }

  std::cin >> R >> C >> F >> N >> B >> T;

  for (int i=0; i < N; ++i) {
    Ride r;
    r.id = i;
    std::cin >> r.start.row >> r.start.col >> r.end.row >> r.end.col
	     >> r.start_t >> r.finish_t;
    r.time_to_drive = r.start.distance_to(r.end);
    rides.push_back(r);
  }

  // Score theorique maximal
  int max_score = 0;
  for (auto &r : rides) {
    max_score += B + r.time_to_drive;
  }
  std::cerr << "Maximum theoretical score = " << max_score << std::endl;

  // Solution naive
  naive();
  // naive_2();

  if (argc > 1)
    input_file.close();

  

  return 0;
}
