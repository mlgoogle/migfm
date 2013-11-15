#ifndef _PASRSER_XML_H__
#define _PASRSER_XML_H__
#include<stdio.h>
#include<stdlib.h>

struct server;

int config_read(struct server *srv);

int get_plugin_info(struct server *srv);

#endif
