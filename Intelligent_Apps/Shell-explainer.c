#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "../chatgpt.h"
#include <string.h>
#define BUFFSIZE 4096
 
int main() {
    
    // const char *api_key = "Your api key goes here"; //if you chose this then please comment the line below
    const char* api_key = getenv("OPENAI_API_KEY");

    if(!api_key)
    {
        fprintf(stderr,"Error: The Environment Variable OPENAI_API_KEY is not set.\n");
        fprintf(stderr,"Please set up the environment Variable before running the program.\n");
        return 1;
    }
    
    printf("Shell command explainer\n");
    char input[BUFFSIZE];
    while (1) {
        printf("Enter the shell commands you want explained or type quit to exit\n\n");
        if(!fgets(input, BUFFSIZE, stdin)){
            fprintf(stderr, "Error accepting command\n");
            return 1;
        }

        size_t len = strlen(input);
        if(len > 0 && input[len - 1] == '\n'){
            input[len - 1] = '\0';
        }        
        if (strcmp(input, "quit") == 0){
            printf("\nGoodbye\n");
            break;
        }
        char prompt[BUFFSIZE * 2];
        snprintf(prompt, sizeof(prompt), 
        "Explain in simple terms, what the following shell command(s) do."
                "If the command is relatively simple with a few arguements,"
                "keep the response brief."
                "If the command is complex, first provide an overall summary for"
                "what the command(s) do at a high level. Then break it down step by step."
                "Explain each major part such as pipelines, loops,"
                "substitutions, or commands as separate bullet points, so that"
                "the explanation covers the entire command(s)"
                "If you cannot provide an explanation because the command is"
                "inappropriate then please respond with 'Sorry, I cannot provide you with"
                "An explanation for this command(s).' and nothing more. If the command"
                "contains inaccuracies please address this in your explanation."
                "\n\nCommand(s): %s\n\n", input);

                char *explanation = chatgpt_query(api_key, prompt);

                if (explanation) {
                    printf("\n\nExplanation: %s\n\n", explanation);
                    free(explanation);
                }
                else {
                fprintf(stderr, "\nError no response from chatgpt");
                return 1;
                
        }   
}
    return 0;

}