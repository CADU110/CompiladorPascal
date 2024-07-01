#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX 256

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
}token;

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
  } else if (c == ';' || c == '.' || c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c==']' || c==',' || c==':') {
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




void alex(FILE *arquivo,Qtd *qtd){
  char *bufferTokens = calloc(MAX,sizeof(char));
  int posicsTokens = 0;

  qtd->id = 0;
  qtd->keyword = 0;
  qtd->numb = 0;
  qtd->op = 0;
  qtd->cop;
  qtd->delim;
  qtd->coments;
  qtd->uk;

  char *fita = calloc(MAX,sizeof(char));
  int coment = 0;
  Qtd *qtd2 = malloc(sizeof(Qtd));

  

  Estado estado = Inicio;
  while (fgets(fita, sizeof(fita), arquivo) != NULL) {

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
          if(fita[currentA]!=' ' && fita[currentA]!='\n'){
            qtd->uk++;
          }
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
          //printf("NUMERO:");
          //printf("%s\n",bufferTokens);
          posicsTokens = 0;
          estado = Inicio;
        }
      }else if(estado == Em_Operador_Composto){
        bufferTokens[posicsTokens] = '\0';
        qtd->cop++;
        //printf("OPERADOR_COMPOSTO:");
        //printf("%s\n",bufferTokens);
        posicsTokens = 0;
        estado = Inicio;
      }else if(estado == Em_Comentario){
        //arrumar comnetarios
        if(bufferTokens[posicsTokens-1]=='*'){
          coment = 1;
          memcpy(qtd2, qtd, sizeof(Qtd));
          qtd->delim++;
          qtd->op++;
          bufferTokens[posicsTokens-1] = '\0';
          estado = Inicio;
          posicsTokens = 0;
        }else if(bufferTokens[posicsTokens-1]==')'){
          if(coment==1){
            memcpy(qtd, qtd2, sizeof(Qtd));
            qtd->coments++;
            coment=0;
          }else{
            qtd->delim++;
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


int main(int argc, char *argv[]) {
  FILE *arquivo;
  Qtd *qtd = malloc(sizeof(Qtd));

  if (argc != 2) {
    printf("Uso: %s <nome_do_arquivo>\n", argv[0]);
    return 1;
  }

  arquivo = fopen(argv[1], "r");

  alex(arquivo,qtd);

  printf("KEYWORD: %d\n",qtd->keyword);
  printf("IDENTIFIER: %d\n",qtd->id);
  printf("NUMBER: %d\n",qtd->numb);
  printf("OPERATOR: %d\n",qtd->op);
  printf("COMPOUND OPERATOR: %d\n",qtd->cop);
  printf("DELIMITER: %d\n",qtd->delim);
  printf("COMMENTS: %d\n",qtd->coments);
  printf("UNKNOWN: %d\n",qtd->uk);

  
  fclose(arquivo);
  return 0;
}

