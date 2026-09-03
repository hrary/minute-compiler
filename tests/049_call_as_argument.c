int g(int x)
{
    return x * 3 + 1;
}
int h(int x)
{
    return x * x + 2;
}
int f(int a, int b)
{
    return a * 10 + b;
}
int main()
{
    return f(g(1), h(2)) + 100;
}
