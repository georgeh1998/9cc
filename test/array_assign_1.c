int main()
{
    int x[2];
    *x = 1;
    *(x + 1) = 2;
    int *p;
    p = x;
    return *p + *(p + 1);
}