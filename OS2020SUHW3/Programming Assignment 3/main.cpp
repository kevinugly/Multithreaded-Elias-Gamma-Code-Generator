#include <iostream>
#include <fstream>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdio.h>
#include <math.h>
using namespace std;

static pthread_mutex_t bsem;
static int order;
static pthread_cond_t empty = PTHREAD_COND_INITIALIZER;

struct elias
{
    string code;
    string position;
};

char codeToBinaryToEliasGamma(string code)
{
    //elias gamma -> binary
    string binary = to_string(stoi(code));
    
    //binary -> int
    int number = 0;
    for(int x = 0; x < binary.length(); x++)
    {
        
        number += ((int)binary[x] - 48) * pow(2, binary.length() - x - 1);
    }
    
    //int -> char
    return char(number); //char
}

void *outputThread(void *arg)
{
    //similar to practice programming question from exam 2
    pthread_mutex_lock(&bsem); //critical section 1 open
    struct elias *pkt = (struct elias *)arg;

    while(stoi(pkt->position) != order) 
    {
        pthread_cond_wait(&empty,&bsem); //opens the mutex and also makes the thread wait
    }

    pthread_mutex_unlock(&bsem); //critical section 1 closed
    
    pthread_mutex_lock(&bsem); //critical section 2 open
    
    cout << codeToBinaryToEliasGamma(pkt->code);

    order++;
    
    pthread_cond_broadcast(&empty); //prvents timeout on moodle
    
    pthread_mutex_unlock(&bsem); //critical section 2 closed
    return NULL;
}

int main(int argc, char* argv[])
{
    string line = "";
    int length = 0;
    vector<string> c, p;
    while(getline(cin, line))
    {
        string cod, pos;
        bool noSpace = true;
        for(int x = 0; x < line.length(); x++)
        {
            if(noSpace == true)
            {
                if(line[x] == ' ')
                {
                    noSpace = false;
                }
                else
                {
                    cod += line[x];
                }
            }
            else
            {
                pos += line[x];
            }
        }
        c.push_back(cod);
        p.push_back(pos);
        length++;
    }

    struct elias e [length]; //struct array

    #define NTHREADS length
    pthread_t tid[NTHREADS];
    for(int x = 0; x < NTHREADS; x++)
    {
        e[x].code = c[x];
        e[x].position = p[x];
        if(pthread_create(&tid[x], NULL, outputThread, &e[x]))
        {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }

    for(int x = 0; x < NTHREADS; x++)
    {
        pthread_join(tid[x], NULL);
    }
    return 0;
}