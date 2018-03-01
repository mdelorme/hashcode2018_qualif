#include <bits/stdc++.h>

int R, C, F, B, T, N;

struct Ride {
  int row_start, col_start, row_end, col_end, start_t, finish_t;

  void print() {
    std::cerr << "(" << row_start << "; " << col_start << "); ("
	      << "(" << row_end   << "; " << col_end << "); "
	      << start_t << " " << finish_t << std::endl;
  }
};

std::vector<Ride> rides;

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
    std::cin >> r.row_start >> r.col_start >> r.row_end >> r.col_end
	     >> r.start_t >> r.finish_t;
    rides.push_back(r);
    r.print();
  }

  if (argc > 1)
    input_file.close();

  return 0;
}
