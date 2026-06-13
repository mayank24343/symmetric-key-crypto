#include <bits/stdc++.h>
using namespace std;

int rotateLeft(uint32_t value, int shift) {
    return (value << shift) | (value >> (32 - shift));
}

vector<vector<uint32_t>> QRs_odd(vector<vector<uint32_t>>& X, int i){
    uint32_t a,b,c,d;
    uint32_t b_dash, a_dash, c_dash, d_dash;
    uint64_t modulo = 0x100000000;

    a = X[i][i];
    b = X[(i+1)%4][i];
    c = X[(i+2)%4][i];
    d = X[(i+3)%4][i];

    b_dash = b ^ rotateLeft((a + d)%modulo, 7);
    c_dash = c ^ (rotateLeft((b_dash + a)%modulo, 9));
    d_dash = d ^ (rotateLeft((c_dash + b_dash)%modulo, 13));
    a_dash = a ^ (rotateLeft((d_dash + c_dash)%modulo, 18));
    X[i][i] = a_dash;
    X[(i+1)%4][i] = b_dash;
    X[(i+2)%4][i] = c_dash;
    X[(i+3)%4][i] = d_dash;


    return X;
}

vector<vector<uint32_t>> QRs_even(vector<vector<uint32_t>>& X, int i){
    uint32_t a,b,c,d;
    uint32_t b_dash, a_dash, c_dash, d_dash;
    uint64_t modulo = 0x100000000;

    a = X[i][i];
    b = X[i][(i+1)%4];
    c = X[i][(i+2)%4];
    d = X[i][(i+3)%4];

    b_dash = b ^ rotateLeft((a + d)%modulo, 7);
    c_dash = c ^ (rotateLeft((b_dash + a)%modulo, 9));
    d_dash = d ^ (rotateLeft((c_dash + b_dash)%modulo, 13));
    a_dash = a ^ (rotateLeft((d_dash + c_dash)%modulo, 18));
    X[i][i] = a_dash;
    X[i][(i+1)%4] = b_dash;
    X[i][(i+2)%4] = c_dash;
    X[i][(i+3)%4] = d_dash;


    return X;
}


void printMatrix(vector<vector<uint32_t>>& X) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            cout << hex << X[i][j] << " ";
        }
        cout << endl;
    }
}

int main() {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);


    vector<vector<uint32_t>> X(4, vector<uint32_t>(4)), Y(4, vector<uint32_t>(4)), delta(4, vector<uint32_t>(4));
    int total = 10000;
    int cnt = 0;
    for (int i = 0; i < total; i++){
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            X[i][j] = dist(gen);
            Y[i][j] = X[i][j];
        }
    }

    Y[1][3]  = Y[1][3]^(1<<31);

    for (int i = 0; i < 4; i++) {
        QRs_even(X, i);
    }
    for (int i = 0; i < 4; i++) {
        QRs_odd(X, i);
    }
    for (int i = 0; i < 4; i++) {
        QRs_even(X, i);
    }
    for (int i = 0; i < 4; i++) {
        QRs_odd(X, i);
    }
        
    for (int i = 0; i < 4; i++) {
        QRs_even(Y, i);
    }
    for (int i = 0; i < 4; i++) {
        QRs_odd(Y, i);
    }
    for (int i = 0; i < 4; i++) {
        QRs_even(Y, i);
    }
    for (int i = 0; i < 4; i++) {
        QRs_odd(Y, i);
    }

    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            delta[i][j] = X[i][j] ^ Y[i][j];
        }
    }

    if ((delta[0][0]<<17)>>31 == 1){
        cnt++;
    }
}

    cout << cnt;
    

    return 0;
}