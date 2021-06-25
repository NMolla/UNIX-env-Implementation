//
//  env.c
//  homework2
//
//  Created by Nahom Molla on 12/02/2021.
//
//  sets environ to point to a new array in the heap
//  program assumes execvp() frees up the all the malloc()

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


char** createEnvironment(const char* argv[], int copyFlag, int numArgs, int envLength);
char** copyEnvironment(int copyFlag, int numArgs, int envLength);
int countArguments(const char* argv[], int index);
int countEnvironLength(void);
int inEnvironment(char** res, char* keyValuePair);
void displayEnviron(void);


extern char** environ;


int main(int argc, const char * argv[]) {
    if (argc == 1){ // command line: ./env
        displayEnviron();
        return 0;
    }
    
    int copyEntries=0; // 0 == yes
    // (command line: ./env -i ...) || (command line: ./env - ...) ; ("-" == "-i")
    if ((strncmp(argv[1], "-i", 2) == 0) || (strncmp(argv[1], "-", 2) == 0)){
        copyEntries = -1;
    }
    
    int startIdx = 1 - copyEntries;
    int numArgs = countArguments(argv, startIdx);
    
    if (numArgs > 0){ //if ./env has key=value arguments
        environ = createEnvironment(argv, copyEntries, numArgs, countEnvironLength());
        if (environ == NULL){
            printf("malloc failed: environ points to NULL!\n");
            exit(1);
        }
    }
   
    if ((startIdx + numArgs) < argc){ // if ./env ... cmd exists
        if (execvp(argv[startIdx+numArgs], (char* const*)(argv + startIdx + numArgs)) == -1){ //casting to avoid warning
            perror("invalid command");
            exit(1);
        }
    }
    
    displayEnviron();
    exit(0);
}

char** createEnvironment(const char* argv[], int copyFlag, int numArgs, int envLength){
    int argIdx = 1 - copyFlag; // 1 if "-i || -" was not passed, 2 if "-i || -" was passed
    int i_key;
    int i_store = -1;
    char** res=NULL;
    char* arg=NULL;
    
    if (copyFlag == 0){ //malloc space for env_length + num_args and copy items
        res = copyEnvironment(copyFlag, numArgs, envLength);
    }
    else{ //copyFlag == -1; malloc space for num_args and copy items
        res = malloc((numArgs+1) * sizeof(char*));
        envLength = 0;
    }
    if (res == NULL){
        printf("failed to malloc() new environment array!\n");
        return NULL;
    }
    
    //add new entries
    for (int i=envLength; i<envLength+numArgs; ++i){
        arg = (char*)argv[argIdx];
        if (arg == NULL){
            return res;
        }
        
        i_key = inEnvironment(res, arg);
        if (i_key != -1){
            i_store = i;
            i = i_key;
        }
        
        res[i] = malloc((strlen(arg)+1) * sizeof(char));
        if (res[i] == NULL){
            printf("failed to malloc() pointer at index %d!\n", i);
            return NULL;
        }
        
        res[i] = arg;
        ++argIdx;
        
        if (i_key != -1){
            if (i_store == -1){
                printf("something is broken in createEnvironment()\n");
                exit(1);
            }
            i = i_store - 1;
        }
    }
    return res;
}

//copies the old environment variables into a new array on the heap
char** copyEnvironment(int copyFlag, int numArgs, int envLength){
    char** res = malloc((envLength + numArgs + 1) * sizeof(char*));
    if (res == NULL){
        printf("failed to malloc() new environment array!\n");
        return NULL;
    }
    for (int i=0; i<envLength; ++i){
        res[i] = malloc((strlen(environ[i]) + 1) * sizeof(char));
        if (res[i] == NULL){
            printf("failed to malloc() pointer at index %d!\n", i);
            return NULL;
        }
        res[i] = environ[i];
    }
    return res;
}

//counts the length of the environ array
int countEnvironLength(void){
    int count = 0;
    for(int i=0; environ[i] != NULL; ++i){
        ++count;
    }
    return count;
};

//counts the number of key=value pairs passed in argv[]
int countArguments(const char* argv[], int index){
    int count = 0;
    for (int i=index; argv[i] != NULL; ++i){
        if (strchr(argv[i], '=') == NULL){
            return count;
        }
        ++count;
    }
    return count;
}

//looks for an existing key in the array
int inEnvironment(char** arr, char* keyValuePair){
    int keyLength;
    int i;
    for (i=0; arr[i] != NULL; ++i){
        keyLength = (int)(strchr(keyValuePair, '=') - keyValuePair);
        if (strncmp(arr[i], keyValuePair, keyLength) == 0){
            return i; //index of entry
        }
    }
    return -1;
}

//prints the contents of the environ variable
void displayEnviron(void){
    if (environ == NULL){
        return;
    }
    for (int i=0; (environ[i] != NULL); ++i){
        //if some null value skip
        printf("%s\n", environ[i]);
    }
    return;
}
