int main()
{
    int a = 0 - 20;
    int b = 3;
    int p = a / b;
    int q = a / (0 - b);
    int r = 20 / (0 - b);
    int s = 20 / b;
    return p * 1 + q * 2 + r * 3 + s * 4 + 100;
}
