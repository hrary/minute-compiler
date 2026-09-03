int main()
{
    int n = 17;
    int steps = 0;
    if (n > 10) {
        while (n != 1) {
            if (n / 2 * 2 == n) {
                n = n / 2;
            } else {
                if (n > 5) {
                    n = n * 3 + 1;
                } else {
                    n = n - 1;
                }
            }
            steps = steps + 1;
        }
    } else {
        steps = 99;
    }
    return steps;
}
