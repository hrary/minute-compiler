int ping(int n, int acc)
{
    if (n <= 0) return acc;
    return pong(n - 1, acc + n);
}
int pong(int n, int acc)
{
    if (n <= 0) return acc;
    return ping(n - 1, acc + n * 2);
}
int main()
{
    return ping(6, 0);
}
