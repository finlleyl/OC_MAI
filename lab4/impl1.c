float E(int x) {
    if (x == 0) return 1;

    float result = 1.0;
    float term = 1.0 + 1.0 / x;

    for (int i = 0; i < x; ++i) {
        result *= term;
    }

    return result;
}