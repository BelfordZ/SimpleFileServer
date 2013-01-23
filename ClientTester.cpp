#include "Client.h"

int main() {
  std::string fileName;

  Client c(0);

  std::cout << "File to retrieve: ";
  std::cin >> fileName;

  c.connect("server", fileName);
  //c.listen();
  //c.write_file();
  //c.disconnect();

  return 0;
}
