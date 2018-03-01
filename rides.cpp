#include <bits/stdc++.h>
#include <omp.h>

constexpr int MAX_T = 1000000000;
int R, C, F, B, T, N;
int max_score        = 0;
int bonus_rides      = 0;
int tot_waiting_time = 0;
int finished_rides   = 0;
int score            = 0;
int unoccupied_cars  = 0;
std::vector<bool> finished;


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

// Affichage de la solution
void print_solution() {
  for (auto &rl : solution) {
    std::cout << rl.size() << " ";

    if (rl.size() == 0)
      unoccupied_cars++;
    
    for (auto &v : rl)
      std::cout << v << " ";
    std::cout << std::endl;
  }
}

// En combien de temps on finit une ride ?
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

  return time_to_start + r.time_to_drive;
}

struct Car {
  int id;
  int time_available;
  Point pos;
};

void score_ride_list(RideList &rl, int &rl_score, std::vector<int> new_missed) {
  new_missed.clear();
  Point pos {0, 0};
  int cur_time = 0;
  rl_score = 0;
  for (auto &rid : rl) {
    Ride &r = rides[rid];
    bool bonus;
    int waiting;
    
    int finish_time = time_when_ride_finished(r, cur_time, pos, bonus, waiting);

    if (finish_time < r.finish_t) {
      cur_time = finish_time;
      pos = r.end;
      rl_score += r.time_to_drive;
      if (bonus)
	rl_score += B;
    }
    else
      new_missed.push_back(r.id);
  }
}


// Solution naive pour gros bonus :
// Pour chaque ride, on regarde si une voiture peut scorer le bonus
// Si oui, on minimise le temps d'attente, et on passe a la ride suivante
void naive(bool swap) {
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

    auto lambda_car = [&] (const Car &a, const Car &b) {
			return a.time_available > b.time_available;
		      };

    std::sort(car_pool.begin(), car_pool.end(), lambda_car);

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

      finished_rides++;
      
      if (best_bonus) {
	score += B;
	bonus_rides++;
      }

      tot_waiting_time += best_waiting;
      
      Car &c = car_pool[best_car];
      solution[c.id].push_back(r.id);
      c.time_available = best_time;
      c.pos = r.end;
    }
  }

  if (swap) {
    // Quels rides ont ete manques ?
    std::vector<int> missed;
    std::vector<bool> attributed(N, false);
    
    for (auto &rl : solution) {
      for (auto id : rl)
	attributed[id] = true;
    }
    
    int n_missed = 0;
    for (int i=0; i < N; ++i) {
      if (!attributed[i]) {
	missed.push_back(i);
	n_missed++;
      }
    }
    
    // On trie les ride par ordre d'importance
    auto lambda_missed = [&] (const int &a, const int &b) {
			   return rides[a].time_to_drive > rides[b].time_to_drive;
			 };
    auto lambda_ride = [&] (const int &a, const int &b) {
			 return rides[a].finish_t < rides[b].finish_t;
		       };
    // On essaie de les caser ou on peut pour augmenter le score
    while (missed.size() > 0) {
      std::sort(missed.begin(), missed.end(), lambda_missed);

      for (int cid=0; cid < F; ++cid) {
	// On stocke son ancienne RideList
	RideList rl = solution[cid];
	int cur_score;
	std::vector<int> new_missed;
	score_ride_list(rl, cur_score, new_missed);
	
	// On ajoute le ride manquant
	rl.push_back(missed[0]);
	std::sort(rl.begin(), rl.end(), lambda_ride);
	
	int new_score;
	score_ride_list(rl, new_score, new_missed);
	if (new_score > cur_score) {
	  int delta = new_score - cur_score;
	  score += delta;
	  solution[cid] = rl;
	  for (auto nm : new_missed)
	    missed.push_back(nm);
	  break;
	}
      }

      missed.erase(missed.begin());
    }
  }
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
  for (auto &r : rides) {
    max_score += B + r.time_to_drive;
  }
  std::cerr << "Maximum theoretical score = " << max_score << std::endl;

  // Solution naive si le bonus est "important"
  if (B > 10)
    naive(true);
  else
    naive(true);

  // Score, solution et stats
  print_solution();

  std::cerr << "Final score : " << score << std::endl;
  std::cerr << "Delta : " << max_score - score << std::endl;
  std::cerr << "Statistics : " << std::endl;
  std::cerr << " - Rides completed : " << finished_rides << " / "
	    << N << "(" << finished_rides * 100.0f / N << "%)" << std::endl;
  std::cerr << " - Bonus completed : " << bonus_rides << " / "
	    << finished_rides << "("
	    << bonus_rides * 100.0f / finished_rides << "%)" << std::endl;
  std::cerr << " - Bonus points to claim : "
	    << (finished_rides - bonus_rides) * B << std::endl;
  std::cerr << " - Total waiting time : " << tot_waiting_time << std::endl;
  std::cerr << " - Unoccupied cars : " << unoccupied_cars
	    << " / " << F << "(" << unoccupied_cars*100.0f / F
	    << "%)" << std::endl;
  std::cout << "Score " << score << std::endl;


  
  if (argc > 1)
    input_file.close();

  return 0;
}
