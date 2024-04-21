#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

/*
    知识点：
*/

/*
    程序要求：
*/

//定义链表的结构体
typedef struct _node{
    int value;
    struct _node *next;
}Node;

//定义一个结构体，用于存放链表相关信息
typedef struct list_inf{
    Node *head;
    Node *tail;
}List;

//向链表中添加元素
void list_add(List plist, int number);
//打印链表
void print_list(List plist);
//删除链表中某个节点
void list_del(List plist, int number);

int main(int argc, char const *argv[])
{
    List pList;
    pList.head = pList.tail = NULL;
    int number;
    do{
        scanf("%d",&number);
        if(number != -1){
            list_add(pList,number);
        }
    }while (number != -1);
    print_list(pList);
    scanf("%d",&number);
    
    system("pause");
}


void list_add(List plist, int number)
{
    Node *p = (Node *)malloc(sizeof(Node));
    p->next = NULL;
    p->value = number;
    //find the last
    Node *last = plist.head;
    if(last){//如果链表不空，遍历链表
        while(last->next){
            last = last->next;
        }
        last->next = p;
    }else{//否则的话，也就是链表本来就是空的的时候，新建一个链表
        plist.head = p;
    }
}

void print_list(List plist)
{
    Node *p = NULL;
    for( p = plist.head; p ; p = p->next){
        printf("%d\t",plist.head->value);
    }
    printf("\n");
}

void list_del(List plist, int number)
{
    Node *p = NULL;
    Node *q = NULL;
    for( p = plist.head ; p ; q = p, p = p->next ){
        if( p->value == number){
            if( q ){            //临界情况：开始的时候q是空，p恰好就是要删的
                q->next = p->next;
            }else {
                plist.head = p->next;
            }
        }
    }
}

/*
    
*/