#include <stdio.h>
int getint() {
    int a;
    scanf("%d", &a);
    return a;
}

const int cnum1 = 1;
const int cnum2 = 2, cnum3 = 3;
const int carray1[2] = {1, 2};
const int carray2[2] = {3, 4}, carray3[2] = {5, 6};
const int carrays1[2][2] = {{1, 2}, {3, 4}};
const int carrays2[2][2] = {{1, 2}, {5, 6}}, carrays3[2][2] = {{1, 2}, {7, 8}};
void vfun1() {
}
void vfun2(int a) {
}
void vfun3(int a, int b) {
}
void vfun4(int a[]) {
}
void vfun5(int a[], int b[]) {
}
void vfun6(int a[][2]) {
}
void vfun7(int a[][2], int b[][2]) {
}
int ifun1() {
    return 0;
}
int ifun2(int a) {
    return a + a;
}
int ifun3(int a, int b) {
    return a + b;
}
int ifun4(int a[]) {
    return a[0];
}
int ifun5(int a[], int b[]) {
    return a[0] + b[0];
}
int ifun6(int a[][2]) {
    return a[0][0];
}
int ifun7(int a[][2], int b[][2]) {
    return a[0][0] + b[0][0];
}
int main() {
    printf("19373700\n");
    int a = 0;
    int b = 0, c = 0;
    int array1[2] = {1, 2};
    int array2[2] = {3, 4}, array3[2] = {5, 6};
    int arrays1[2][2] = {{1, 2}, {3, 4}};
    int arrays2[2][2] = {{1, 2}, {5, 6}}, arrays3[2][2] = {{1, 2}, {7, 8}};
    a = 1;
    array1[a * a] = 3;
    printf("%d\n", array1[1] + array1[0]);
    {
    }
    a + b;
    ;
    if (1) {
        a = a;
    }
    if (1 < 0) {
        a = 1;
    }
    if (0 > 1) {
        a = 1;
    }
    if (1 <= 0) {
        a = 1;
    }
    if (a >= b) {
        a = a + a;
    } else {
        a = a + b;
    }
    printf("%d\n", a);
    if (a == b) {
        printf("yes\n");
    }
    if (a != b) {
        printf("no\n");
    }
    int i = 2;
    while (1) {
        if (i != 0) {
            i = i - 1;
            continue;
        }
        break;
    }
    a = b;
    a = 2;
    a = (a + b);
    a = +-a;
    a = ifun1();
    array1[1] = ifun2(b);
    printf("%d\n", array1[1]);
    arrays1[0][0] = ifun3(a, b);
    printf("%d\n", arrays1[0][0]);
    a = a * arrays1[1][1];
    a = a / arrays2[1][0];
    a = a % arrays3[0][1];
    printf("%d\n", a);
    a = ifun4(arrays3[0]);
    printf("%d\n", a);
    a = ifun7(arrays1, arrays2);
    printf("%d\n", a);
    a = a + array1[0] - array2[0];
    printf("%d\n", a);
    return 0;
}