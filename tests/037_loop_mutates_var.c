int main()
{
    int i = 0;
    int n = 0;
    for (i = 0; i < 20; i = i + 1) {
        n = n + 1;
        if (i == 5)
            i = i + 3;
    }
    return n * 10 + i;
}
