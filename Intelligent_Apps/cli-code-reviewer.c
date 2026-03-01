#include <stdio.h>
#include <stdlib.h>
#include "../chatgpt.h"
#include <string.h>

char *read_file(const char *filename){
    FILE *fp = fopen(filename, "r");
    if(!fp) return NULL;

    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    rewind(fp);

     char *buffer = malloc(length + 1);
     if (!buffer) {
        fclose(fp);
        return NULL;
     }
     fread(buffer, 1 , length, fp);
     buffer[length] = '\0';
     fclose(fp);

     return buffer;
}


int main(int argc, char *argv[]) {
    
    // const char *api_key = "Your api key goes here"; //if you chose this then please comment the line below
    const char* api_key = getenv("OPENAI_API_KEY");

    if(!api_key)
    {
        fprintf(stderr,"Error: The Environment Variable OPENAI_API_KEY is not set.\n");
        fprintf(stderr,"Please set up the environment Variable before running the program.\n");
        return 1;
    }
    
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filename> <review_mode> \n Use %s <filename> help for review options\n ", argv[0], argv[0]);
        return 1;
    }

    if (strcmp(argv[2],"DEEP") != 0 && strcmp(argv[2],"SIMPLE") != 0) {
        fprintf(stderr, "Second parameter must be either 'DEEP' or 'SIMPLE'\n");
        return 1;
    }
    char *file_contents = read_file(argv[1]);
    if (!file_contents) {
        fprintf(stderr, "Error in reading the file %s\n", argv[1]);
        return 1;
    }
    
    const char *template =
             "You are a senior software engineer reviewing a source code file provided by the user."
             "Infer the programming language. Provide a brief, concise, professional code review "
             "focused on correctness, safety, clarity, and maintainability. Do not assume external requirements "
             "or missing files unless clearly implied by the code. Do not rewrite the code. "
             "First give a brief summary explaining what the file appears to do and its likely "
             "role in the project, then list major issues that must be fixed, followed by minor issues, "
             "design or maintainability concerns, and suggested tests. If the review mode is SIMPLE, "
             "perform a high-level scan and report only obvious or high-impact problems with minimal verbosity. "
             "If the review mode is DEEP, perform a careful technical review that considers control flow, "
             "edge cases, memory or resource safety, and unclear assumptions. Use concise bullet points and refer "
             "to functions or behaviors by description rather than line numbers. If you cannot provide an explanation "
             "because the file is inappropriate, then please respond with, 'Sorry, I cannot provide you a review for this file.'and nothing more."
             "Here are the file contents: \n\n%s";
    size_t prompt_length = strlen(template) + strlen(file_contents); 
    char *prompt = malloc(prompt_length);
    if (!prompt) {
        free(file_contents);
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return 1;
    }
    strcpy(prompt, template);
    strcat(prompt, file_contents);

    char *response = chatgpt_query(api_key, prompt);
    if (!response) {
        fprintf(stderr, "Error in obtaining a response from ChatGPT");
        return 1;
    }
    printf("%s",response);


    free(file_contents);

    return 0;
}