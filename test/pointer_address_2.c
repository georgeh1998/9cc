int main()
{
    int x;
    int *y;
    y = &x;
    *y = 10;
    return x;
}