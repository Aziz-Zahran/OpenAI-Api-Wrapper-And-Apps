#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../chatgpt.h"
#include "../cJSON.h"

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
    
    char input[BUFFER_SIZE];

    char*full_story = strdup("You are the narrator of an interactive fantasy"
                            "adventure story."
                            "You are going to produce the story one part at a time,"
                            " as there will be a pause for the user interaction "
                            "in between each part of the story."
                            "Each part of the story should be brief. no more "
                            "than a couple of sentences of text. "
                            "Make the story fun and creative. " 
                            "Begin the story in JSON with two keys: "
                            "\"next_part\" (what the player sees next in the story) and "
                            "\"full_story\" (everything so far).\n");

    if (!full_story) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return 1;
    }

    printf("\n\nWelcome to forever story! The story keeps going forever and ever. \n\n");

    while (1) {
    char *response = chatgpt_query(api_key, full_story);

    if (!response) {
        fprintf(stderr, "Error: Chatgpt request failed.");
        free(full_story);
        return 1;
    }
    
    cJSON *json = cJSON_Parse(response);
    free(response);

    if(!json) {
        fprintf(stderr, "Error: Failed to parse JSON");
        free(full_story);
        return 1;
    }

    cJSON *next_part = cJSON_GetObjectItem(json, "next_part");
    cJSON *new_full_story = cJSON_GetObjectItem(json, "full_story");

    if (!cJSON_IsString(next_part) || !cJSON_IsString(new_full_story)) {
        fprintf(stderr, "Error: invalid JSON format from ChatGPT");
        free(full_story);
        return 1;
    }

    printf("\n%s\n", next_part->valuestring);

    free(full_story);
    full_story = strdup(new_full_story->valuestring);
    if (!full_story) {
        fprintf(stderr, "Error: Memory allocation failed");
        cJSON_Delete(json);
        return 1;
    }

    cJSON_Delete(json);

    printf("\nWhat do u want to do? (enter 'quit' to exit)\n>");
    if (!fgets(input, BUFFER_SIZE, stdin)) {
        free(full_story);
        fprintf(stderr, "Could not obtain user input\n");
        return 1;
    }

    size_t input_len = strlen(input);
    if(input_len > 0 && input[input_len - 1 == '\n']) {
        input[input_len - 1] = '\0';
    }
    
    if (strcmp(input, "quit") == 0){
        printf("\n\n Thanks for playing!\n\n");
        break;
    }

    size_t new_len = strlen(full_story) + strlen(input) + 512;
    char *extended = malloc(new_len);
    if (!extended) {
        fprintf(stderr, "Error: Memory allocation failed");
        free(full_story);
        return 1;
    }

    snprintf(extended, new_len,
         "You are the narrator of an interactive fantasy"
                "adventure story."
                "You are going to produce the story one part at a time,"
                " as there will be a pause for the user interaction "
                "in between each part of the story."
                "Each part of the story should be brief. no more "
                "than a couple of sentences of text. "
                "Make the story fun and creative. " 
                "You have already produced the story below so far: "
                "%s\nNow the player chooses: %s\nRespond again with"
                "JSON {\"next_part\"}:..., \"full_story\":...",
                full_story, input);
    free(full_story);
    full_story = extended;
    }

    free(full_story);
    return 0;
}