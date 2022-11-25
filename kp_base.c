#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>

int main()
{
    //num_items Define o numero de items que podem ser postos dentro da mala
    int num_items = 0;
    //weight é o peso maximo que a mala pode levar
    int weight = 0;
    //processes define o numero de processos que vão ser criados
    int processes = 1;
    //O array values vai guardar os valores de cada item 
    int values[50];
    //o array weights vai guardar os pesos dos items
    int weights[50];

    //Vai criar a memoria partilhada
    int size = 64;
    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_ANONYMOUS | MAP_SHARED;
    void *shmem = mmap(NULL, size, protection, visibility, 0, 0);

    for(int i = 0; i < processes; i++){
        if (fork() == 0){
            int result = sol_kb(weight, weights, values, num_items);
            //vai correr o algoritemo durante o tempo pedido no comando
            //Vai fazer isto com um loop while
            //A cada solução que encontrar, vai fazer o output "Child x has found solution y at time z"
        } else {
            //vai matar os filhos
        }
    }

    return 0;
}

//Resolve o problema e devolve um valor para cada processo depois compara com a solução do ficheiro
//e puder colocar em memória partilhada
int sol_kp(int weight, int weights[], int values[], int num_items){
    if (num_items == 0 || weight == 0){
        return 0;
    }

    if(weights[num_items - 1] > weight){
        return sol_kp(weight, weights, values, num_items - 1);
    }
    else {
        return max(values[num_items - 1] + sol_kp(weight - weights[num_items - 1], weight, values, num_items - 1), sol_kp(weight, weights, values, num_items - 1));
    }
}

int max(int x, int y){return (x > y) ? x : y;}