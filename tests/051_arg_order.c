int f3(int a, int b, int c)
{
    return a * 100 + b * 10 + c;
}
int f6(int a, int b, int c, int d, int e, int f)
{
    return ((((a * 2 + b) * 2 + c) * 2 + d) * 2 + e) * 2 + f;
}
int main()
{
    return f3(1, 2, 3) - f6(1, 0, 1, 0, 1, 1) - 30;
}
