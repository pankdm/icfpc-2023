#include "shared/libs/json.hpp"

#include <fstream>
#include <iostream>

using json = nlohmann::json;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: ./solver PROBLEM_ID" << std::endl;
    return 1;
  }

  std::string problem = std::string(argv[1]) + ".json";
  std::cout << "Problem: " << problem << std::endl;

  // read a JSON file
  std::ifstream i("problems/" + problem);
  json j;
  i >> j;

  // write prettified JSON to another file
  std::cout << std::setw(4) << j << std::endl;

  std::cout << "Value of `id` is: " << j["id"] << std::endl;

  return 0;
}
