int level3(int x)
{
    return x + 1;
}
int level2(int x)
{
    return level3(x * 2) + 3;
}
int level1(int x)
{
    return level2(x + 4) * 2;
}
int main()
{
    return level1(5);
}
