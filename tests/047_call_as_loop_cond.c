int keep_going(int i, int limit)
{
    return i < limit;
}
int step(int x)
{
    return x + 3;
}
int main()
{
    int i = 0;
    int total = 0;
    while (keep_going(i, 7)) {
        total = step(total);
        i = i + 1;
    }
    for (i = 0; keep_going(i, 4); i = i + 1) {
        total = total + i;
    }
    return total;
}
