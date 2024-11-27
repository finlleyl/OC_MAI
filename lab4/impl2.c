float E(int x) {
    float result = 1.0;
    float factorial = 1.0;

    for (int n = 0; n <= x; ++n) {
        if (n > 0) {
            factorial *= n;
        }

        result += 1.0 / factorial;
    }

    return result;
}