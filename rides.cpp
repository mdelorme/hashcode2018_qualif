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
			    bool &valid, bool &on_time) {
  int time_to_start = time_start + car_pos.distance_to(r.start);
  if (time_to_start <= r.start_t) {
    time_to_start = r.start_t;
    on_time = true;
  }
  else
    on_time = false;

  int total = time_to_start + r.time_to_drive;
  if (total < r.finish_t)
    valid = true;
  else
    valid = false;

  return total;
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

  // Lambda pour trier les voitures
  auto lambda_car = [&] (const Car &a, const Car &b) {
		      return a.time_available < b.time_available;
		    };

  int score = 0;
  
  // Maintenant on gere chaque ride en associant la prochaine voiture dispo
  for (int i=0; i < N; ++i) {
    Ride &r = rides[i];
    
    // On trie les voitures
    std::sort(car_pool.begin(), car_pool.end(), lambda_car);

    // Avec quelle voiture on peut obtenir un bonus ?
    bool valid, bonus;
    int finish_time;

    int  best_time  = MAX_T;
    int  best_car   = -1;
    bool best_bonus = false;
    bool best_valid = false;

    // On verifie chaque voiture
    for (int j=0; j < F; ++j) {
      Car &c = car_pool[j];
      
      // En combien de temps on finit ?
      finish_time = time_when_ride_finished(r, c.time_available, c.pos,
					    valid, bonus);

      // On garde le meilleur
      if (finish_time < best_time || (!best_bonus && bonus)) {
	best_time  = finish_time;
	best_car   = j;
	best_bonus = bonus;
	best_valid = valid;
      }
    }

    if (best_valid) {
      score += r.time_to_drive + (best_bonus ? B : 0);
      Car &c = car_pool[best_car];
      solution[c.id].push_back(i);
      c.time_available = best_time;
      c.pos = r.end;
    }
  }

  std::cerr << "Final score : " << score << std::endl;

  print_solution();
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

  if (argc > 1)
    input_file.close();

  

  return 0;
}
