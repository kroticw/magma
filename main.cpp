//
// Created by  Денис Давыдов on 22.10.2024.
//
#include <iostream>
#include <fstream>
#include "magma.h"

using namespace std;

int main(int argc, const char *argv[]) {
  ifstream envs("../.env");
  if (!envs.is_open()) {
    cout << "Error opening envs file" << endl;
  }
  uint8_t *key = nullptr;
  while (envs.good()) {
    string line;
    getline(envs, line);
    if (line.length() == 36 && line.substr(0, 4) == "KEY=") {
      key = new uint8_t[32];
      for (int i = 0; i < 32; i++) {
        key[i] = static_cast<uint8_t>(line[i + 4]);
      }
    }
  }
  envs.close();

  if (key == nullptr) {
    cout << "Error riding envs key" << endl;
    delete key;
    return 1;
  }

  ifstream inputfile("../input.txt", ios::binary);
  ofstream outputfile("../output.txt", ios::binary);
  if (inputfile.is_open() && outputfile.is_open()) {
    Magma alg(key);
    while (!inputfile.eof()) {
      auto *line = new uint32_t[2];
      inputfile.read(reinterpret_cast<char*>(line), sizeof(uint32_t) * 2);
      alg.encrypt_block(line);
      outputfile.write(reinterpret_cast<char*>(line), sizeof(uint32_t) * 2);
    }

    inputfile.close();
    outputfile.close();
  }
  delete[] key;
  return 0;
}