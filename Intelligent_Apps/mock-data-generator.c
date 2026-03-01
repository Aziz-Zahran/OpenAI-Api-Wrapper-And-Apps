#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../chatgpt.h"
 
int main(int argc, char *argv[]) {
    
    // const char *api_key = "Your api key goes here"; //if you chose this then please comment the line below
    const char* api_key = getenv("OPENAI_API_KEY");

    if(!api_key)
    {
        fprintf(stderr,"Error: The Environment Variable OPENAI_API_KEY is not set.\n");
        fprintf(stderr,"Please set up the environment Variable before running the program.\n");
        return 1;
    }

    // Check if we have alteast 3 comand line arguments
    // if not, either file name or the description of the data is missing
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <output_filename> <mock data description>",
               argv[0]);
               return 1;
    }    
    const char *output_filenam = argv[1]; // stores the second argument as a pointer to string
    const char *mock_data_description = argv[2]; // stores the third argument as a pointer to string
    
    
    const char *prefix = 
        "You are generating mock data for a file that may be used for "
        "purposes such as software testing. Respond with ONLY the mock data and no other text or information. Here is the description"
        "of the mock data to provide: ";

    size_t prompt_length =strlen(prefix) + strlen(mock_data_description) + 1;
    char *prompt = malloc(prompt_length);
    if (!prompt) {
        fprintf(stderr, "Error allocating space for prompt.\n");
        return 1;
    }
    snprintf(prompt, prompt_length, "%s%s", prefix, mock_data_description);

    char *mock_data = chatgpt_query(api_key, prompt);

    free(prompt);


    if (!mock_data) {
        fprintf(stderr, "Error in receiving mock data from chatgpt.\n");
        return 1;
    }

    FILE *file = fopen(output_filenam, "w");
    if (!file) {
        fprintf(stderr, "Error opening output file");
        free(mock_data);
        return 1;
    }

    fputs(mock_data, file);
    fclose(file);
    
    printf("\nMock data written to: %s\n\n", output_filenam);

    free(mock_data);


    return 0;
}