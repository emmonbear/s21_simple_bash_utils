#include "parser_args.h"

data_t parser(int argc, char **argv) {
    data_t data;
    data.opt.b = data.opt.E = data.opt.n = data.opt.s = data.opt.T = data.opt.v = 0;
    data.invalid = VALID;
    data.file_paths = NULL;
    data.num_line = 1;
    data.num_files = 0;
    int opt;

    struct option long_options[] = {
        {"show-all", no_argument, 0, 'A'},
        {"number-nonblank", no_argument, 0, 'b'},
        {"show-ends", no_argument, 0, 'E'},
        {"number", no_argument, 0, 'n'},
        {"squeeze-blank", no_argument, 0, 's'},
        {"show-tabs", no_argument, 0, 'T'},
        {"show-nonprinting", no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };
    
    while((opt = getopt_long(argc, argv, "beEnstTvA", long_options, NULL)) != -1) {
        switch_parser(opt, &data);
    }

    alloc_filepaths(&data, argc, argv);

    return data;
}

void switch_parser(int opt, data_t *data) {
    switch (opt) {
        case 'b':
            data->opt.b = 1;
            break;
        case 'e':
            data->opt.E = 1;
            data->opt.v = 1;
            break;
        case 'E':
            data->opt.E = 1;
            break;
        case 'n':
            data->opt.n = 1;
            break;
        case 's':
            data->opt.s = 1;
            break;
        case 't':
            data->opt.T = 1;
            data->opt.v = 1;
            break;
        case 'T':
            data->opt.T = 1;
            break;
        case 'v':
            data->opt.v = 1;
            break;
        case 'A':
            data->opt.v = 1;
            data->opt.E = 1;
            data->opt.T = 1;
            break;
        case '?':
            data->invalid = UNKNOWN_OPT;
        default:
            //Just printf files.
    }
}

char *write_buffer(data_t *data, int file_count) {
    char* buffer = NULL;
    FILE *file = fopen(data->file_paths[file_count], "r");
    if(!file) {
        fprintf(stderr, "cat: %s: No such file or directory\n", data->file_paths[file_count]);
    }

    if(file) {
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        buffer = (char *)malloc(file_size + 1);
        if(!buffer) {
            data->invalid = BUFFER_ALLOC;
        } 

        if(!data->invalid) {
            size_t bytes_read;
            bytes_read = fread(buffer, 1, file_size, file);
            if((long)bytes_read != file_size) {
                data->invalid = FILE_READ;
            } 
        }
        
        if(!data->invalid) {
            buffer[file_size] = '\0';
        }

        fclose(file);
    }

    return buffer;
}

void alloc_filepaths (data_t *data, int argc, char **argv) {
    for (int index = optind; index < argc && !data->invalid; index++) {
        data->file_paths = realloc(data->file_paths, sizeof(char *) * (data->num_files + 1));
        if (!data->file_paths) {
            data->invalid = FILEPATH_ALLOC;
        }

        if(!data->invalid) {
            data->file_paths[data->num_files] = strdup(argv[index]);
            if (!data->file_paths[data->num_files]) {
                data->invalid = FILEPATH_ALLOC;
            }
        }

        data->num_files++;
    }
}

char* strdup(const char* str) {
    size_t length = strlen(str) + 1;
    char* new_str = malloc(length);
    if (new_str) {
        memcpy(new_str, str, length);
    }
    
    return new_str;
}