int main()
{
    int x;
    int a;
    x = 0;
    for (a = 2; a < 100; a = (a * a))
        x = x + 1;
    x;
}