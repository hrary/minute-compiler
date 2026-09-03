int main()
{
    int t = 0;
    if (((1 < 2) == (3 < 4)) == 1)
        t = t + 10;
    if ((2 > 1) != (5 > 9))
        t = t + 20;
    int u = (t == 30) * 3 + (t < 30) * 5;
    return t + u;
}
