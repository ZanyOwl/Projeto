#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>

int main()
{
    //n Define o numero de items que podem ser postos dentro da mala
    int n;
    //weight é o peso maximo que a mala pode levar
    int weight;
    //A matriz values vai ser do tamanho do numere de items e vai guarda as combinações de pares
    //que vem no ficheiro de texto  
    int values[n][50];

    //Vai criar a memoria partilhada
    int size = 64;
    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_ANONYMOUS | MAP_SHARED;
    void *shmem = mmap(NULL, size, protection, visibility, 0, 0);

    return 0;
}

//Função de apoio que vai fazer a soma dos valores dos pares e vai retornar o resultado para
//depois ser comparada com o valor da solução na memoria partilhada. 
int calc_weight_sum(int values[]){
    int result = 0;
    int arr_size = sizeof(values)/sizeof(values[0]);
    for (int i; i < arr_size; i++){
        int result = result + values[i];
    }
    return result;
}