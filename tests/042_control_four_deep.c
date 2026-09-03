int main()
{
    int a = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    for (i = 1; i <= 4; i = i + 1) {
        if (i > 1) {
            j = 0;
            while (j < i) {
                if (j == 2) {
                    for (k = 0; k < 3; k = k + 1) {
                        if (k == 1)
                            a = a + 5;
                        else
                            a = a + 1;
                    }
                } else {
                    a = a + 2;
                }
                j = j + 1;
            }
        } else {
            a = a + 100;
        }
    }
    return a;
}
