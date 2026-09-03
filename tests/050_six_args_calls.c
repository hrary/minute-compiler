int sq(int x)
{
    return x * x;
}
int dbl(int x)
{
    return x + x;
}
int diff(int x, int y)
{
    return x - y;
}
int six(int a, int b, int c, int d, int e, int f)
{
    return a * 1 + b * 2 + c * 3 + d * 4 + e * 5 + f * 6;
}
int main()
{
    return six(sq(2), dbl(3), diff(9, 4), sq(1), dbl(2), diff(7, 5));
}
