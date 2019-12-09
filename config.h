/*****************************************************************************
​ ​* ​ ​ @file​ ​  		config.h
​ * ​ ​ @brief​ ​ 		Contains variables common to both types of locks
​ * ​ ​ @Author(s)​  	​​Devansh Mittal
​ * ​ ​ @Date​ ​​ 		December 6th, 2019
​ * ​ ​ @version​ ​ 		1.0
*****************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

#include <pthread.h>
#include <atomic>

using namespace std;

extern atomic<int> duplicate_count; //Number of duplicate keys during put
extern atomic<int> get_success_count; //Number of successful get operations
extern atomic<int> get_fail_count; //Number of failed get operations
extern pthread_mutex_t duplicate_count_lock;
extern pthread_mutex_t get_success_count_lock;
extern uint8_t isPrint;

void help();

#endif /* CONFIG_H */