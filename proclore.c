#include "headers.h"

void proclore(char* pid, int ap, int w) {
    char* path_stat = (char*) calloc(256, sizeof(char));
    sprintf(path_stat, "/proc/%d/stat", atoi(pid));

    char* path_maps = (char*) calloc(256, sizeof(char));
    sprintf(path_maps, "/proc/%d/maps", atoi(pid));

    char* path_exe = (char*) calloc(256, sizeof(char));
    sprintf(path_exe, "/proc/%d/exe", atoi(pid));

    char* status;
    char* tty_nr;
    char* process_group;
    unsigned long virtual_address;
    char executable_path[MAX_LEN];

    FILE *fptr = fopen(path_stat, "r");
    if (fptr == NULL) {
        if (ap == 0 && w == 0) {
            char buff[MAX_LEN] = {0};
            sprintf(buff, "\033[1;31mNo such process with process id %s running\033[1;0m\n", pid);
            bprintf(global_buffer, buff);
        } else {
            char buff[MAX_LEN] = {0};
            sprintf(buff, "No such process with process id %s running\n", pid);
            bprintf(global_buffer, buff);
        }
    } else {
        char data[100000];
        fscanf(fptr, " %[^\n]", data);
        char** data_array = generate_tokens(data, ' ');
        status = data_array[2];
        tty_nr = data_array[6];
        bg_process = 0;
        if (strcmp(tty_nr, "0") == 0) {
            bg_process = 1;
        }
        process_group = data_array[4];
        fclose(fptr);
        free_tokens(data_array);
    }

    FILE* fptr2 = fopen(path_maps, "r");
    if (fptr2 != NULL) {
        char buffer[50000];
        unsigned long start;
        unsigned long end;
        fgets(buffer, 50000, fptr2);
        sscanf(buffer, "%lx-%lx", &start, &end);
        virtual_address = start;
    } else {
        if (ap == 0 && w == 0) {
            bprintf(global_buffer, "\033[1;31mproclore: cannot open /proc/pid/maps\033[1;0m\n");
        } else {
            bprintf(global_buffer, "proclore: cannot open /proc/pid/maps\n");
        }
    }

    char buff[MAX_LEN] = {0};
    sprintf(buff, "pid : %s\n", pid);
    bprintf(global_buffer, buff);

    char buff2[MAX_LEN] = {0};
    sprintf(buff2, "process status : %s", status);
    bprintf(global_buffer, buff2);

    int background_process = 0;
    LL_Node trav = LL->first;
    while (trav != NULL) {
        if (trav->pid == atoi(pid)) {
            background_process = 1;
            break;
        }
        trav = trav->next;
    }
    if (background_process == 1) {
        bprintf(global_buffer, "\n");
    } else {
        bprintf(global_buffer, "+\n");
    }
    char buff3[MAX_LEN] = {0};
    sprintf(buff3, "Process group : %s\n", process_group);
    bprintf(global_buffer, buff3);

    char buff4[MAX_LEN] = {0};
    sprintf(buff4, "Virtual memory : %lu\n", virtual_address);
    bprintf(global_buffer, buff4);
    
    ssize_t length = readlink(path_exe, executable_path, sizeof(executable_path) - 1);
    executable_path[length] = '\0';

    char buff5[MAX_LEN] = {0};
    sprintf(buff5, "executable path : %s\n", executable_path);
    bprintf(global_buffer, buff5);

    free(path_stat);
    free(path_maps);
    free(path_exe);
}