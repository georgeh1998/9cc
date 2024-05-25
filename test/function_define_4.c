int fibonacci(int n)
{
    int a;
    int b;
    int tmp;
    int i;
    a = 0;
    b = 1;
    if (n == 0)
    {
        return a;
    }
    if (n == 1)
    {
        return b;
    }
    for (i = 0; i < (n - 1); i = i + 1)
    {
        tmp = a;
        a = b;
        b = tmp + b;
    }
    return b;
}
int main() { return fibonacci(10); }