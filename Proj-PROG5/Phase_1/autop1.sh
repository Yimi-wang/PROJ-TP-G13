#!/bin/bash

erreur () {
    echo $* 1>&2
    exit 1
}
GREEN='\033[0;32m'
NC='\033[0m' # No Color
for e in "${@:2}"
do
    case $e in
    1)  
        printf "${GREEN}/////ETAPE 1/////  :${NC}\n"
        ./Pharse1 -h examples/"$1".o
        printf "\nreadelf :\n"
        readelf examples/"$1".o -h | sed -n -E '/^  Machine|  Type|  Size|  Number of section|  Section header|  Class/p'
        ;;
    2)
        printf "${GREEN}/////ETAPE 2/////  :${NC}\n"
        ./Pharse1 -S examples/"$1".o
        printf "\nreadelf :\n" 
        readelf examples/"$1".o -S | sed -n -E '/^Section Headers|  \[/p'
        ;; 
    3)
        printf "${GREEN}/////ETAPE 3/////  :${NC}\n"
        ./Pharse1 -x .text examples/"$1".o
        printf "\nreadelf :\n" 
        readelf examples/"$1".o -x .text | sed -n -E '/^ NOTE/!p'
        ;;
    4)
        printf "${GREEN}/////ETAPE 4/////  :${NC}\n"
        ./Pharse1 -s examples/"$1".o
        printf "\nreadelf :\n"
        readelf examples/"$1".o -s
        ;;
    5)
        printf "${GREEN}/////ETAPE 5/////  :${NC}\n"
        ./Pharse1 -r examples/"$1".o
        printf "\nreadelf :\n" 
        readelf examples/"$1".o -r | awk '{print $1,"   "$3}'
        ;;
    *)
        printf "mauvais num etape \n"
        ;;
    esac
    printf "\n \n"
done
