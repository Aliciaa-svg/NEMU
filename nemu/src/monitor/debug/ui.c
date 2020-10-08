#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_scan(char *args);

static int cmd_p(char *args);
 
static int cmd_w(char *args);

static int cmd_d(char *args);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
        { "si", "Stop after performing n operations", cmd_si },
        { "info", "print the value of every register", cmd_info},
        { "x", "scan the memory", cmd_scan},
        { "p", "calculate the expr", cmd_p},
        { "w", "set the watchpoint", cmd_w},
        { "d", "free the watchpoint", cmd_d},
	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

static int cmd_si(char *args){
       char *arg=strtok(NULL," ");

       if(arg==NULL){
             cpu_exec(1);
             return 0;
       }
       
       int n=0;
       sscanf(arg,"%d",&n);
       if(n<-1){
             printf("illegal input\n");
       }
       else if(n==-1){
             cpu_exec(-1);
       }
       else{
             int i=0;
             for(i=0;i<n;i++){
             cpu_exec(1);
             }
       }
       return 0;
}

static int cmd_info(char *args){
       char *arg=strtok(NULL," ");
       if(strcmp(arg,"r")==0){
       int i=0;
       for(i=0;i<8;i++){
           printf("%s 0x%x %d",regsl[i],cpu.gpr[i]._32,cpu.gpr[i]._32);
           printf("\n");
           }
       int j=0;
       for(j=0;j<8;j++){
           printf("%s 0x%x %d\n",regsw[j],cpu.gpr[j]._16,cpu.gpr[j]._16);
       }
       }

       if(strcmp(arg,"w")==0){
           print_wp();
       }
    
       return 0;
}  

static int cmd_scan(char *args){
       char *arg_1=strtok(NULL," ");
       char *arg_2=strtok(NULL," ");

       int len;
       uint32_t address;
      
       sscanf(arg_1,"%d",&len);
       sscanf(arg_2,"%x",&address);

       printf("0x%x: ",address);
       int i=0;
       while(i<len){
             printf("0x%x ",lnaddr_read(address,4));
             address+=4;
             i++;
       }
       printf("\n");
       return 0;       
}

static int cmd_p(char *args){
       char *arg=strtok(NULL," ");
       bool success;
       uint32_t result=expr(arg,&success);
       printf("%d\n",result);
       return 0;
}

static int cmd_w(char *args){
       char *arg=strtok(NULL," ");
       WP *w;
       w=new_wp(arg);
       w->str=malloc(strlen(arg)+1);
       strcpy(w->str,arg);
       printf("watchpoint No.%d\n",w->NO);
       
       return 0;
}

static int cmd_d(char *args){
       char *arg=strtok(NULL," ");
       int num;
       sscanf(arg,"%d",&num);
       free_wp(num);
       printf("free watchpoint No.%d\n",num);
       return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
