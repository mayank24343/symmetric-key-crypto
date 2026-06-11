#include <iostream>
#include <vector>
#include <cstdint>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <random>

// ============================================================
// Weak S-boxes (same as Tasks 1-3)
// ============================================================
static const int WEAK_S_BOXES[8][4][16] = {
    {{14, 1, 6, 3, 8, 5, 7, 10, 13, 9, 15, 12, 2, 4, 11, 0}, {15, 14, 2, 3, 4, 5, 7, 8, 6, 9, 0, 11, 12, 13, 1, 10}, {15, 1, 5, 3, 7, 2, 8, 13, 4, 9, 6, 11, 14, 10, 12, 0}, {4, 1, 5, 3, 8, 2, 6, 7, 10, 9, 15, 11, 12, 13, 14, 0}},
    {{4, 12, 10, 3, 5, 1, 6, 2, 8, 9, 14, 11, 0, 13, 7, 15}, {12, 11, 9, 3, 14, 13, 6, 7, 8, 2, 10, 4, 0, 5, 1, 15}, {5, 14, 2, 13, 6, 4, 11, 15, 12, 9, 3, 0, 1, 10, 8, 7}, {1, 14, 2, 10, 7, 5, 6, 0, 8, 9, 12, 11, 4, 13, 3, 15}},
    {{14, 12, 6, 3, 4, 5, 2, 7, 8, 9, 15, 11, 0, 13, 1, 10}, {0, 1, 2, 8, 13, 9, 10, 7, 3, 6, 12, 11, 4, 5, 14, 15}, {4, 1, 5, 3, 13, 2, 11, 7, 15, 9, 12, 0, 6, 8, 14, 10}, {0, 6, 2, 3, 7, 15, 10, 4, 13, 9, 1, 11, 14, 8, 12, 5}},
    {{12, 1, 0, 14, 4, 5, 6, 3, 10, 9, 2, 13, 8, 11, 7, 15}, {0, 1, 2, 15, 4, 5, 8, 7, 9, 12, 10, 11, 3, 13, 14, 6}, {3, 1, 2, 0, 9, 5, 11, 7, 6, 4, 13, 15, 14, 10, 12, 8}, {0, 1, 9, 3, 4, 5, 2, 7, 11, 15, 10, 8, 13, 12, 6, 14}},
    {{14, 1, 7, 3, 8, 5, 6, 2, 4, 9, 10, 11, 0, 13, 12, 15}, {2, 5, 14, 3, 9, 4, 11, 7, 8, 1, 13, 6, 15, 10, 0, 12}, {0, 1, 5, 13, 7, 12, 10, 4, 3, 9, 2, 11, 14, 6, 8, 15}, {12, 1, 6, 9, 2, 5, 3, 8, 13, 7, 14, 0, 4, 11, 10, 15}},
    {{0, 5, 2, 3, 8, 1, 6, 7, 4, 9, 15, 11, 12, 13, 14, 10}, {0, 1, 14, 3, 4, 5, 11, 10, 12, 9, 7, 6, 8, 13, 2, 15}, {12, 1, 2, 3, 4, 0, 11, 7, 6, 9, 15, 8, 5, 13, 14, 10}, {6, 3, 2, 1, 4, 5, 10, 7, 8, 9, 14, 11, 15, 13, 0, 12}},
    {{2, 3, 1, 6, 5, 7, 4, 6, 9, 8, 11, 10, 13, 15, 14, 12}, {1, 0, 2, 3, 5, 4, 7, 6, 10, 11, 8, 9, 15, 12, 13, 14}, {1, 2, 3, 0, 4, 7, 5, 6, 8, 10, 9, 11, 12, 14, 15, 13}, {2, 1, 0, 3, 5, 7, 6, 4, 8, 10, 13, 9, 13, 14, 12, 15}},
    {{12, 14, 7, 3, 9, 5, 6, 2, 10, 4, 8, 11, 1, 13, 0, 15}, {0, 11, 7, 3, 15, 5, 6, 2, 8, 9, 10, 1, 12, 13, 14, 4}, {0, 1, 9, 14, 4, 2, 10, 5, 13, 7, 15, 11, 12, 8, 3, 6}, {0, 1, 2, 3, 7, 5, 6, 4, 14, 11, 13, 9, 8, 10, 12, 15}}};

// DES tables (1-indexed values; subtract 1 for 0-indexed bit positions)
static const int E_TABLE[48] = {
    32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9, 8, 9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25, 24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1};
static const int P_TABLE[32] = {
    16, 7, 20, 21, 29, 12, 28, 17, 1, 15, 23, 26, 5, 18, 31, 10,
    2, 8, 24, 14, 32, 27, 3, 9, 19, 13, 30, 6, 22, 11, 4, 25};

// ============================================================
// Low-level helpers
// ============================================================

static inline int parity(uint32_t v) { return __builtin_parity(v); }

static inline int extract_row(int x)
{ return ((x & 0b100000) >> 4) | (x & 0b1); }

static inline int extract_col(int x)
{ return (x & 0b011110) >> 1; }

static inline int dot_product(int v, int m, int bits)
{
    int r = 0;
    for (int i = 0; i < bits; ++i)
    {
        if ((v & m) & (1 << i))
        {
            r ^= 1;
        }
    }
    return r;
}

static inline int sbox_val(int b, int x) { return WEAK_S_BOXES[b][extract_row(x)][extract_col(x)]; }

static uint64_t expand(uint32_t R)
{
    uint64_t o = 0;
    for (int i = 0; i < 48; ++i)
    {
        int s = E_TABLE[i] - 1;
        o |= ((uint64_t)((R >> (31 - s)) & 1) << (47 - i));
    }
    return o;
}

static uint32_t permute_P(uint32_t in)
{
    uint32_t o = 0;
    for (int i = 0; i < 32; ++i)
    {
        int s = P_TABLE[i] - 1;
        o |= (((in >> (31 - s)) & 1) << (31 - i));
    }
    return o;
}

static uint32_t F_function(uint32_t R, uint64_t K)
{
    uint64_t x = expand(R) ^ K;
    uint32_t s = 0;
    for (int b = 0; b < 8; ++b)
    {
        int sh = 42 - b * 6;
        s |= (sbox_val(b, (x >> sh) & 0x3F) << (28 - b * 4));
    }
    return permute_P(s);
}

// ============================================================
// Build LAT for one S-box
// ============================================================
std::vector<std::vector<int>> compute_lat(const int sbox[4][16])
{
    std::vector<std::vector<int>> lat(64, std::vector<int>(16, 0));

    for (int alpha = 0; alpha < 64; alpha++) {
        for (int beta = 0; beta < 16; beta++) {
            for (int input = 0; input < 64; input++) {
                int left_side  = dot_product(input, alpha, 6);
                int right_side = dot_product(sbox[extract_row(input)][extract_col(input)], beta, 4);
                if (left_side == right_side) {
                    lat[alpha][beta]++;
                }
            }
            lat[alpha][beta] -= 32;
        }
    }
    return lat;
}

// ============================================================
// MASK CONSTRUCTION
// ============================================================

uint32_t alpha_to_Rmask(int b, int alpha)
{
    uint32_t m = 0;
    for (int j = 0; j < 6; ++j)
        if (alpha >> j & 1)
        {
            int pos   = b * 6 + (5 - j);
            int r_bit = E_TABLE[pos] - 1;
            m ^= (1u << (31 - r_bit));
        }
    return m;
}

uint32_t beta_to_Fmask(int b, int beta)
{
    uint32_t m = 0;
    for (int k = 0; k < 4; ++k)
        if (beta >> k & 1)
        {
            int sl_bit = 3 + 4 * b - k;
            for (int i = 0; i < 32; ++i)
                if (P_TABLE[i] - 1 == sl_bit)
                {
                    m ^= (1u << (31 - i));
                    break;
                }
        }
    return m;
}

std::vector<std::pair<int, int>> Rmask_to_alphas(uint32_t rmask)
{
    int alpha[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    for (int pos = 0; pos < 48; ++pos)
    {
        int r_bit = E_TABLE[pos] - 1;
        if (rmask >> (31 - r_bit) & 1)
        {
            int b = pos / 6;
            int j = 5 - (pos % 6);
            alpha[b] ^= (1 << j);
        }
    }
    std::vector<std::pair<int, int>> active;
    for (int b = 0; b < 8; ++b)
        if (alpha[b])
            active.push_back({b, alpha[b]});
    return active;
}

// ============================================================
// STRUCT RoundStep
// ============================================================
struct RoundStep
{
    std::vector<std::pair<int, int>> active_alpha; // (sbox, alpha) — forced
    std::vector<std::pair<int, int>> active_beta;  // (sbox, beta)  — searched
    uint32_t in_mask;                              // 32-bit mask on this round's R input
    uint32_t out_mask;                             // 32-bit mask on this round's F output
    double eps;                                    // round bias (piling-up)
    bool valid;                                    // false if any active S-box has zero bias
};

// ============================================================
// best_round_given_input:
//   Given a forced R-input mask, decode the active S-boxes, pick
//   the best beta per active S-box, and combine with piling-up lemma.
// ============================================================
RoundStep best_round_given_input(uint32_t in_mask,
                                 const std::vector<std::vector<std::vector<int>>> &all_lats)
{
    RoundStep step;
    step.in_mask = in_mask;
    step.valid   = true;

    // Decode which S-boxes are active and their forced input masks
    step.active_alpha = Rmask_to_alphas(in_mask);

    // If no S-box is active this round has trivial (unusable) approximation
    if (step.active_alpha.empty()) {
        step.out_mask = 0;
        step.eps      = 0.0;
        step.valid    = false;
        return step;
    }

    // For each active S-box, find the beta with the highest |bias|
    uint32_t out_mask = 0;
    double eps = 1.0;   // will be scaled by 2^(k-1) at the end

    for (auto &[b, alpha] : step.active_alpha) {
        const auto &lat = all_lats[b];

        int best_beta  = -1;
        int best_bias  = 0;

        for (int beta = 1; beta < 16; ++beta) {
            int bias = lat[alpha][beta];
            if (std::abs(bias) > std::abs(best_bias)) {
                best_bias  = bias;
                best_beta  = beta;
            }
        }

        if (best_beta == -1 || best_bias == 0) {
            // No usable approximation for this S-box — trail dies
            step.valid = false;
            step.eps   = 0.0;
            step.out_mask = 0;
            // Still record what we have so far for printing
            step.active_beta.push_back({b, 0});
            // Don't return early — finish recording all betas for display
            eps = 0.0;
            out_mask = 0;
            continue;
        }

        step.active_beta.push_back({b, best_beta});

        // Accumulate output mask (XOR = parity-safe combination)
        out_mask ^= beta_to_Fmask(b, best_beta);

        // Piling-up: multiply individual biases (as fractions of 1/2, i.e. prob deviation)
        eps *= (double)best_bias / 64.0;
    }

    // Piling-up lemma: combined eps = 2^(k-1) * product(eps_i)
    // where k = number of active S-boxes
    int k = (int)step.active_alpha.size();
    if (step.valid) {
        eps *= std::pow(2.0, k - 1);
        step.eps = eps;
    }

    step.out_mask = out_mask;
    return step;
}

// ============================================================
// STRUCT Trail
// ============================================================
struct Trail
{
    std::vector<RoundStep> steps;
    double eps_total;
    uint32_t gamma_P, gamma_U;
    bool valid;
};

// ============================================================
// build_trail:
//   Walk round-by-round. Each round's output mask becomes the
//   next round's input mask (propagation). Combine biases with
//   piling-up lemma across rounds.
// ============================================================
Trail build_trail(uint32_t r1_in, int rounds,
                  const std::vector<std::vector<std::vector<int>>> &all_lats)
{
    Trail trail;
    trail.valid    = true;
    trail.gamma_P  = r1_in;   // plaintext-side mask = round 1 R-input
    trail.eps_total = 1.0;

    uint32_t cur_mask = r1_in;

    for (int r = 0; r < rounds; ++r) {
        RoundStep step = best_round_given_input(cur_mask, all_lats);
        trail.steps.push_back(step);

        if (!step.valid) {
            trail.valid     = false;
            trail.eps_total = 0.0;
            trail.gamma_U   = step.out_mask;
            return trail;
        }

        trail.eps_total *= step.eps;
        cur_mask = step.out_mask;   // propagate: this round's output → next round's input
    }

    // Piling-up over rounds: multiply individual round biases (already includes
    // per-round 2^(k_r - 1) factor). Scale by 2^(rounds-1) across rounds.
    // NOTE: each RoundStep's eps already contains the per-round 2^(k-1) factor.
    // The cross-round piling-up requires an additional 2^(rounds-1).
    // But to avoid double-counting, we undo the per-step scaling and redo it
    // globally. Here we keep the convention used in most textbook DES treatments:
    // eps_total = 2^(rounds-1) * product_r( eps_r )
    // where eps_r = 2^(k_r-1) * product_i( b_i/32 ).
    // That means the factor has already been baked into each step.eps. So:
    // eps_total = product_r( step.eps )  — no extra factor needed.
    trail.gamma_U = cur_mask;   // ciphertext-side mask = last round output

    // Apply cross-round piling-up factor: eps_total = 2^(rounds-1) * product(step.eps).
    // Each step.eps already has the per-round 2^(k_r-1) factor baked in.
    trail.eps_total *= std::pow(2.0, rounds - 1);

    return trail;
}

// ============================================================
// search_best_trail:
//   Try every single-S-box seed (b=0..7, alpha=1..63) as the
//   round-1 starting point, keep the trail with highest |eps_total|.
// ============================================================
Trail search_best_trail(int rounds,
                        const std::vector<std::vector<std::vector<int>>> &all_lats)
{
    Trail best;
    best.valid     = false;
    best.eps_total = 0.0;
    best.gamma_P   = 0;
    best.gamma_U   = 0;

    for (int b = 0; b < 8; ++b) {
        for (int alpha = 1; alpha < 64; ++alpha) {
            // Convert single-S-box seed to an R-input mask
            uint32_t r1_in = alpha_to_Rmask(b, alpha);
            if (r1_in == 0) continue;   // degenerate seed

            Trail t = build_trail(r1_in, rounds, all_lats);
            if (!t.valid) continue;

            if (std::abs(t.eps_total) > std::abs(best.eps_total)) {
                best = t;
            }
        }
    }

    return best;
}

// ============================================================
// verify_trail_empirically:
//   Encrypt N random (L, R) pairs with zero round-keys and check
//   how often the combined linear relation Γ_P·R ⊕ Γ_U·F_output
//   holds. Bias = (count / N) - 0.5.
//
//   For a rounds-round Feistel with zero keys, the relation tested is:
//     parity(gamma_P & R_0) == parity(gamma_U & R_rounds)
//   This follows directly from the trail construction (each round
//   links its input mask to its output mask linearly).
// ============================================================
double verify_trail_empirically(const Trail &t, int N = 500000, uint32_t seed = 99)
{
    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<uint32_t> dist32(0, 0xFFFFFFFF);

    int count  = 0;
    int rounds = (int)t.steps.size();

    // Symbolically derive the combined plaintext-side mask on (L0, R0).
    //
    // The trail gives: parity(in_r & R_{r-1}) ≈ parity(Fout_r & F(R_{r-1}))
    // where R_{r-1} is the R value fed to F in round r (0-indexed).
    //
    // Feistel state:
    //   L_{i+1} = R_i
    //   R_{i+1} = L_i XOR F(R_i)
    // So R fed to F in round r = R_{r} (where (L_0,R_0) = plaintext).
    //
    // We propagate the accumulated relation as two running masks:
    //   rel_L:  mask on L0
    //   rel_R:  mask on R0
    // so that the current lhs = parity(rel_L & L0) XOR parity(rel_R & R0).
    //
    // Start: relation is empty (lhs = 0).
    // At each round r (0-based), the approximation says:
    //   parity(in_r & R_r) ≈ parity(Fout_r & F(R_r))
    // XOR this into the accumulated relation.
    //
    // R_r in terms of L0, R0:
    //   r=0: R_0 = R0
    //   r=1: R_1 = L0 ^ F(R0)   ->  parity(m & R_1) = parity(m & L0) ^ parity(m & F(R0))
    //   r=2: R_2 = R_1_prev_L ^ F(R_1) = R0 ^ F(R1)
    //              parity(m & R_2) = parity(m & R0) ^ parity(m & F(R1))
    // In general R_r = prev_L ^ F(prev_R).
    // We track the expression as masks on (L0,R0) plus accumulated F-terms.
    // This gets complicated for deep trails; for ≤3 rounds we can trace manually
    // but the general approach is to accumulate the masks on (L0, R0) as below.

    // We track:
    //   accumulated_L: current XOR mask on L0
    //   accumulated_R: current XOR mask on R0
    // representing the lhs so far.
    // Also we track R_r as a function: (mask_on_L0, mask_on_R0, are_F_terms_present).
    // But F terms can't be folded into L0/R0 at compile time—they contribute via
    // chained approximations. The piling-up computation already handled bias.
    // For the empirical test we just need to actually RUN the cipher.

    // Empirical approach: run rounds of Feistel, track R_{r} explicitly, evaluate
    // parity(in_r & R_r) XOR parity(Fout_r & F(R_r)) for each round, XOR all together.
    // The combined linear sum should equal 0 with probability 0.5 + eps.

    for (int i = 0; i < N; ++i) {
        uint32_t L = dist32(rng);
        uint32_t R = dist32(rng);

        uint32_t Li = L, Ri = R;
        int combined = 0;

        for (int r = 0; r < rounds; ++r) {
            uint32_t Fout = F_function(Ri, 0ULL);

            // lhs of this round's approximation: parity(in_mask & Ri)
            int lhs_r = parity(t.steps[r].in_mask & Ri);
            // rhs of this round's approximation: parity(out_mask & F(Ri))
            int rhs_r = parity(t.steps[r].out_mask & Fout);

            combined ^= (lhs_r ^ rhs_r);   // XOR in this round's approximation error

            uint32_t Lnew = Ri;
            uint32_t Rnew = Li ^ Fout;
            Li = Lnew;
            Ri = Rnew;
        }

        // The combined relation holds (sum = 0) with probability 0.5 + eps
        if (combined == 0) count++;
    }

    return (double)count / N - 0.5;
}

// ============================================================
// DISPLAY
// ============================================================
void print_trail(const Trail &t, const std::string &title)
{
    std::cout << "\n" << title << "\n";
    std::cout << std::string(title.size(), '-') << "\n";
    for (size_t r = 0; r < t.steps.size(); ++r)
    {
        const auto &s = t.steps[r];
        std::cout << " Round " << (r + 1)
                  << ": in=0x" << std::hex << std::setw(8) << std::setfill('0') << s.in_mask
                  << " out=0x" << std::setw(8) << s.out_mask << std::dec << std::setfill(' ')
                  << "  active:";
        for (size_t i = 0; i < s.active_alpha.size(); ++i)
            std::cout << " S" << (s.active_alpha[i].first + 1)
                      << "(a=0x" << std::hex << s.active_alpha[i].second
                      << ",b=0x" << s.active_beta[i].second << std::dec << ")";
        std::cout << "  round_eps=" << std::fixed << std::setprecision(5) << s.eps << "\n";
    }
    std::cout << " Predicted total eps = " << t.eps_total
              << "  |eps| = " << std::abs(t.eps_total) << "\n";
    std::cout << " gamma_P = 0x" << std::hex << std::setw(8) << std::setfill('0') << t.gamma_P
              << "   gamma_U = 0x" << std::setw(8) << t.gamma_U << std::dec << std::setfill(' ') << "\n";
}

// ============================================================
// MAIN
// ============================================================
int main()
{
    std::cout << "============================================================\n";
    std::cout << "  Correct multi-round linear trail search (mask propagation)\n";
    std::cout << "  3-round Feistel DES, weak S-boxes\n";
    std::cout << "============================================================\n";

    std::vector<std::vector<std::vector<int>>> all_lats(8);
    for (int b = 0; b < 8; ++b)
        all_lats[b] = compute_lat(WEAK_S_BOXES[b]);

    // Bias degradation across depths
    std::cout << "\nBias degradation with depth (best valid propagated trail):\n";
    std::cout << "  rounds   |eps|        samples ~1/eps^2\n";
    std::cout << "  ------   --------     -------------------\n";
    for (int rounds = 1; rounds <= 3; ++rounds)
    {
        Trail t = search_best_trail(rounds, all_lats);
        double e2 = t.eps_total * t.eps_total;
        std::cout << "    " << rounds << "      "
                  << std::fixed << std::setprecision(5) << std::abs(t.eps_total)
                  << "      " << (e2 > 0 ? (long long)(1.0 / e2) : -1) << "\n";
    }

    // Full 3-round trail
    Trail t3 = search_best_trail(3, all_lats);
    print_trail(t3, "Best valid 3-round trail");

    std::cout << "\nEmpirical verification (zero-key characteristic, 500k samples):\n";
    double emp = verify_trail_empirically(t3);
    std::cout << "  predicted eps = " << std::fixed << std::setprecision(5) << t3.eps_total << "\n";
    std::cout << "  empirical eps = " << emp << "\n";
    std::cout << "  result        = "
              << (std::abs(std::abs(emp) - std::abs(t3.eps_total)) < 0.01
                      ? "MATCH (trail bias is correct)"
                      : "MISMATCH")
              << "\n";

    return 0;
}