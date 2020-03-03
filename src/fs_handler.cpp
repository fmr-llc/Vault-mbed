#include "fs_handler.h"

BlockDevice *bd = BlockDevice::get_default_instance();
LittleFileSystem fs("fs");
struct dirent dir;

FSHandler::FSHandler(){
    fflush(stdout);
    int err = fs.mount(bd);
    if(err){
        fflush(stdout);
        fs.reformat(bd);
    }
    fs.mkdir("transactions", S_IRWXU);
}

FILE *FSHandler::open(char* txid, int mode){
    char file_name[64];
    sprintf(file_name, "%s%s%s", "fs/transactions/", txid, ".txt");
    FILE *f;
    if(mode == 0){
        f = fopen(file_name, "r");
    }
    else{
        f = fopen(file_name, "w");

    }

    if(f == NULL){
        //error testing
        //return error
    }

    return f;
}

void FSHandler::close(FILE *f){
    //FIXME
    //somehow mark the txn/file as spent
    //fs.remove(file_name);
    fflush(f);
    fclose(f);

}

int FSHandler::get_size(FILE *f){
    int size = 0;
    fseek(f, 0L, SEEK_END);
    size = ftell(f);
    rewind(f);

    return size;
}


/*
a way to clean up directory
DIR *d = opendir("fs/transactions");

    char file_name[128];
    for (size_t i = 0; i < d->size(); i++){
        d->read(&dir);
        sprintf(file_name, "%s%s%s", "fs/transactions/", dir.d_name, ".txt");
        serial2.printf("%s\n", (char*)file_name);
        //fs.remove(file_name);
    }

    serial2.printf("%s%zu\n", "Size: ", d->size());
    d->close();
*/
