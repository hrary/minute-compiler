int main()
{
    int a = (1 < 2) == 1;
    int b = (3 > 4) == 0;
    int c = (5 == 5) + (6 != 6);
    return a + b + c + (2 <= 2) + (3 >= 4);
}
