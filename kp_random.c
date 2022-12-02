#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

#define MAX 100

int sol_kp(int weight, int weights[], int values[], int num_items);
int max(int x, int y);

int main(int argc, char *argv[])
{
    if (argc != 4){
        printf("Usage: kp_base testNum fileName numProcesses time");
        return -1;
    }

    //num_items Define o numero de items que podem ser postos dentro da mala
    int num_items = rand() % 11;
    int weight = rand() % 25;
    //weight é o peso maximo que a mala pode levar
    //processes define o numero de processos que vão ser criados
    int processes = atoi(argv[2]);
    //time define o tempo que o programa vai correr
    int time = atoi(argv[3]);
    //O array values vai guardar os valores de cada item 
    int values[MAX];
    for (int i = 0; i < num_items; i++){
        values[i] = rand() % 100;
    }
    //o array weights vai guardar os pesos dos items
    int weights[MAX];
    for (int x = 0; x < num_items; x++){
        weights[x] = rand() % 25;
    }
    int pids[processes];

    //Vai criar a memoria partilhada
    int size = 64;
    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_ANONYMOUS | MAP_SHARED;
    void *shmem = mmap(NULL, size, protection, visibility, 0, 0);

    sem_unlink("mymutex");
    sem_t *sem1 = sem_open("mymutex", O_CREAT, 0644, 1);

    *((int *)shmem) = 1;
    for(int i = 0; i < processes; i++){
        if ((pids[i] = fork()) == 0){
            //vai correr o algoritemo durante o tempo pedido no comando
            //Vai fazer isto com um loop while
            //A cada solução que encontrar, vai fazer o output "Child x has found solution y at time z"
            do {
                int result = sol_kp(weight, weights, values, num_items);
                sem_wait(sem1);
                if(result > *((int *)shmem)){
                    *((int *)shmem) = result;
                    printf("%s%d%s%d%s", "Child ", pids[i]," has found a solution: ", result, "\n");
                }
                sem_post(sem1);
                result = 0;
            } while(1);
        }
    }
    //O pai dorme até ao fim do tempo de execução
    sleep(time);
    //Agora vai matar os filhos
    for(int x = 0; x < processes; x++){
        kill(pids[x], SIGKILL);
    }

    //Agora vai aceder a memoria partilhada e vai escrever o valor final no ecrã e vai terminar o program
    sem_wait(sem1);
    printf("%s%d%s", "Best Overall Solution Found: ", *((int *)shmem), "\n");
    sem_post(sem1);
    sem_close(sem1);

    return (EXIT_SUCCESS);
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
        return max(values[num_items - 1] + sol_kp(weight - weights[num_items - 1], weights, values, num_items - 1), sol_kp(weight, weights, values, num_items - 1));
    }
}

int max(int x, int y){return (x > y) ? x : y;}