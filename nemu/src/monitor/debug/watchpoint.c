#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;
static int num;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
                wp_pool[i].Enb='N';
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
        num=0;
}

WP* new_wp(char *arg){
    if(free_==NULL) assert(0);

    WP *p=free_;
    bool success;

    free_=p->next;
    p->next=head;
    p->value=expr(arg,&success);
    head=p;
    p->NO=num;
    num++;
    p->Enb='Y';  
    p->type="watchpoint"; 
   // int i;
  //  p=free_;
   // free_=free_->next;
   /* for(i=0;i<NR_WP;i++){
          if(wp_pool[i].Enb=='N'){
                p=free_+i;
                wp_pool[i].Enb='Y';
                wp_pool[i].type="Watchpoint";
                int vvalue=expr(str,&success);
                wp_pool[i].value=vvalue;
                strcpy(wp_pool[i].str,str);
                printf("watchpoint NO. %d\n",wp_pool[i].NO);
                break;
          }
     }
     if(head==NULL) p=head;
     else{
         p->next=head;
         head=p;
     }*/
    return p;
}

void free_wp(int num){
    if(head==NULL) assert(0);
    WP *p=head;
    
    if((p->NO)==num){
        p->str=NULL;
        p->value=0;
        p->type=NULL;
        p->Enb='N';
        head=p->next;
        p->next=free_;
        free_=p;
        p->value=0;
        return;
    }
    int flag=0;
    while((p!=NULL)&&((p->NO)!=num)){
          p=p->next;
          if((p->NO)==num) flag=1;
    }

    if(flag==0) assert(0);
    else{
         WP *a=head;
         WP *b=head->next;
         
         while(b!=NULL){
              if(b->NO==num){
                    b->str=NULL;
                    b->value=0;
                    b->type=NULL;
                    b->Enb='N';
                    a->next=b->next;
                    b->next=free_;
                    free_=b;
           
                    return;
              }
              a=a->next;
              b=b->next;
         }
     }
    
}

int check(){
    WP *h=head;
    bool success;
    if(h==NULL) return 0;
    while(h!=NULL){
          if((h->value)!=expr(h->str,&success)) return 1;
          h=h->next;
    }
    return 0;
}

void print_wp(){
     printf("%-4s%-14s%-4s%-9s%-4s\n","Num","Type","Enb","Value","What");

     int i;
     for(i=0;i<NR_WP;i++){
        printf("%-4d%-14s%-4c0x%-9x%-4s\n",wp_pool[i].NO,wp_pool[i].type,wp_pool[i].Enb,wp_pool[i].value,wp_pool[i].str);
     }     
}
/* TODO: Implement the functionality of watchpoint */


