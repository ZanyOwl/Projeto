#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

void signal_handler(int signal){
    printf("Killed Child %d\n", signal);
}

int main(int argc, char *argv[])
{
    if (argc != 5){
        printf("Usage: kp_base testNum fileName numProcesses time");
        return -1;
    }

    char fileName[50];
    strcpy(fileName, argv[2]);
    
    FILE *testFile = fopen(fileName, "r");
    if(testFile == NULL){
        printf("ERROR: File returned NULL!!!!");
        return -1;
    }
    //num_items Define o numero de items que podem ser postos dentro da mala
    int num_items = 0;
    //weight é o peso maximo que a mala pode levar
    int weight = 0;
    //processes define o numero de processos que vão ser criados
    int processes = atoi(argv[3]);
    //time define o tempo que o programa vai correr
    int time = atoi(argv[4]);
    //O array values vai guardar os valores de cada item 
    int values[50];
    //o array weights vai guardar os pesos dos items
    int weights[50];
    int pids[processes];

    //Vai criar a memoria partilhada
    int size = 64;
    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_ANONYMOUS | MAP_SHARED;
    void *shmem = mmap(NULL, size, protection, visibility, 0, 0);

    sem_unlink("mymutex");
    sem_t *sem1 = sem_open("mymutex", O_CREAT, 0644, 1);

    signal(SIGUSR1, signal_handler);

    for(int i = 0; i < processes; i++){
        if ((pids[i] = fork()) == 0){
            //vai correr o algoritemo durante o tempo pedido no comando
            //Vai fazer isto com um loop while
            //A cada solução que encontrar, vai fazer o output "Child x has found solution y at time z"
            while(1){
                int result = sol_kb(weight, weights, values, num_items);
                sem_wait(sem1);
                if(result < shmem){
                    shmem = result;
                    printf("%s%i%s%i%s%i", "Child", getpid(),"has foand a solution: ", result, "at time", time);
                }
                sem_post(sem1);
                result = 0;
            }
        }
    }
    //O pai dorme até ao fim do tempo de execução
    sleep(time);
    //Agora vai matar os filhos
    for(int x = 0; x < processes; x++){
        kill(pids[x], SIGUSR1);
    }

    //Agora vai aceder a memoria partilhada e vai escrever o valor final no ecrã e vai terminar o program
    sem_wait(sem1);
    printf("%s%i", "Best Overall Solution Found: ", shmem);
    sem_post(sem1);
    sem_close(sem1);

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