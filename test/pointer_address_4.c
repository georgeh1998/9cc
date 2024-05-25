int main()
{
    int x;
    int *y;
    int **z;
    y = &x;
    z = &y;
    **z = 167;
    return x;
}