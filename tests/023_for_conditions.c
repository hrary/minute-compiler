int main()
{
    int s = 0;
    int i = 0;
    for (; i < 10; i = i + 1)
    {
        if (i > 5)
            s = s + 1;
    }
    return s;
}