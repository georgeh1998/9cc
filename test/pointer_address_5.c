int main()
{
    int a;
    int *b;
    int **c;
    int ***d;
    b = &a;
    c = &b;
    d = &c;
    ***d = 3;
    return a;
}