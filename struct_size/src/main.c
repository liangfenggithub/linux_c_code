#include <stdio.h>


// struct person0{
//     char *name;
//     int age;
//     char score;
//     int id;
// };

// struct person1{
//     char *name;
//     int age;
//     char score;
//     int id;
// }__attribute__((packed));

// struct person2{
//     char *name;
//     int age;
//     char score;
//     int id;
// } __attribute__((aligned (4)));

struct person3{
    // char *name;  //指针居然在64位机中占有8个字节
    int age;
    char score;
};

struct person4{
    // char *name;
    int age;
    char score;
}__attribute__((packed));

int main(int argc,char **argv)
{
    // printf("size of (struct person0) = %d.\n",sizeof(struct person0));
    // printf("size of (struct person1) = %d.\n",sizeof(struct person1));
    // printf("size of (struct person2) = %d.\n",sizeof(struct person2));
    printf("size of (struct person3) = %d.\n",sizeof(struct person3));
    printf("size of (struct person4) = %d.\n",sizeof(struct person4));


    return 0;
}