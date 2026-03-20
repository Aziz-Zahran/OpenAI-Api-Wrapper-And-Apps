# OpenAI API Wrapper Library And Apps

A lightweight OpenAI API wrapper written from scratch in C, with a collection of intelligent command-line applications built on top of it.

---

## Project Structure

```
openai-api-wrapper-and-apps/
├── chatgpt.c              # API wrapper implementation
├── chatgpt.h              # API wrapper header
├── cJSON.c                # JSON parsing (cJSON library)
├── cJSON.h
└── Intelligent_Apps/
    ├── starter.c          # Minimal usage example
    ├── Shell.c            # Natural language → shell command generator
    ├── Shell-explainer.c  # Shell command explainer
    ├── forever-story.c    # Interactive AI-driven story game
    ├── mock-data-generator.c  # AI mock data file generator
    └── cli-code-reviewer.c    # AI-powered code review tool
```

---

## The Wrapper — `chatgpt.c`

The core of the project is a hand-rolled C wrapper around the OpenAI Chat Completions API. It uses **libcurl** for HTTP and **cJSON** for JSON parsing, with no other dependencies.

```c
char *chatgpt_query(const char *api_key, const char *prompt);
```

Pass in your API key and a prompt string — get back a heap-allocated string with the model's reply. The caller is responsible for `free()`-ing it.

Internally it handles:

- JSON payload construction
- HTTP POST via libcurl with proper headers
- Response buffering via a curl write callback
- API error detection and reporting
- JSON response parsing and content extraction

---

## Applications

### `starter.c` — Hello World

The minimal usage example. Sends a single hardcoded prompt and prints the response. A good starting point for understanding how the wrapper works.

```bash
./starter
```

---

### `Shell.c` — Natural Language → Shell Commands

Describe a task in plain English, get back the shell command(s) to accomplish it.

```bash
./shell
# Enter the description of the desired shell task:
# > find all files larger than 100MB modified in the last 7 days
```

> ⚠️ Always review AI-generated shell commands before running them.

---

### `Shell-explainer.c` — Shell Command Explainer

Paste any shell command and get a plain-English explanation. Handles simple one-liners briefly, and breaks down complex pipelines step by step. Runs in a loop until you type `quit`.

```bash
./shell-explainer
# Enter the shell commands you want explained or type quit to exit
# > tar -czf archive.tar.gz --exclude='*.log' /var/www
```

---

### `forever-story.c` — Interactive AI Story

An infinite interactive fantasy story. The AI narrates one part at a time, pauses for your input, and continues the story based on your choices. Uses structured JSON responses to track the full story context across turns.

```bash
./forever-story
# Welcome to forever story! The story keeps going forever and ever.
```

Type `quit` to exit.

---

### `mock-data-generator.c` — Mock Data Generator

Generates mock data and writes it to a file. Pass the output filename and a description of the data you want as a **single quoted string**.

```bash
./mock-data-generator output.csv "100 rows of customer data with name, email, country, and purchase amount"
```

> **Note:** The description must be wrapped in quotes. Without quotes, the shell splits on spaces and only the first word is passed as the description.

The output format is determined by your description — ask for CSV, JSON, SQL inserts, or anything else.

---

### `cli-code-reviewer.c` — CLI Code Reviewer

Submits a source file to the AI for a professional code review. Supports two modes:

|Mode|Description|
|---|---|
|`SIMPLE`|High-level scan, obvious/high-impact issues only|
|`DEEP`|Full technical review: control flow, edge cases, memory safety, assumptions|

```bash
./cli-code-reviewer myfile.c DEEP
./cli-code-reviewer myfile.c SIMPLE
```

---

## Prerequisites

- A C compiler (gcc or clang)
- [libcurl](https://curl.se/libcurl/) — pre-installed on macOS and most Linux distros. On Debian/Ubuntu you may need the dev headers for compilation: `sudo apt install libcurl4-openssl-dev`
- An [OpenAI API key](https://platform.openai.com/api-keys)

---

## Building

Each app links against the wrapper and cJSON. Example for `starter`:

```bash
gcc starter.c ../chatgpt.c ../cJSON.c -o starter -lcurl
```

Or write a simple `Makefile` to build all apps at once.

---

## Configuration

Set your OpenAI API key as an environment variable before running any app:

```bash
export OPENAI_API_KEY="sk-..."
```

Alternatively, you can hardcode it directly in any app (a comment in the source marks the spot), but the environment variable approach is strongly recommended.

---

## Dependencies

| Dependency                                   | Purpose       | Included?                                                                                                  |
| -------------------------------------------- | ------------- | ---------------------------------------------------------------------------------------------------------- |
| [libcurl](https://curl.se/libcurl/)          | HTTP requests | No — usually pre-installed. [Click here if not installed.](https://everything.curl.dev/install/index.html) |
| [cJSON](https://github.com/DaveGamble/cJSON) | JSON parsing  | Yes — bundled in repo                                                                                      |

---

## License

See [LICENSE](LICENSE)
