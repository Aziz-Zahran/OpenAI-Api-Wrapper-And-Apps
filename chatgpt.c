#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "cJSON.h"
#include "chatgpt.h"


struct memory {
  char *response; 
  size_t size;
};

/* Curl write callback function: https://everything.curl.dev/examples/getinmem.html
*  This function is called by libcurl whenever data is received from
*  the HTTP response. It dynamically reallocates the memory buffer to
*  accommodate the new data and appends it to the existing response.
*/
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t total_size = size * nmemb;
  struct memory *mem = (struct memory *) userp;

  char *ptr = realloc(mem->response, mem->size + total_size + 1);
  if(!ptr) return 0;

  mem->response = ptr;
  memcpy(&(mem->response[mem->size]), contents, total_size);
  mem->size += total_size;
  mem->response[mem->size] = 0;

  return total_size;
}

/*
* Makes request to ChatGPT API with a prompt.
* The function handles JSON construction, HTTP request setup, 
* response parsing and error handling. The function returns
* a pointer to dynamically allocated string containing
* the response to the provided prompt or NULL or error.
*/
char *chatgpt_query(const char *api_key, const char *prompt) {
  // Build JSON payload
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "model", "gpt-5");
    cJSON *messages = cJSON_CreateArray();
    cJSON *msg = cJSON_CreateObject();
    cJSON_AddStringToObject(msg, "role", "user");
    cJSON_AddStringToObject(msg, "content", prompt);
    cJSON_AddItemToArray(messages, msg);
    cJSON_AddItemToObject(root, "messages", messages);

    /* {  //representation of the json data that we have created above:
     *   "model": "gpt-5",
     *   "messages": [
     *     {
     *       "role": "user",
     *       "content": "Hello!"
     *     }
     *   ]
     * }'
     */
    // Convert JSON string for request
    char *json_data = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    if(!json_data) return NULL;

    // Initialize Libcurl and preparing HTTP requst
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL; 
    struct memory chunk = {0}; // Initialize response buffer

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (!curl) {
      free(json_data);
      return NULL;
    }
    /* Set up HTTP headers
     * -H "Content-Type: application/json"
     * -H "Authorization: Bearer $OPENAI_API_KEY"
     */
    char auth_header[512];
    snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", api_key);
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, auth_header);

    // Setup libcurl options
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    // perform request
    res = curl_easy_perform(curl);

    // request data cleanup
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    free(json_data);

    // Check if HTTP request was successful
    if (res != CURLE_OK) {
      free(chunk.response);
      return NULL;
    }
    // parse the JSON response from the API
    cJSON *resp_json = cJSON_Parse(chunk.response);
    if (!resp_json) {
      free(chunk.response);
      return NULL;
    }
    // Check for API errors
    cJSON *error_obj = cJSON_GetObjectItem(resp_json, "error");
    if (error_obj) {
      cJSON *error_message = cJSON_GetObjectItem(error_obj, "message");

      if (error_message && cJSON_IsString(error_message)) {
        fprintf(stderr, "API Error: %s\n", error_message->valuestring);
      } else {
        fprintf(stderr, "API Returned an error, but no message\n");
      }

      cJSON_Delete(resp_json);
      free(chunk.response);
      return NULL;
    }

    /* {
     *   "id": "chatcmpl-CtZfFxdb31S8Mb6GYWpXjwB438CWq",
     *   "object": "chat.completion",
     *   "created": 1767361169,
     *   "model": "gpt-5-2025-08-07",
     *   "choices": [
     *     {
     *       "index": 0,
     *       "message": {
     *         "role": "assistant",
     *         "content": "Paris.",
     *         "refusal": null,
     *         "annotations": []
     *       },
     *       "finish_reason": "stop"
     *     }
     *   ]
     * } */
    // Extract response content from the JSON structure
    cJSON *choices = cJSON_GetObjectItem(resp_json, "choices");
    if (!cJSON_IsArray(choices) || cJSON_GetArraySize(choices) == 0) {
      cJSON_Delete(resp_json);
      free(chunk.response);
      return NULL;
    }
    // Get the first Choice and extract the message content
    cJSON *first_choice = cJSON_GetArrayItem(choices, 0);
    cJSON *message = cJSON_GetObjectItem(first_choice, "message");
    cJSON *content = cJSON_GetObjectItem(message, "content");

    // Validate the content is a string
    if (!cJSON_IsString(content)) {
      cJSON_Delete(resp_json);
      free(chunk.response);
      return NULL;
    }
    // printf("ChatGPT says: %s\n\n", content->valuestring);
    
    // Create a copy of the string
    char *result = strdup(content->valuestring);
    // Free reaminging resources
    cJSON_Delete(resp_json);
    free(chunk.response);

    return result;
}
