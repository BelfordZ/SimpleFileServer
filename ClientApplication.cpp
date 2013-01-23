#include "Client.h"

int main() {
   std::string fileName;// = "text.txt";

  Client c(0);

  std::cout << "File to retrieve: ";
  std::cin >> fileName;

  c.connect(fileName);
  c.listen();
  c.write_file();

  return 0;
}
