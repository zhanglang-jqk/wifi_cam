#include <stdio.h>
#include <string>

class Parent
{
private:
    char * name;
    int age;

public:
    Parent(char * name);
    char * toString();
};

class Son : public Parent
{
private:
    char * name;

public:
    Son(char * name);
};

Parent::Parent(char * name)
{
    printf("I am parent , my name is %s \r\n", name);
    this->name = name;
}

char * Parent::toString()
{
    printf("I am %s , tostring ", this->name);
}

Son::Son(char * name) : Parent("bb")
{
    printf("I am son \r\n");
    this->name = name;
}

int main()
{
    /* code */
    Son son("e");
    son.toString();
    return 0;
}
