#include <stdio.h>
#include <stdlib.h>
#include "../chatgpt.h"
 
int main() {
    
    // const char *api_key = "Your api key goes here"; //if you chose this then please comment the line below
    const char* api_key = getenv("OPENAI_API_KEY");

    if(!api_key)
    {
        fprintf(stderr,"Error: The Environment Variable OPENAI_API_KEY is not set.\n");
        fprintf(stderr,"Please set up the environment Variable before running the program.\n");
        return 1;
    }
    const char *prompt = "What is the capital of france";

    char *reply = chatgpt_query(api_key, prompt);

    if (reply) 
    {
        printf("Chatgpt says: %s\n", reply);
        free(reply);
    }
    else
    {
        fprintf(stderr, "Failed to get response .\n");
    }

    return 0;
}