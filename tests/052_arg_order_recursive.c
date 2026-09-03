int weigh(int a, int b, int c, int d)
{
    return a * 1000 + b * 100 + c * 10 + d;
}
int shift(int n, int d)
{
    if (d == 0) return n;
    return shift(n / 10, d - 1);
}
int main()
{
    int w = weigh(1, 2, 3, 4);
    return shift(w, 2) + shift(w, 3) + 100;
}
