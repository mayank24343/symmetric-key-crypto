#include <bits/stdc++.h>
using namespace std;

// All 8 DES S-boxes  [sbox][row 0-3][col 0-15]
static const int SBOX[8][4][16] = {
    // S1
    {
        {14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7},
        { 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8},
        { 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0},
        {15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13}
    },
    // S2
    {
        {15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10},
        { 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5},
        { 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15},
        {13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9}
    },
    // S3
    {
        {10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8},
        {13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1},
        {13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7},
        { 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12}
    },
    // S4
    {
        { 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15},
        {13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9},
        {10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4},
        { 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14}
    },
    // S5
    {
        { 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9},
        {14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6},
        { 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14},
        {11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3}
    },
    // S6
    {
        {12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11},
        {10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8},
        { 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6},
        { 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13}
    },
    // S7
    {
        { 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1},
        {13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6},
        { 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2},
        { 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12}
    },
    // S8
    {
        {13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7},
        { 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2},
        { 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8},
        { 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11}
    }
};

int extract_row(int input)
{
    return ((input & 0b100000) >> 4) | (input & 0b1);
    // write your logic here
}

// Function 02: extract_col

int extract_col(int input)
{
    return (input & 0b011110) >> 1;
    // write your logic here
}

vector<vector<int>> createDAT(int sbox_no){
    vector<vector<int>> dat(64, vector<int>(16, 0));
    
    for (int input_diff = 0; input_diff < 64; input_diff++) {
        for (int input = 0; input < 64; input++) {
            int output_diff = SBOX[sbox_no][extract_row(input)][extract_col(input)] ^ SBOX[sbox_no][extract_row(input ^ input_diff)][extract_col(input ^ input_diff)];
            dat[input_diff][output_diff]++;
        }
        
    }
    return dat;
}

void print_dat_table(const std::vector<std::vector<int>> &dat, int sbox_index)
{

    std::cout << "\n";
    std::cout << "============================================================\n";
    std::cout << " Differentiation Approximation Table  --  S-Box " << sbox_index + 1 << "\n";
    std::cout << "============================================================\n";

    // --- Header row: output masks beta in hex ---
    std::cout << " Alpha \\ Beta |";
    for (int beta = 0; beta < 16; ++beta)
    {
        std::cout << std::setw(4) << std::hex << beta << std::dec;
    }
    std::cout << "\n";
    std::cout << " " << std::string(14 + 16 * 4, '-') << "\n";

    // --- Data rows: one per input mask alpha ---
    for (int alpha = 0; alpha < 64; ++alpha)
    {
        std::cout << std::setw(12) << std::dec << alpha << "  |";
        for (int beta = 0; beta < 16; ++beta)
        {
            std::cout << std::setw(4) << dat[alpha][beta];
        }
        std::cout << "\n";
    }

    // --- Summary: strongest non-trivial linear approximation ---
    /*
    int best_alpha, best_beta;
    int max_abs_bias = find_max_bias(lat, best_alpha, best_beta);
    int raw_count = max_abs_bias + 32;                          // back to count (0..64)
    double epsilon = (double)lat[best_alpha][best_beta] / 64.0; // Matsui's eps

    std::cout << "\n Best non-trivial approximation:\n";
    std::cout << "   Input  mask alpha = " << best_alpha
              << "  (0x" << std::hex << best_alpha << std::dec << ")\n";
    std::cout << "   Output mask beta  = " << best_beta
              << "  (0x" << std::hex << best_beta << std::dec << ")\n";
    std::cout << "   Bias              = " << lat[best_alpha][best_beta]
              << "  (raw count = " << raw_count << " / 64)\n";
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "   Epsilon (bias/64) = " << epsilon << "\n";
    std::cout << "   |Epsilon|         = " << std::abs(epsilon) << "\n";
    */
}

int main(){
    for (int sbox_no = 0; sbox_no < 8; sbox_no++){
        print_dat_table(createDAT(sbox_no), sbox_no);
    }
}


