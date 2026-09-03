int c2(int x, int y)
{
    return x * y + x;
}
int b2(int x, int y)
{
    return c2(x + 1, y + 2) + c2(y, x);
}
int a2(int x, int y)
{
    return b2(x, y) - b2(y, x);
}
int main()
{
    return a2(3, 1) + 100;
}
