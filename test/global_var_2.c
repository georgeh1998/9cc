int a;
int foo()
{
    a = 100;
    return a;
}
int main()
{
    a = 10;
    return foo() + 31;
}