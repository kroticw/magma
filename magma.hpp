//
// Created by  Денис Давыдов on 22.10.2024.
//

#ifndef MAGMA_H
#define MAGMA_H

#include <iostream>

using namespace std;

class Magma {
  uint32_t *key = new uint32_t[32];
  uint32_t *round_keys = new uint32_t[32];
  uint8_t s_block[9][16] = {
      //0,          1,          2,          3,          4,          5,           6,         7,          8,          9,          A,          B,          C,          D,          E,          F
    {0b00000000, 0b00000001, 0b00000010, 0b00000011, 0b00000100, 0b00000101, 0b00000110, 0b00000111, 0b00001000, 0b00001001, 0b00001010, 0b00001011, 0b00001100, 0b00001101, 0b00001110, 0b00001111},
    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    {0b00001100, 0b00000100, 0b00000110, 0b00000010, 0b00001010, 0b00000101, 0b00001011, 0b00001001, 0b00001110, 0b00001000, 0b00001101, 0b00000111, 0b00000000, 0b00000011, 0b00001111, 0b00000001},
    {0b00000110, 0b00001000, 0b00000010, 0b00000011, 0b00001001, 0b00001010, 0b00000101, 0b00001100, 0b00000001, 0b00001110, 0b00000100, 0b00000111, 0b00001011, 0b00001101, 0b00000000, 0b00001111},
    {0b00001011, 0b00000011, 0b00000101, 0b00001000, 0b00000010, 0b00001111, 0b00001010, 0b00001101, 0b00001110, 0b00000001, 0b00000111, 0b00000100, 0b00001100, 0b00001001, 0b00000110, 0b00000000},
    {0b00001100, 0b00001000, 0b00000010, 0b00000001, 0b00001101, 0b00000100, 0b00001111, 0b00000110, 0b00000111, 0b00000000, 0b00001010, 0b00000101, 0b00000011, 0b00001110, 0b00001001, 0b00001011},
    {0b00000111, 0b00001111, 0b00000101, 0b00001010, 0b00001000, 0b00000001, 0b00000110, 0b00001101, 0b00000000, 0b00001001, 0b00000011, 0b00001110, 0b00001011, 0b00000100, 0b00000010, 0b00001100},
    {0b00000101, 0b00001101, 0b00001111, 0b00000110, 0b00001001, 0b00000010, 0b00001100, 0b00001010, 0b00001011, 0b00000111, 0b00001000, 0b00000001, 0b00000100, 0b00000011, 0b00001110, 0b00000000},
    {0b00001000, 0b00001110, 0b00000010, 0b00000101, 0b00000110, 0b00001001, 0b00000001, 0b00001100, 0b00001111, 0b00000100, 0b00001011, 0b00000000, 0b00001101, 0b00001010, 0b00000011, 0b00000111},
    {0b00000001, 0b00000111, 0b00001110, 0b00001101, 0b00000000, 0b00000101, 0b00001000, 0b00000011, 0b00000100, 0b00001111, 0b00001010, 0b00000110, 0b00001001, 0b00001100, 0b00001011, 0b00000010},
  };

  uint32_t s_box_permutation(uint32_t x) {
    int r = 0;
    uint32_t a;
    for (int i = 1; i < 9; i+=2) {
      a = (x >> r) & 0b00000000000000000000000011111111;
      uint8_t a1, a2;
      a1 = a >> 4;
      a2 = a & 0b00001111;
      int j = 0;
      while (a1 != this->s_block[0][j]) {
        j++;
      }
      a1 = this->s_block[i][j];
      j = 0;
      while (a2 != this->s_block[0][j]) {
        j++;
      }
      a2 = this->s_block[i+1][j];

      uint32_t res_a = (a1 << 4) | a2;
      res_a = res_a << r;
      a = x & res_a;
      x = a | x;
      r += 8;
    }
    return x;
  }

  static uint32_t ciclic_shift(uint32_t x) {
    x = (x << 11) | (x >> 21);
    return x;
  }

  public:
    explicit Magma(const uint32_t *key) {
      for (int i = 0; i < 8; i++) {
        this->round_keys[i] = key[i];
        this->round_keys[i+8] = key[i];
        this->round_keys[i+16] = key[i];
        this->round_keys[i+24] = key[7-i];
      }
    };

    int encrypt_block(uint32_t *str) {
      for (int i = 0; i < 32; i++) {
        uint32_t s = ((uint64_t)str[1] | (uint64_t)this->round_keys[i]) & 0xffffffff;
        s = this->s_box_permutation(s);
        s = Magma::ciclic_shift(s);
        str[0] ^= s;
        swap(str[0], str[1]);
        // cout << i << " " << std::bitset<sizeof(str[0]) * CHAR_BIT>(str[0]) << "     ";
        // cout << "2 " << std::bitset<sizeof(str[1]) * CHAR_BIT>(str[1]) << std::endl;
      }
      swap(str[0], str[1]);
      return 0;
    }

    int decrypt_block(uint32_t *str) {
      for (int i = 0; i < 32; i++) {
        uint32_t s = ((uint64_t)str[1] | (uint64_t)this->round_keys[31-i]) & 0xffffffff;
        s = this->s_box_permutation(s);
        s = Magma::ciclic_shift(s);
        str[0] ^= s;
        swap(str[0], str[1]);
        // cout << 0 << " " << std::bitset<sizeof(str[0]) * CHAR_BIT>(str[0]) << "     ";
        // cout << "2 " << std::bitset<sizeof(str[1]) * CHAR_BIT>(str[1]) << std::endl;
      }
      swap(str[0], str[1]);
      return 0;
    }
};

#endif //MAGMA_H
