int max2(int a, int b)
{
    if (a > b) return a;
    return b;
}
int min2(int a, int b)
{
    if (a < b) return a;
    return b;
}
int main()
{
    int r = 0;
    if (max2(3, 9) == 9)
        r = r + 1;
    if (min2(max2(2, 5), 4) == 4)
        r = r + 10;
    if (max2(min2(8, 6), min2(3, 9)) > 5)
        r = r + 100;
    return r;
}
