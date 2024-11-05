//
// Created by  Денис Давыдов on 22.10.2024.
//
#include <iostream>
#include <fstream>
#include "magma.hpp"

using namespace std;

int main(int argc, const char *argv[]) {
  ifstream envs("../.env");
  if (!envs.is_open()) {
    cout << "Error opening envs file" << endl;
  }
  auto *key_name = new char[4];
  auto *key = new uint32_t[8];
  envs.read(key_name, 4);
  envs.read(reinterpret_cast<char *>(key), sizeof(key)*4);
  envs.close();

  Magma alg(key);

  cout << "ENCRYPT" << endl;
  ifstream inputfile("../input.txt", ios::binary);
  ofstream outputfile("../encrypt.txt", ios::binary);
  if (inputfile.is_open() && outputfile.is_open()) {
    auto *line = new uint32_t[2];
    uint32_t init_vec[2] {0xffff, 0xffff};
    while (
      inputfile.read(reinterpret_cast<char *>(&line[0]), sizeof(line[0])) &&
      inputfile.read(reinterpret_cast<char *>(&line[1]), sizeof(line[0]))
      )
    {
      cout << "INP LINE " << line[0] << " " << line[1] << endl;
      line[0] ^= init_vec[0];
      line[1] ^= init_vec[1];
      alg.encrypt_block(line);
      cout << "ENC LINE " << line[0] << " " << line[1] << endl;
      init_vec[0] = line[0];
      init_vec[1] = line[1];
      outputfile.write(reinterpret_cast<char*>(&line[0]), sizeof(line[0]));
      outputfile.write(reinterpret_cast<char*>(&line[1]), sizeof(line[0]));
    }

    inputfile.close();
    outputfile.close();
    delete[] line;
  }


  cout << "DECRYPT" << endl;
  ifstream enc_file("../encrypt.txt", ios::binary);
  ofstream dec_file("../decrypt.txt", ios::binary);
  if (enc_file.is_open() && dec_file.is_open()) {
    auto *line = new uint32_t[2];
    uint32_t init_vec[2] {0xffff, 0xffff};
    uint32_t cbc_vec[2];
    while (
      enc_file.read(reinterpret_cast<char *>(&line[0]), sizeof(line[0])) &&
      enc_file.read(reinterpret_cast<char *>(&line[1]), sizeof(line[0]))
      )
    {
      cout << "ENC LINE " << line[0] << " " << line[1] << endl;
      cbc_vec[0] = line[0];
      cbc_vec[1] = line[1];
      alg.decrypt_block(line);
      line[0] ^= init_vec[0];
      line[1] ^= init_vec[1];
      init_vec[0] = cbc_vec[0];
      init_vec[1] = cbc_vec[1];
      cout << "DEC LINE " << line[0] << " " << line[1] << endl;
      dec_file.write(reinterpret_cast<char*>(&line[0]), sizeof(line[0]));
      dec_file.write(reinterpret_cast<char*>(&line[1]), sizeof(line[0]));
    }

    delete[] line;
    enc_file.close();
    dec_file.close();
  }
  delete[] key;
  return 0;
}