#include <bits/stdc++.h>

int R, C, F, B, T, N;

struct Point {
  int row, col;
};

struct Ride {
  Point start;
  Point end;
  int start_t, finish_t;

  void print() {
    std::cerr << "(" << start.row << "; " << start.col << "); ("
	      << "(" << end.row   << "; " << end.col << "); "
	      << start_t << " " << finish_t << std::endl;
  }
};

std::vector<Ride> rides;

int time_to_finish_ride(Ride &r, Point &car_pos) {

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
    rides.push_back(r);
    r.print();
  }

  if (argc > 1)
    input_file.close();

  

  return 0;
}
