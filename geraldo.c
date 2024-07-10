#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX 1000000

typedef enum Estados{
  Inicio,
  Em_Identificador,
  Em_Numero,
  Em_Numero_Decimal,
  Em_Operador,
  Em_Operador_Composto,
  Em_Separador,
  Em_Palavra_Reservada,
  Em_Comentario
}Estado;

typedef enum Tipos{
  Identificador,
  Numero,
  Operador,
  Operador_Composto,
  Separador,
  Palavra_Reservada,
  Comentario,
  Nao_identificado
}Tipos;

typedef struct token{
  Tipos tipo;
  char *token;
}Token;

typedef struct quantidade{
  int keyword;
  int id;
  int numb;
  int op;
  int cop;
  int delim;
  int coments;
  int uk;
}Qtd;

Tipos verificar(char c) {
  if (isdigit(c)) {
    return Numero;
  } else if (isalpha(c)) {
    return Identificador;
  } else if (c == '+' || c == '-' || c == '*' || c == '/' || c== '<' || c=='>' || c=='=') {
    return Operador;
  } else if (c == ';' || c == '.' || c == '(' || c == ')'|| c == '[' || c==']' || c==',' || c==':') {
    return Separador;
  } else {
    return Nao_identificado;
  }
}

const char *keywords[] = {
    "and", "array", "begin", "div", "do", "else", "end",
    "function", "goto", "if", "label", "not", "of", "or",
    "procedure", "program", "then", "type", "var", "while",
    "read", "write"
};
//fazer busca binaria
int isPalavrasreservadas(char *palavra, int n){
  for(int i = 0; i < 22; i++){
    if (strncmp(palavra, keywords[i], n) == 0 && strlen(keywords[i]) == n) {
      return 1;
    }
  }
  return 0;
}

void alex(FILE *arquivo,Qtd *qtd,Token *tokens){
  char *bufferTokens = calloc(MAX,sizeof(char));
  int posicsTokens = 0;

  int saveTokens = 0;

  qtd->id = 0;
  qtd->keyword = 0;
  qtd->numb = 0;
  qtd->op = 0;
  qtd->cop=0;
  qtd->delim=0;
  qtd->coments=0;
  qtd->uk=0;

  char *fita = calloc(MAX,sizeof(char));
  int coment = 0;
  Qtd *qtd2 = malloc(sizeof(Qtd));
  Token *tokens2 = malloc(sizeof(Token)*MAX);
  int saveTokens2 = 0;

  Estado estado = Inicio;
  while (fgets(fita, MAX, arquivo) != NULL) {
    int tam = strlen(fita);
    fita[tam]=' ';
    fita[tam+1]='\0';
    int currentA = 0;
    while(fita[currentA] != '\0'){
      Tipos leitor = verificar(fita[currentA]);
      
      if(estado == Inicio){
        if(leitor == Identificador){
          estado = Em_Identificador;
          bufferTokens[posicsTokens++] = fita[currentA];
          currentA++;
        }else if(leitor == Numero){
          estado = Em_Numero;
          bufferTokens[posicsTokens++] = fita[currentA];
          currentA++;
        }else if(leitor == Operador){
          estado = Em_Operador;
          bufferTokens[posicsTokens++] = fita[currentA];
          currentA++;
        }else if(leitor == Separador){
          estado = Em_Separador;
          bufferTokens[posicsTokens++] = fita[currentA];
          currentA++;
        }else{
          if(fita[currentA]!=' ' && fita[currentA]!='\n' && fita[currentA]!='\r' && fita[currentA]!='\t'){
            qtd->uk++; 
            bufferTokens[posicsTokens] = '\0';
            tokens[saveTokens].tipo = Nao_identificado;
            tokens[saveTokens].token = malloc(sizeof(char)*strlen(bufferTokens));
            strcpy(tokens[saveTokens].token,bufferTokens);
            saveTokens++;
            //printf("%s\n",bufferTokens);
          }
          posicsTokens = 0;
          estado = Inicio;
          currentA++;
        }
        
      }else if(estado == Em_Identificador){

        if(leitor == Identificador){
          bufferTokens[posicsTokens++] = fita[currentA];
          if(isPalavrasreservadas(bufferTokens,posicsTokens)){
            estado = Em_Palavra_Reservada;
          }else{
            estado = Em_Identificador;
          }
          currentA++;
        }else if(leitor == Numero){
          estado = Em_Identificador;
          bufferTokens[posicsTokens++] = fita[currentA];
          currentA++;
        }else{
          bufferTokens[posicsTokens] = '\0';
          qtd->id++;
          tokens[saveTokens].tipo = Identificador;
          tokens[saveTokens].token = malloc(sizeof(char)*strlen(bufferTokens));
          strcpy(tokens[saveTokens].token,bufferTokens);
          saveTokens++;
         //printf("IDENTIFICADOR:");
         //printf("%s\n",bufferTokens);
          posicsTokens = 0;
          estado = Inicio;
        }
      }else if(estado == Em_Numero){   
        if(leitor == Numero){
          estado = Em_Numero;
          bufferTokens[posicsTokens++] = fita[currentA];
          currentA++;
        }else if(fita[currentA]=='.' && verificar(fita[currentA+1])==Numero ){         
          estado = Em_Numero_Decimal;
          bufferTokens[posicsTokens++] = fita[currentA];
          currentA++;
        }else{
          bufferTokens[posicsTokens] = '\0';
          qtd->numb++;
          tokens[saveTokens].tipo = Numero;
          tokens[saveTokens].token = malloc(sizeof(char)*strlen(bufferTokens));
          strcpy(tokens[saveTokens].token,bufferTokens);
          saveTokens++;
          //printf("NUMERO:");
          //printf("%s\n",bufferTokens);
          posicsTokens = 0;
          estado = Inicio;
        }
      }else if(estado == Em_Operador){
        //melhorar condicao
        if((bufferTokens[posicsTokens-1]=='>') || (bufferTokens[posicsTokens-1]=='<')){
          if(bufferTokens[posicsTokens-1]=='<'){
            if(fita[currentA]=='>'||(fita[currentA]=='=')){
              estado = Em_Operador_Composto;
              bufferTokens[posicsTokens++] = fita[currentA];
              currentA++;
            }else{
              bufferTokens[posicsTokens] = '\0';
              qtd->op++;
              tokens[saveTokens].tipo = Operador;
              tokens[saveTokens].token = malloc(sizeof(char)*strlen(bufferTokens));
              strcpy(tokens[saveTokens].token,bufferTokens);
              saveTokens++;
              //printf("OPERADOR:");
              //printf("%s\n",bufferTokens);
              posicsTokens = 0;
              estado = Inicio;
            }
          }else if(bufferTokens[posicsTokens-1]=='>'){
            if(fita[currentA]=='='){
              estado = Em_Operador_Composto;
              bufferTokens[posicsTokens++] = fita[currentA];
              currentA++;
            }else{
              bufferTokens[posicsTokens] = '\0';
              qtd->op++;
              tokens[saveTokens].tipo = Operador;
              tokens[saveTokens].token = malloc(sizeof(char)*strlen(bufferTokens));
              strcpy(tokens[saveTokens].token,bufferTokens);
              saveTokens++;
              //printf("OPERADOR:");
              //printf("%s\n",bufferTokens);
              posicsTokens = 0;
              estado = Inicio;
            }
           
          }
        }else if(bufferTokens[posicsTokens-1]=='*' && fita[currentA]==')'){
          estado = Em_Comentario;
          bufferTokens[posicsTokens++] = fita[currentA];
          currentA++;
        }else{
          bufferTokens[posicsTokens] = '\0';
          qtd->op++;
          tokens[saveTokens].tipo = Operador;
          tokens[saveTokens].token = malloc(sizeof(char)*strlen(bufferTokens));
          strcpy(tokens[saveTokens].token,bufferTokens);
          saveTokens++;

          //printf("OPERADOR:");
          //printf("%s\n",bufferTokens);
          posicsTokens = 0;
          estado = Inicio;
        }
      }else if(estado == Em_Separador){
        if(bufferTokens[posicsTokens-1]==':' && fita[currentA]=='='){
          estado = Em_Operador_Composto;
          bufferTokens[posicsTokens++] = fita[currentA];
          currentA++;
        }else if(bufferTokens[posicsTokens-1]=='(' && fita[currentA]=='*'){
          estado = Em_Comentario;
          bufferTokens[posicsTokens++] = fita[currentA];
          currentA++;
        }else{
          bufferTokens[posicsTokens] = '\0';
          qtd->delim++;
          tokens[saveTokens].tipo = Separador;
          tokens[saveTokens].token = malloc(sizeof(char)*strlen(bufferTokens));
          strcpy(tokens[saveTokens].token,bufferTokens);
          saveTokens++;

          //printf("SEPARADOR:");
          //printf("%s\n",bufferTokens);
          posicsTokens = 0;
          estado = Inicio;
        }
      }else if(estado == Em_Palavra_Reservada){
        if(leitor == Identificador || leitor == Numero){
          estado = Em_Identificador;
          bufferTokens[posicsTokens++] = fita[currentA];
          currentA++;
        }else{
          bufferTokens[posicsTokens] = '\0';
          qtd->keyword++;
          tokens[saveTokens].tipo = Palavra_Reservada;
          tokens[saveTokens].token = malloc(sizeof(char)*strlen(bufferTokens));
          strcpy(tokens[saveTokens].token,bufferTokens);
          saveTokens++;
          //printf("PALAVRA_RESERVADA:");
          //printf("%s\n",bufferTokens);
          posicsTokens = 0;
          estado = Inicio;
        }
      }else if(estado == Em_Numero_Decimal){
        if(leitor == Numero){
          estado = Em_Numero_Decimal;
          bufferTokens[posicsTokens++] = fita[currentA];
          currentA++;   
        }else{
          bufferTokens[posicsTokens] = '\0';
          qtd->numb++;
          tokens[saveTokens].tipo = Numero;
          tokens[saveTokens].token = malloc(sizeof(char)*strlen(bufferTokens));
          strcpy(tokens[saveTokens].token,bufferTokens);
          saveTokens++;

          //printf("NUMERO:");
          //printf("%s\n",bufferTokens);
          posicsTokens = 0;
          estado = Inicio;
        }
      }else if(estado == Em_Operador_Composto){
        bufferTokens[posicsTokens] = '\0';
        qtd->cop++;
        tokens[saveTokens].tipo = Operador_Composto;
        tokens[saveTokens].token = malloc(sizeof(char)*strlen(bufferTokens));
        strcpy(tokens[saveTokens].token,bufferTokens);
        saveTokens++;

        //printf("OPERADOR_COMPOSTO:");
        //printf("%s\n",bufferTokens);
        posicsTokens = 0;
        estado = Inicio;
      }else if(estado == Em_Comentario){
        //arrumar comnetarios
        if(bufferTokens[posicsTokens-1]=='*'){
          coment = 1;
          saveTokens2 = saveTokens;
          memcpy(qtd2, qtd, sizeof(Qtd));
          memcpy(tokens2, tokens, sizeof(Token)*MAX);
          qtd->delim++;
          qtd->op++;
          //printf("DELIMITER:(");
          //printf("OPERADOR:*");
          tokens[saveTokens].tipo = Separador;
          tokens[saveTokens].token = malloc(sizeof(char)*2);
          strcpy(tokens[saveTokens].token,"(");
          saveTokens++;

          tokens[saveTokens].tipo = Operador;
          tokens[saveTokens].token = malloc(sizeof(char)*2);
          strcpy(tokens[saveTokens].token,"*");
          saveTokens++;

          bufferTokens[posicsTokens-1] = '\0';
          estado = Inicio;
          posicsTokens = 0;
        }else if(bufferTokens[posicsTokens-1]==')'){
          if(coment==1){
            saveTokens = saveTokens2;
            memcpy(qtd, qtd2, sizeof(Qtd));
            memcpy(tokens, tokens2, sizeof(Token)*MAX);
            qtd->coments++;
            //printf("COMENTARIO:");
          }else{
            qtd->delim++;
            tokens[saveTokens].tipo = Separador;
            tokens[saveTokens].token = malloc(sizeof(char)*2);
            strcpy(tokens[saveTokens].token,"*");
            saveTokens++;

            tokens[saveTokens].tipo = Operador;
            tokens[saveTokens].token = malloc(sizeof(char)*2);
            strcpy(tokens[saveTokens].token,")");
            saveTokens++;
            //printf("OPERADOR:*"); 
            //printf("DELIMITER:)");
            qtd->op++;
          }
          bufferTokens[posicsTokens-1] = '\0';
          estado = Inicio;
          posicsTokens = 0;
        }
      }
    }
  }

}


void program(Token *tokens, int *p, char **variaveis,int *vari);
void bloco(Token *tokens, int *p, char **variaveis,int *vari);
void declaracaoVar(Token *tokens, int *p, char **variaveis,int *vari);
void declaracao(Token *tokens, int *p, char **variaveis,int *vari);
int comandoComposto(Token *tokens, int *p, char **variaveis,int *vari);
int comando(Token *tokens, int *p, char **variaveis,int *vari);
int write(Token *tokens, int *p, char **variaveis,int *vari);
int read(Token *tokens, int *p, char **variaveis,int *vari);
int comandoRotulo(Token *tokens, int *p, char **variaveis,int *vari);
int atribuicao(Token *tokens, int *p, char **variaveis,int *vari);
int expressao(Token *tokens, int *p, char **variaveis,int *vari);
int expressaoSimples(Token *tokens, int *p, char **variaveis,int *vari);
int fator(Token *tokens, int *p, char **variaveis,int *vari);
int termo(Token *tokens, int *p, char **variaveis,int *vari);
void variavel(Token *tokens, int *p, char **variaveis,int *vari);
int comandoCondicional(Token *tokens, int *p, char **variaveis,int *vari);
int comandoRepetitivo(Token *tokens, int *p, char **variaveis,int *vari);
void declaracaoRotulos(Token *tokens, int *p, char **variaveis,int *vari);
int desvio(Token *tokens, int *p, char **variaveis,int *vari);
void declaracaoSub(Token *tokens, int *p, char **variaveis,int *vari);
void declaracaoFunction(Token *tokens, int *p, char **variaveis,int *vari);
void declaracaoProcedure(Token *tokens, int *p, char **variaveis,int *vari);
int chamadaProcedure(Token *tokens, int *p, char **variaveis,int *vari);
void geraldo(Token *tokens, int *p, char **variaveis,int *vari,int *label);


void program(Token *tokens, int *p, char **variaveis, int*vari) {
    if (tokens[*p].tipo == Palavra_Reservada && strcmp(tokens[*p].token, "program") == 0) {
        (*p)++;
    } else {
        printf("Rejeito\n");
        exit(0);
    }

    if (tokens[*p].tipo == Identificador) {
        (*p)++;
    } else {
        printf("Rejeito\n");
        exit(0);
    }

    if(tokens[*p].tipo == Separador && strcmp(tokens[*p].token, "(") == 0){
        (*p)++;
        while (tokens[*p].tipo == Identificador) {
            (*p)++;
            if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ",") == 0) {
                (*p)++;
                if(tokens[*p].tipo != Identificador){
                  printf("Rejeito\n");
                  exit(0);
                }
            } else {
                break;
            }
        }

        if(tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ")") == 0){
            (*p)++;
        } else {
            printf("Rejeito\n");
            exit(0);
        }
    }

    if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ";") == 0) {
        (*p)++;
    } else {
        printf("Rejeito\n");
        exit(0);
    }
    bloco(tokens, p,variaveis,vari);
    return;
}

void bloco(Token *tokens, int *p,char **variaveis, int*vari) {
  declaracaoRotulos(tokens,p,variaveis,vari);
  declaracaoVar(tokens,p, variaveis,vari);
  declaracaoSub(tokens,p,variaveis,vari);
  comandoComposto(tokens,p,variaveis,vari);

  return;
}

void declaracaoRotulos(Token *tokens, int *p,char **variaveis, int*vari){
    if (tokens[*p].tipo == Palavra_Reservada && strcmp(tokens[*p].token, "label") == 0) {
      (*p)++;
    } else {
      return;
    }
    

    while (tokens[*p].tipo == Numero) {
        (*p)++;
      if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ",") == 0) {
        (*p)++;
        if(tokens[*p].tipo != Numero){
          printf("Rejeito\n");
          exit(0);
        }
      }else {
        break;
      }
    }

    if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ";") == 0) {
        (*p)++;
    }else{
        printf("Rejeito\n");
        exit(0);
    }
    return;
}

void declaracaoVar(Token *tokens, int *p,char **variaveis,int *vari) {
    if (tokens[*p].tipo == Palavra_Reservada && strcmp(tokens[*p].token, "var") == 0) {
        (*p)++;
    } else {
        return;
    }

    while (tokens[*p].tipo == Identificador) {
        declaracao(tokens, p,variaveis,vari);

        if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ";") == 0) {
            (*p)++;
        }else {
            printf("Rejeito\n");
            exit(0);
        }
    }

    return;

}

void declaracao(Token *tokens, int *p,char **variaveis,int *vari) {
    while (tokens[*p].tipo == Identificador) {
        variaveis[*vari] = malloc(sizeof(char)*strlen(tokens[*p].token));
        strcpy(variaveis[*vari],tokens[*p].token);
        (*vari)++;
        (*p)++;
        if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ",") == 0) {
            (*p)++;
          if(tokens[*p].tipo != Identificador){
            printf("Rejeito\n");
            exit(0);
         }
        } else {
          break;
        }
    }

    if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ":") == 0) {
        (*p)++;
    } else {
        printf("Rejeito\n");
        exit(0);
    }

    if (tokens[*p].tipo == Identificador && (strcmp(tokens[*p].token, "integer") == 0 || strcmp(tokens[*p].token, "boolean") == 0)) {
        (*p)++;
    } else {
        printf("Rejeito\n");
        exit(0);
    }

    return;
}

void declaracaoSub(Token *tokens, int *p,char **variaveis,int *vari){
  while (tokens[*p].tipo == Palavra_Reservada && (strcmp(tokens[*p].token, "procedure") == 0 || strcmp(tokens[*p].token, "function") == 0)) {
    if (strcmp(tokens[*p].token, "procedure") == 0) {
      declaracaoProcedure(tokens, p,variaveis,vari);

      if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ";") == 0) {
        (*p)++;
      }else {
        return;
      }
    } else if (strcmp(tokens[*p].token, "function") == 0) {
      declaracaoFunction(tokens, p,variaveis,vari);
      if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ";") == 0) {
        (*p)++;
      }else {
        return;
      }
    }
  }

  return;
}

void declaracaoProcedure(Token *tokens, int *p,char **variaveis,int *vari) {
    if (tokens[*p].tipo == Palavra_Reservada && strcmp(tokens[*p].token, "procedure") == 0) {
        (*p)++;
    } else {
        printf("Rejeito\n");
        exit(0);
    }


    if (tokens[*p].tipo == Identificador) {
        (*p)++;
    } else {
        printf("Rejeito\n");
        exit(0);
    }

     

    if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, "(") == 0) {
        (*p)++;
         
        while (1) {
            if (tokens[*p].tipo == Palavra_Reservada && strcmp(tokens[*p].token, "var") == 0) {
                (*p)++;
            }
            
            while (tokens[*p].tipo == Identificador) {
                (*p)++;
                if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ",") == 0) {
                  (*p)++;
                } else {
                  break;
                }
            }

            if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ":") == 0) {
              (*p)++;
            } else {
              printf("Rejeito\n");
              exit(0);
            }

            if (tokens[*p].tipo == Identificador) {
              (*p)++;
            } else {
              printf("Rejeito\n");
               exit(0);
            }


            if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ";") == 0) {
              (*p)++;
                if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ")") == 0) {
                  break;
                }
            } else{
                break;
            }
        }

        

        if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ")") == 0) {
            (*p)++;
        } else {
            printf("Rejeito\n");
            exit(0);
        }


    }

     

    if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ";") == 0) {
        (*p)++;
    } else {
        printf("Rejeito\n");
        exit(0);
    }

    bloco(tokens, p,variaveis,vari);
    return;
}


void declaracaoFunction(Token *tokens, int *p,char **variaveis,int *vari) {
  if (tokens[*p].tipo == Palavra_Reservada && strcmp(tokens[*p].token, "function") == 0) {
    (*p)++;
  } else {
    printf("Rejeito\n");
    exit(0);
  }

  if (tokens[*p].tipo == Identificador) {
    (*p)++;
  } else {
    printf("Rejeito\n");
    exit(0);
  }

  if(tokens[*p].tipo == Separador && strcmp(tokens[*p].token, "(")==0){
    (*p)++;  
  

    while (1) {
            if (tokens[*p].tipo == Palavra_Reservada && strcmp(tokens[*p].token, "var") == 0) {
                (*p)++;
            }
            
            while (tokens[*p].tipo == Identificador) {
                (*p)++;
                if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ",") == 0) {
                  (*p)++;
                } else {
                  break;
                }
            }

            if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ":") == 0) {
              (*p)++;
            } else {
              printf("Rejeito\n");
              exit(0);
            }

            if (tokens[*p].tipo == Identificador) {
              (*p)++;
            } else {
              printf("Rejeito\n");
               exit(0);
            }


            if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ";") == 0) {
              (*p)++;
                if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ")") == 0) {
                  break;
                }
            } else{
                break;
            }
        }

        if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ")") == 0) {
            (*p)++;
        } else {
            printf("Rejeito\n");
            exit(0);
        }

    }

  if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ":") == 0) {
    (*p)++;
  }else {
    printf("Rejeito\n");
    exit(0);
  }

  if (tokens[*p].tipo == Identificador) {
    (*p)++;
  } else {
    printf("Rejeito\n");
    exit(0);
  }

  if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ";") == 0) {
    (*p)++;
  }else {
    printf("Rejeito\n");
    exit(0);
  }

  bloco(tokens, p,variaveis,vari);
  return;
}

int comandoComposto(Token *tokens, int *p,char **variaveis,int *vari) {
    if (tokens[*p].tipo == Palavra_Reservada && strcmp(tokens[*p].token, "begin") == 0) {
      (*p)++;
    } else {
      return 0;
    }


    int c = comando(tokens, p,variaveis,vari);
    while (1) {
        if((c)==0){
          printf("Rejeito\n");
          exit(0);
        }
        if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ";") == 0) {
          (*p)++;
          c = comando(tokens, p,variaveis,vari);
        } else {
          break;
        }
    }



    if (tokens[*p].tipo == Palavra_Reservada && strcmp(tokens[*p].token, "end") == 0) {
      (*p)++;
      return 1;
    } else {
      printf("Rejeito\n");
      exit(0);
    }
   
    return  0;
}

int comando(Token *tokens, int *p,char **variaveis,int *vari) {
    if (tokens[*p].tipo == Numero) {
        (*p)++;
        if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ":") == 0) {
            (*p)++;
        } else {
            printf("Rejeito\n");
            exit(0);
        }
    }
    return comandoRotulo(tokens, p, variaveis, vari);
}

int comandoRotulo(Token *tokens, int *p, char **variaveis,int *vari) {
    if (atribuicao(tokens, p, variaveis, vari)) return 1;
    if (chamadaProcedure(tokens, p, variaveis, vari)) return 1;
    if (desvio(tokens, p, variaveis, vari)) return 1;
    if (comandoComposto(tokens, p, variaveis, vari)) return 1;
    if (comandoCondicional(tokens, p, variaveis, vari)) return 1;
    if (comandoRepetitivo(tokens, p, variaveis, vari)) return 1;
    if (write(tokens, p, variaveis, vari)) return 1;
    if (read(tokens, p, variaveis, vari)) return 1;

    return 1;
}

int compareStrings(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}


int atribuicao(Token *tokens, int *p, char **variaveis,int *vari) {
     char *chave = tokens[*p].token;
     qsort(variaveis, *vari, sizeof(char *), compareStrings);
     char **item = (char **) bsearch(&chave, variaveis, *vari, sizeof(char *), compareStrings);
 
    if (tokens[*p].tipo == Identificador && item != NULL) {

      int p2 = *p;
        while (1) {
            (*p)++;
            expressao(tokens, p, variaveis, vari);
            if (strcmp(tokens[*p].token, ",") == 0) {
              (*p)++;
              int t =  expressao(tokens, p, variaveis, vari);
              if(t==0){
                return 0;
              }
            } else {
                break;
            }
        }
    
        if (tokens[*p].tipo == Operador_Composto && strcmp(tokens[*p].token, ":=") == 0) {
            (*p)++;
            int j = expressao(tokens, p, variaveis, vari);
            if(j){
              return 1;
            }
        } else {
          *p = p2;
          return 0;
        }
    } else {
        return 0;
    }
    return 0;
}

int expressao(Token *tokens, int *p, char **variaveis,int *vari) {
    int resultado = expressaoSimples(tokens, p, variaveis, vari);
    if ((tokens[*p].tipo ==Operador && (strcmp(tokens[*p].token,"=") == 0 || strcmp(tokens[*p].token, "<") == 0 || strcmp(tokens[*p].token, ">") == 0)) || (tokens[*p].tipo == Operador_Composto && (strcmp(tokens[*p].token, "<=") == 0 || strcmp(tokens[*p].token, ">=") == 0 || strcmp(tokens[*p].token, "<>") == 0))){
      (*p)++;
      if(expressaoSimples(tokens, p, variaveis, vari)==0){
          printf("Rejeito\n");
          exit(0);
      }
    }
    return resultado;
}

int expressaoSimples(Token *tokens, int *p, char **variaveis,int *vari) {
    if (tokens[*p].tipo == Operador && (strcmp(tokens[*p].token, "+") == 0 || strcmp(tokens[*p].token, "-") == 0)) {
        (*p)++;
    }
    int t = termo(tokens, p, variaveis, vari);
    while (t) {
        if (strcmp(tokens[*p].token, "+") == 0 || strcmp(tokens[*p].token, "-") == 0 || strcmp(tokens[*p].token, "or") == 0) {
            (*p)++;
            t = termo(tokens, p, variaveis, vari);
        } else {
            break;
        }
    }
    return t;
}

int termo(Token *tokens, int *p, char **variaveis,int *vari) {
    int f = fator(tokens, p, variaveis, vari);
    while (f) {
        if (strcmp(tokens[*p].token, "*") == 0 || strcmp(tokens[*p].token, "div") == 0 || strcmp(tokens[*p].token, "and") == 0) {
            (*p)++;
            f = fator(tokens, p, variaveis, vari);
        } else {
            break;
        }
    }
    return f;
}

int fator(Token *tokens, int *p, char **variaveis,int *vari) {
    if (tokens[*p].tipo == Identificador) {
      int p2 = *p;
        while (1) {
            (*p)++;
            expressao(tokens, p, variaveis, vari); //arrumar aqui
            if (strcmp(tokens[*p].token, ",") == 0) {
                (*p)++;
                int t =expressao(tokens, p, variaveis, vari);
                if(t==0){
                  return 0;
                }
            } else {
                break;
            }
        }
          int p3 = *p;
          *p = p2;
          if(chamadaProcedure(tokens,p, variaveis,vari)){
            return 1;
          }else{
            *p = p3;
            return 1;
          }

       
        
    } else if (tokens[*p].tipo == Numero) {
        (*p)++;
        return 1;
    }else if(chamadaProcedure(tokens,p, variaveis, vari)){
      (*p)++;
      return 1;
    } else if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, "(") == 0) {
        (*p)++;

        if (!expressao(tokens, p, variaveis, vari)) {
          return 0;
        }

        if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ")") == 0) {
            (*p)++;
            return 1;
        } else {
            return 0;
        }
    } else if (strcmp(tokens[*p].token, "not") == 0) {
        (*p)++;
        return fator(tokens, p, variaveis, vari);
    } else {
      return 0;
    }
}

void variavel(Token *tokens, int *p, char **variaveis,int *vari) {
    if (tokens[*p].tipo == Identificador) {
        (*p)++;
        while (1) {
            expressao(tokens, p, variaveis, vari);
            if (strcmp(tokens[*p].token, ",") == 0) {
                (*p)++;
            } else {
                break;
            }
        }
    }
}

int desvio(Token *tokens, int *p, char **variaveis,int *vari) {
    if (tokens[*p].tipo == Palavra_Reservada && strcmp(tokens[*p].token, "goto") == 0) {
        (*p)++;
        if (tokens[*p].tipo == Numero) {
            (*p)++;
            return 1;
        } else {
            printf("Rejeito\n");
            exit(0);
        }
    }
    return 0;
}

int comandoCondicional(Token *tokens, int *p, char **variaveis,int *vari) {
    if (tokens[*p].tipo == Palavra_Reservada && strcmp(tokens[*p].token, "if") == 0) {
        (*p)++;
        if (!expressao(tokens, p, variaveis, vari)) {
          printf("Rejeito\n");
          exit(0);
        }
        if (tokens[*p].tipo == Palavra_Reservada && strcmp(tokens[*p].token, "then") == 0) {
            (*p)++;
            if(comandoRotulo(tokens, p, variaveis, vari)==0) {
              printf("Rejeito\n");
              exit(0);
            }
            if (tokens[*p].tipo == Palavra_Reservada && strcmp(tokens[*p].token, "else") == 0) {
                (*p)++;
                if (comandoRotulo(tokens, p, variaveis, vari)==0) {
                  printf("Rejeito\n");
                  exit(0);
                }
            }
            return 1;
        } else {
            printf("Rejeito\n");
            exit(0);
        }
    }
    return 0;
}

int comandoRepetitivo(Token *tokens, int *p, char **variaveis,int *vari) {
    if (tokens[*p].tipo == Palavra_Reservada && strcmp(tokens[*p].token, "while") == 0) {
        (*p)++;
        if (!expressao(tokens, p, variaveis, vari)) {
            printf("Rejeito\n");
            exit(0);
        }
        if (tokens[*p].tipo == Palavra_Reservada && strcmp(tokens[*p].token, "do") == 0) {
            (*p)++;
            if (!comandoRotulo(tokens, p, variaveis, vari)) {
                printf("Rejeito\n");
                exit(0);
            }
            return 1;
        } else {
            printf("Rejeito\n");
            exit(0);
        }
    }
    return 0;
}

int chamadaProcedure(Token *tokens, int *p, char **variaveis,int *vari) {

    if (tokens[*p].tipo == Identificador) {
        (*p)++;
        if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, "(") == 0) {
            (*p)++;
            while (1) {
                expressao(tokens, p, variaveis, vari);
                if (strcmp(tokens[*p].token, ",") == 0) {
                  (*p)++;
                } else {
                    break;
                }
            }


            if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ")") == 0) {
                (*p)++;
            } else {
                printf("Rejeito aq\n");
                exit(0);
            }

        }
        return 1;
    }
    return 0;
}

int write(Token *tokens, int *p, char **variaveis,int *vari) {
    if (tokens[*p].tipo == Palavra_Reservada && strcmp(tokens[*p].token, "write") == 0) {
        (*p)++;
        if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, "(") == 0) {
            (*p)++;
            while (1) {
                expressao(tokens, p, variaveis, vari);
                if (strcmp(tokens[*p].token, ",") == 0) {
                    (*p)++;
                } else {
                    break;
                }
            }
            if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ")") == 0) {
                (*p)++;

                return 1;
            } else {
                printf("Rejeito\n");
                exit(0);
            }
        } else {
            printf("Rejeito\n");
            exit(0);
        }

    }
    return 0;
}

int read(Token *tokens, int *p, char **variaveis,int *vari) {
    if (tokens[*p].tipo == Palavra_Reservada && strcmp(tokens[*p].token, "read") == 0) {
        (*p)++;
        if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, "(") == 0) {
            (*p)++;
            variavel(tokens, p, variaveis, vari);
            if (tokens[*p].tipo == Separador && strcmp(tokens[*p].token, ")") == 0) {
                (*p)++;
                return 1;
            } else {
                printf("Rejeito\n");
                exit(0);
            }
        } else {
            printf("Rejeito\n");
            exit(0);
        }
    }
    return 0;
}

void sintia(Token *tokens, char **variaveis){
    int currentT=0;
    int vari = 0;
    int label = 0;

    program(tokens,&currentT,variaveis,&vari);

    if (tokens[currentT].tipo == Separador && strcmp(tokens[currentT].token, ".") == 0) {
        geraldo(tokens,&currentT,variaveis,&vari,&label);
        //printf("Aceito\n");
    } else {
        exit(0);
        //printf("Rejeito\n");
    }

    return;
}

void handleComparison(char *token) {
    if (strcmp(token, ">") == 0) {
        printf("CMMA\n");
    } else if (strcmp(token, "<") == 0) {
        printf("CMME\n");
    } else if (strcmp(token, ">=") == 0) {
        printf("CMAG\n");
    } else if (strcmp(token, "<=") == 0) {
        printf("CMEG\n");
    } else if (strcmp(token, "=") == 0) {
        printf("CMIG\n");
    } else if (strcmp(token, "<>") == 0) {
        printf("CMDG\n");
    }else if (strcmp(token, "+")==0) {
        printf("SOMA\n");
    }else if (strcmp(token, "-")==0) {
        printf("SUBT\n");
    }else if (strcmp(token, "*")==0) {
        printf("MULT\n");
    }
}


void geraldo(Token *tokens, int *p, char **variaveis,int *vari,int *label){
    for(int c = 0; c < *p; c++){
        if(tokens[c].tipo == Palavra_Reservada && strcmp(tokens[c].token, "program") == 0){
            printf("INPP\n");
        }
        if(tokens[c].tipo == Palavra_Reservada && strcmp(tokens[c].token, "var") == 0){
            printf("AMEM ");
            c = c+1;
            int qtd = 0;
            while(1){
                if(tokens[c].tipo == Palavra_Reservada){
                    break;
                }
                if(strcmp(tokens[c].token, ",")==0 || strcmp(tokens[c].token, "integer") == 0 || strcmp(tokens[c].token, ":") == 0 || strcmp(tokens[c].token, ";") == 0){
                    c++;
                }else{
                    qtd++;
                    c++;
                }
            }
            printf("%d\n",qtd);
        }
        if (strcmp(tokens[c].token, ":=") == 0) {
            c++;
            if (tokens[c].tipo == Numero) {
                printf("CRCT %s\n", tokens[c].token);
            } else {
                char *chave = tokens[c].token;
                char **item = (char **) bsearch(&chave, variaveis, vari, sizeof(char *), compareStrings);
                if (item != NULL) {
                    long int index = item - variaveis;
                    printf("CRVL 0, %ld\n", index);
                }
            }
            char *chave = tokens[c - 2].token;
            char **item = (char **) bsearch(&chave, variaveis, vari, sizeof(char *), compareStrings);
            if (item != NULL) {
                long int index = item - variaveis;
                printf("ARMZ 0, %ld\n", index);
            }
        }
        if(tokens[c].tipo == Palavra_Reservada && strcmp(tokens[c].token, "write")==0){
            c=c+2;
            if(tokens[c].tipo == Numero){
                printf("CRCT %s\n",tokens[c].token);
            }else{
                char *chave = tokens[c].token;
                char **item = (char **) bsearch(&chave, variaveis, *vari, sizeof(char *), compareStrings);
                long int index = item - variaveis;
                printf("CRVL 0, %ld\n",index);
            }
            printf("IMPR\n");
        } 
        if(tokens[c].tipo == Palavra_Reservada && strcmp(tokens[c].token, "read")==0){
            c=c+2;
            printf("LEIT\n");
            char *chave = tokens[c].token;
            char **item = (char **) bsearch(&chave, variaveis, *vari, sizeof(char *), compareStrings);
            long int index = item - variaveis;
            printf("ARMZ 0, %ld\n",index); 
        } 
      
        if(tokens[c].tipo == Numero){
            printf("CRCT %s\n",tokens[c].token);
        }
        if (strcmp(tokens[c].token, "+")==0) {
            printf("SOMA\n");
        }
        if (strcmp(tokens[c].token, "-")==0) {
            printf("SUBT\n");
        }
        if (strcmp(tokens[c].token, "*")==0) {
            printf("SUBT\n");
        }
        if (strcmp(tokens[c].token, ">")==0) {
            printf("CMMA\n");
        }
        
    }
    printf("PARA\n");
}


int main(int argc, char *argv[]) {
  FILE *arquivo;
  Qtd *qtd = malloc(sizeof(Qtd));
  char caractere = ' '; 


  if (argc != 2) {
    printf("Uso: %s <nome_do_arquivo>\n", argv[0]);
    return 1;
  }

  arquivo = fopen(argv[1], "r");

  Token *tokens = malloc(sizeof(Token)*MAX);
  char **variaveis = malloc(sizeof(char*)*MAX);
 
  alex(arquivo,qtd,tokens);
  sintia(tokens,variaveis);
  
  fclose(arquivo);
  return 0;
}
