int main()
{
    int i = 3;
    int p = 1;
    for (; i > 0;) {
        p = p * 2;
        i = i - 1;
    }
    for (i = 0; ; i = i + 1) {
        if (i == 4)
            return p + i;
    }
    return 0;
}
