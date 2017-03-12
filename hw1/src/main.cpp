#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <arpa/inet.h>

#define DEFAULT     3
#define TCP         1
#define UDP         2
#define TCP_UDP     3

#define BUFFER_SIZE 65536
using namespace std;

int flag = DEFAULT;

const char* short_opt = "tu";
const struct option long_opt[] = {
    {"tcp", 0, NULL, 't'},
    {"udp", 0, NULL, 'u'}
};

char buffer[BUFFER_SIZE];


void parse_options(int argc, char** argv);
void parse_net(FILE* target_file, const char* type);
string parse_address_and_port(char* token);



void parse_options(int argc, char** argv){
  
  char c;
  while ( (c = getopt_long(argc, argv, short_opt, long_opt, NULL)) != -1){
    switch(c)
    {
      case 't':
        flag = TCP;
        break;
      case 'u':
        flag ^= UDP; 
        break;
      default:
         fprintf(stderr, "Unknown argument!\n");
         fprintf(stderr, "./hw1 [--t|--tcp] [-u|--udp] [filter-string]\n");
         break;
    }
  }

}

void parse_net(FILE* target_file, const char* type){
  char buffer[256];
  fgets(buffer, 256, target_file);

  while (fgets(buffer, 256, target_file) != NULL){
    
    string data = string(type) + '\t';
    
    char *token;

    token = strtok(buffer, " ");
    token = strtok(NULL, " ");
    data += parse_address_and_port(token) + "\t";

    token = strtok(NULL, " ");
    data += parse_address_and_port(token) + "\t";

    printf("%s\n", data.c_str());
 
  }

}

string parse_address_and_port(char* token){
  unsigned port_i;
  char ip[256], port[32];
  sscanf(token, "%32[0-9A-F]:%X", ip, &port_i);

  if (strlen(ip) > 8){
    struct in6_addr addr;
    sscanf(ip, "%08X%08X%08X%08X", &addr.s6_addr32[0], &addr.s6_addr32[1], &addr.s6_addr32[2], &addr.s6_addr32[3]);
    inet_ntop(AF_INET6, &addr, ip, sizeof(ip));
  }
  else{
    struct in_addr addr;
    sscanf(ip, "%X", &addr);
    inet_ntop(AF_INET, &addr, ip, sizeof(ip));
  }
  if (port_i != 0)
    sprintf(port, "%d", port_i);  
  else
    sprintf(port, "%c", '*');
  return string(ip) + string(":") + string(port);
}

void command_handler(char* type){
  
  printf("List of %s connections:\n", type);
  printf("Proto\tLocal Address\tForeign Address\tPID/Program name and arguments\n");
  
  char ipv4_file[256], ipv6_file[256];

  sprintf(ipv4_file, "/proc/net/%s", type);

  FILE* ipv4 = fopen(ipv4_file , "r");
  if(ipv4 != NULL){
    parse_net(ipv4, type); 
    fclose(ipv4);
  }
  sprintf(ipv6_file, "/proc/net/%s6", type);
  
  FILE* ipv6 = fopen(ipv6_file, "r");
  if(ipv6 != NULL){
    parse_net(ipv6, (string(type)+"6").c_str());
    fclose(ipv6);
  }
}

int main(int argc, char** argv)
{   
  parse_options(argc, argv );
  if (flag & TCP) command_handler("tcp");
  if (flag & UDP) command_handler("udp");
}
