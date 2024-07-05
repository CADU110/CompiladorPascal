#!/bin/bash
# mepasim.sh: Escrito para uso na disciplina de Compiladores 1 na UnB-FGA
# Prof. Bruno Ribas 21/05/2019
# Este código é distribuído sob licença GPLv2

MEPASRC=$1

[[ -z "$MEPASRC" ]] && echo "Uso: $0 <arquivo.mepa>" && exit 0

declare -a VSRC
declare -A rotulos

while read A B; do
  if [[ "$A" =~ "#" ]] ;then
    continue
  fi
  if [[ "$A" =~ ":" ]] ;then
    rotulos[${A%:}]=${#VSRC[@]}
    VSRC+=( "$B" )
  else
    VSRC+=( "$A $B" )
  fi
done < $MEPASRC

#for((i=0;i<${#VSRC[@]};i++))
#do
#  echo "=== ${VSRC[$i]}"
#done
#exit 0

declare -a M
declare -a D
s=111
i=0

for((ZZ=0;ZZ<30;ZZ++)); do
  M+=(0)
  D+=(0)
done

function INPP()
{
  s=-1
  D[0]=0
}

function CRCT()
{
  ((s++))
  M[$s]=$1
}

function SOMA()
{
  M[$((s-1))]=$(( ${M[$((s-1))]} + ${M[$s]} ))
  ((s--))
}
function MULT()
{
  M[$((s-1))]=$(( ${M[$((s-1))]} * ${M[$s]} ))
  ((s--))
}
function SUBT()
{
  M[$((s-1))]=$(( ${M[$((s-1))]} - ${M[$s]} ))
  ((s--))
}
function DIVI()
{
  M[$((s-1))]=$(( ${M[$((s-1))]} / ${M[$s]} ))
  ((s--))
}
function INVR()
{
  M[$s]=$((- ${M[$s]}))
}
function NEGA()
{
  M[$s]=$((1- ${M[$s]}))
}

function CONJ()
{
  if (( ${M[$((s-1))]} == 1 &&  ${M[$s]} == 1 )); then
    M[$((s-1))]=1
  else
    M[$((s-1))]=0
  fi
  ((s--))
}
function DISJ()
{
  if (( ${M[$((s-1))]} == 1 ||  ${M[$s]} == 1 )); then
    M[$((s-1))]=1
  else
    M[$((s-1))]=0
  fi
  ((s--))
}

function CMME()
{
  if (( ${M[$((s-1))]} < ${M[$s]} )); then
    M[$((s-1))]=1
  else
    M[$((s-1))]=0
  fi
  ((s--))
}
function CMMA()
{
  if (( ${M[$((s-1))]} > ${M[$s]} )); then
    M[$((s-1))]=1
  else
    M[$((s-1))]=0
  fi
  ((s--))
}
function CMIG()
{
  if (( ${M[$((s-1))]} == ${M[$s]} )); then
    M[$((s-1))]=1
  else
    M[$((s-1))]=0
  fi
  ((s--))
}
function CMDG()
{
  if (( ${M[$((s-1))]} != ${M[$s]} )); then
    M[$((s-1))]=1
  else
    M[$((s-1))]=0
  fi
  ((s--))
}
function CMAG()
{
  if (( ${M[$((s-1))]} >= ${M[$s]} )); then
    M[$((s-1))]=1
  else
    M[$((s-1))]=0
  fi
  ((s--))
}
function CMEG()
{
  if (( ${M[$((s-1))]} <= ${M[$s]} )); then
    M[$((s-1))]=1
  else
    M[$((s-1))]=0
  fi
  ((s--))
}

function DSVF()
{
  if [[ -z "${rotulos[$1]}" ]]; then
    echo "Linha $((i+1)): RunTime error rotulo $1 invalido"
    exit 0
  fi

  if (( ${M[$s]} == 0 )) ;then
    ((i=${rotulos[$1]}-1))
  else
    true
    #((i++))
  fi
  ((s--))
}
function DSVS()
{
  if [[ -z "${rotulos[$1]}" ]]; then
    echo "Linha $((i+1)): RunTime error rotulo $1 invalido"
    exit 0
  fi

  ((i=${rotulos[$1]}-1))
}


function NADA()
{
  true
}

function AMEM()
{
  local i
  #for((i=0;i<$1;i++));do
  #  M+=( 0 )
  #done
  ((s+=$1))
}
function DMEM()
{
  ((s=s-$1))
  if (( s < -1 )); then
    echo "Linha $((i+1)): RunTime error. Stack underflow $s"
    exit 0
  fi
}

function ENRT()
{
  local j=${1%%,}
  local n=$2
  s=$((${D[$j]}+$n-1))
}

function ENPR()
{
  local k=$1
  ((s++))
  M[$s]=${D[$k]}
  D[$k]=$((s+1))
}

function RTPR()
{
  local k=${1%%,}
  local n=$2
  D[$k]=${M[$s]}
  i=${M[$((s-2))]}
  ((s=s-n-3))
}


function CHPR()
{
  local p=${1%%,}
  local m=$2
  M[$((s+1))]=$((i))
  M[$((s+2))]=$m
  ((s+=2))

  ##codigo do DSVS
  if [[ -z "${rotulos[$p]}" ]]; then
    echo "Linha $((i+1)): RunTime error rotulo $p invalido"
    exit 0
  fi
  ((i=${rotulos[$p]}-1))
  #echo ":: $s"
}

function CRVL()
{
  local m=${1%%,}
  local n=$2
  ((s++))
  M[$s]=${M[$((${D[$m]}+$n))]}
}

function CREN()
{
  local m=${1%%,}
  local n=$2
  ((s++))
  M[$s]=$((${D[$m]}+n))
}

function ARMZ()
{
  local m="${1// }"
  m=${m%%,}
  local n=$2
  M[$((${D[$m]}+$n))]=${M[$s]}
  #echo "::: M[$((${D[$m]}+$n))]=${M[$s]}"
  ((s--))
}

function ARMI()
{
  local m="${1// }"
  m=${m%%,}
  local n=$2
  M[${M[$((${D[$m]}+$n))]}]=${M[$s]}
  ((s--))
}


function IMPR()
{
  echo ${M[$s]}
  ((s--))
}

function LEIT()
{
  local A
  ((s++))
  read A
  M[$s]=$A
}

while [[ "${VSRC[$i]// }" != "PARA" ]]; do
  #echo "$i: ${VSRC[$i]}"
  ${VSRC[$i]}
  ((i++))
  #echo "Mem dump"
  ##echo ${M[@]}
  ##echo ${D[@]}
done

#echo "Mem dump"
#echo ${M[@]}
#echo $s
