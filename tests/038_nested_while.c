int main()
{
    int total = 0;
    int i = 1;
    int j = 1;
    while (i <= 5) {
        j = 1;
        while (j <= i) {
            total = total + j;
            j = j + 1;
        }
        i = i + 1;
    }
    return total;
}
