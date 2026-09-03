int main()
{
    int x = 5;
    int y = 8;
    int r = (x < y) * 100 + (x > y) * 10 + (x == y);
    int s = ((x < y) + (y < 10)) * 7;
    return r + s - 100;
}
