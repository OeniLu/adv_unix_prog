#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>

#define DEFAULT     0
#define TCP_ONLY    1
#define UDP_ONLY    2
#define TCP_UDP     3


const char short_opt = "tu:";
const struct long_opt[] = {
    {"tcp", 0, NULL, 't'},
    {"udp", 0, NULL, 'u'}
};



int main(int argc, char** argv)
{   
    char c;
    while ( (c = getopt_long(argc, argv, short_opt, long_opt, NULL)) != -1){
        switch(c)
        {
            case 't':
                break;
            case 'u':
                break;
            default:
                fprintf(stderr, "Unknown argument!\n");
                fprintf(stderr, "./hw1 [--t|--tcp] [-u|--udp] [filter-string]\n");
                break;
        }
    }    
}
