int main()
{
    int i = 0;
    int s = 0;
    for (;;) {
        s = s + i;
        i = i + 1;
        if (i > 10)
            return s + 100;
    }
    return 0;
}
