#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ, UEQ, AND, OR, NOT, REGISTER, NUMBER, HEX, VARIABLE, EIP

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
        int priority;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE, -1},				// spaces
	{"\\+", '+', 4},					// plus
	{"==", EQ, 7},						// equal
        {"\\*",'*', 3},
        {"-", '-',4},
        {"/",'/', 3},
        {"\\(",'(', 1},
        {"\\)",')', 1},
        {"!=", UEQ, 7},
        {"&&", AND, 11},
        {"\\|\\|", OR, 12},
        {"!", NOT, 100},
        {"0[xX][a-fA-F0-9]{1,8}", HEX,-1},
        {"[0-9]+",NUMBER,-1},
        {"[a-zA-Z][0-9a-zA-Z]*[0-9a-zA-Z]*", VARIABLE, -1},
        {"\\$[a-dA-D][HhLl]|\\$[eE]?(ax|AX|bx|BX|cx|CX|dx|DX|si|SI|di|DI|bp|BP|sp|SP)", REGISTER, -1},
        {"\\$[eE][iI][pP]", EIP, -1}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token=0;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type){
                                        case 43:
                                             tokens[nr_token].type=43;
                                             nr_token++;
                                             break;
                                        case 257: 
                                             tokens[nr_token].type=257;
                                             strcpy(tokens[nr_token].str,"==");
                                             nr_token++;
                                             break;
                                        case 42:
                                             tokens[nr_token].type=42;
                                             nr_token++;
                                             break;
                                        case 45:
                                             tokens[nr_token].type=45;
                                             nr_token++;
                                             break;
                                        case 47:
                                             tokens[nr_token].type=47;
                                             nr_token++;
                                             break;
                                        case 40:
                                             tokens[nr_token++].type=40;
                                             break;
                                        case 41:
                                             tokens[nr_token++].type=41;
                                             break;
                                        case 258:
                                             tokens[nr_token].type=258;
                                             strcpy(tokens[nr_token].str,"!=");
                                             nr_token++;
                                             break;
                                        case 259:
                                             tokens[nr_token].type=259;
                                             strcpy(tokens[nr_token].str,"&&");
                                             nr_token++;
                                             break;
                                        case 260:
                                             tokens[nr_token].type=260;
                                             strcpy(tokens[nr_token].str,"||");
                                             nr_token++;
                                             break;
                                        case 261:
                                             tokens[nr_token].type=261;
                                             strcpy(tokens[nr_token].str,"!");
                                             nr_token++;
                                             break;
                                        case 264:
                                             tokens[nr_token].type=264;
                                             strncpy(tokens[nr_token].str,&e[position-substr_len],substr_len);
                                             nr_token++;
                                             break;
                                        case 263:
                                             tokens[nr_token].type=263;
                                             strncpy(tokens[nr_token].str,&e[position-substr_len],substr_len);
                                             nr_token++;
                                             break;
                                        case 265:
                                             tokens[nr_token].type=265;
                                             strncpy(tokens[nr_token].str,&e[position-substr_len],substr_len);
                                             nr_token++;
                                             break;
                                        case 262:
                                             tokens[nr_token].type=262;
                                             strncpy(tokens[nr_token].str,&e[position-substr_len],substr_len);
                                             nr_token++;
                                             break;
                                        case 266:
                                             tokens[nr_token].type=266;
                                             nr_token++;
                                             break;         
                                        default:
                                             nr_token--;
                                             break;                            
                                        
				}
                                //nr_token++;
				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}
	return true; 
}

bool check_parentheses(int p,int q){
     int count=0;
     int sign=1;

     if(tokens[p].type==40){
        count++;
        int i=p+1;
        for(i=p+1;i<=q;i++){
            if(tokens[i].type==40) count++;
            if(tokens[i].type==41){
               count--;
               if((count==0)&&(i!=q)) sign=0;
               if(count<0) assert(0);
            }
        }
        if((count==0)&&(sign==1)&&(tokens[q].type==41)) return 1;
        else return 0;  
      }
      return 0;     
}

int dominant_operator(int p,int q){
    int i;
    int op=p;
    int left=0;
    int val=-1;
    if(check_parentheses(p,q)&&tokens[p].type==40&&tokens[q].type==41){
            return dominant_operator(p+1,q-1);
    };
    if(!check_parentheses(p,q)){
            printf("illegal input\n");
            assert(0);
    };
    for(i=p;i<=q;i++){
       if(tokens[i].type==262||tokens[i].type==263||tokens[i].type==264||tokens[i].type==265||tokens[i].type==266) continue;
       if(tokens[i].type==40){
          left++;
          i++;
          while(left!=0){
               if(tokens[i].type==41) left--;
               if(tokens[i].type==40) left++;
               i++;
          }
          if(i>q) break;
       }
       if(rules[tokens[i].type].priority>val){
            val=rules[tokens[i].type].priority;
            op=i;
       }
    }
    return op;    
}

int eval(int p,int q){
    if(p>q){
       printf("illegal\n");
       assert(0);
    }
    else if(p==q){
       int t;
       if(tokens[p].type==264){
           sscanf(tokens[p].str,"%x",&t);
           return t;
       }
       if(tokens[p].type==263){
           sscanf(tokens[p].str,"%d",&t);
           return t;
       }
       if(tokens[p].type==266){
           return cpu.eip;
       }
       if(tokens[p].type==262){
           int j=0;
           int r32=1;
           int r16=1;
           while(j<8){
                r32=strcmp(tokens[p].str+1,regsl[j]);//+1:because the first token is $
                r16=strcmp(tokens[p].str+1,regsw[j]);
                if((r32==0)||(r16==0)) break;
                j++;
           }
           if(r32==0){//the register is 32-bit
                return cpu.gpr[j]._32;
           }
           else if(r16==0){
                return cpu.gpr[j]._16;//the value of the register
           }
           else{//8-bit,match individually because of their names
               
                if(!strcmp(tokens[p].str+1,"al")) return cpu.gpr[0]._8[0];
                if(!strcmp(tokens[p].str+1,"cl")) return cpu.gpr[1]._8[0];
                if(!strcmp(tokens[p].str+1,"dl")) return cpu.gpr[2]._8[0];
                if(!strcmp(tokens[p].str+1,"bl")) return cpu.gpr[3]._8[0];
                if(!strcmp(tokens[p].str+1,"ah")) return cpu.gpr[0]._8[1];
                if(!strcmp(tokens[p].str+1,"ch")) return cpu.gpr[1]._8[1];
                if(!strcmp(tokens[p].str+1,"dh")) return cpu.gpr[2]._8[1];
                if(!strcmp(tokens[p].str+1,"bh")) return cpu.gpr[3]._8[1];
           }
           if(j==8){
                 printf("error\n");
                 assert(0);        
           }        
       }
     }
     else if(check_parentheses(p,q)==true){
          
          return eval(p+1,q-1);
     }
     else{
         if((tokens[p].type==261)&&((q-p==1)||(tokens[p+1].type==40&&tokens[q].type==41))){
            int t=eval(p+1,q);
            return !t;
         }
 
         if((q-p==1)&&(tokens[p].type==45)){
            return 0-eval(q,q);
         } 
         if((tokens[p].type==42)&&((q-p==1)||((tokens[p+1].type==40)&&(tokens[q].type==41)))){
            int add=eval(p+1,q);
            return swaddr_read(add,4);
         }

         int op=dominant_operator(p,q);
      //   if(tokens[p].type==40&&tokens[q].type==41)         

         int val1=eval(p,op-1);
         int val2=eval(op+1,q);

         switch(tokens[op].type)
         {
              case 42: return val1*val2;
              case 43: return val1+val2;
              case 45: return val1-val2;
              case 47: return val1/val2;
              case 257: return (val1==val2);
              case 258: return (val1!=val2);
              case 259: return (val1&&val2);
              case 260: return (val1||val2);
              default:
                   printf("no matched operator\n");
                   assert(0);
                 
         }
     }
     return 0;
       
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}
        *success=true;
        int result=eval(0,nr_token-1);
	/* TODO: Insert codes to evaluate the expression. */
    
	return result;
}

