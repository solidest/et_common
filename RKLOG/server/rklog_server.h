/*
** EPITECH PROJECT, 2018
** rklog
** File description:
** 
*/

#ifndef RKLOG_H_
#define RKLOG_H_

#define rklog(format, args...) printf("RKLOG> "); printf(format, ##args); printf("\n");

int rklog_start(const char* serverip, int serverport, int connpoolsize);


#endif /* !RKLOG_H_ */
