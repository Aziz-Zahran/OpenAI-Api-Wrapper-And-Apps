#include <stdio.h>
#include <stdlib.h>
#include "../chatgpt.h"
#define BUFFER_SIZE 4096 


int main() {

    // const char *api_key = "Your api key goes here"; //if you chose this then please comment the line below
    const char* api_key = getenv("OPENAI_API_KEY");

    if(!api_key)
    {
        fprintf(stderr,"Error: The Environment Variable OPENAI_API_KEY is not set.\n");
        fprintf(stderr,"Please set up the environment Variable before running the program.\n");
        return 1;
    }
    
    char task[BUFFER_SIZE];

    printf("Enter the description of the desired shell task:\n>");
    if(!fgets(task, BUFFER_SIZE, stdin)) {
        fprintf(stderr, "Error: Error in reading the input.\n ");
        return 1;
    }
    const char *template = 
            "You are a Linux/Unix shell command assistant."
            "Given plain english description of a task, output the shell command(s)."
            "That will accomplish it, using bash-compatible syntax."
            "Only output the commands, nothing else."
            "If you cannot create the shell command(s) or the description of the task "
            "is not appropriate, please output instead 'Sorry I cannot create shell command(s)"
            "for this task.' and nothing else.\n\nTask: %s";

    char prompt[BUFFER_SIZE * 2];
    snprintf(prompt, sizeof(prompt), template, task);

    char *commands = chatgpt_query(api_key, prompt);

    if(!commands) {
        fprintf(stderr, "Error producing command(s).\n");
        return 1;
    }

    printf("\n\n=== Disclaimer ===");
    printf("\nThis program uses generative AI and may give commands");
    printf("\nthat are unhelpful or even harmful.");
    printf("\nYou are responsible for any damages done as a");
    printf("\nresult of using these commands");

    printf("\n\n=== Suggested Command(s) ===\n\n%s\n", commands);

    free(commands);
    return 0;
}