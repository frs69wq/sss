/* Copyright (c) 2014-2016. The SimGrid Team. All rights reserved.          */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#include "simgrid/msg.h"

#define INMEGA (1024*1024)

XBT_LOG_NEW_DEFAULT_CATEGORY(remote_io, "Messages specific for this io example");


int create_copy();
static int host(int argc, char *argv[]){
  msg_file_t file;

  if (argc > 2){

      char** data = xbt_new(char*, 3);
      data[0] = argv[1];
      data[1] = argv[2];
      data[2] = argv[3];

      MSG_process_create("copy1", create_copy, data, MSG_host_self());
      MSG_process_create("copy2", create_copy, data, MSG_host_self());
      MSG_process_create("copy3", create_copy, data, MSG_host_self());
  }

  return 0;
}

int create_copy(){
    char** argv = MSG_process_get_data(MSG_process_self());
    msg_file_t file = MSG_file_open(argv[0], NULL);
    const char* filename = MSG_file_get_name(file);
    msg_host_t src, dest;
    src= MSG_host_self();
    dest = MSG_host_by_name(argv[1]);

    XBT_INFO("Copy '%s' (of size %llu) from '%s' to '%s'", filename, MSG_file_get_size(file), MSG_host_get_name(src),
             argv[1]);
    MSG_file_rcopy(file, dest, argv[2]);
    MSG_file_close(file);
    return 0;
}

int main(int argc, char **argv)
{
  unsigned int cur;
  msg_storage_t st;

  MSG_init(&argc, argv);
  MSG_create_environment(argv[1]);
  MSG_function_register("host", host);
  MSG_launch_application(argv[2]);

  xbt_dynar_t storages = MSG_storages_as_dynar();
  xbt_dynar_foreach(storages, cur, st){
    XBT_INFO("Init: %llu MiB used on '%s'", MSG_storage_get_used_size(st)/INMEGA, MSG_storage_get_name(st));
  }

  int res = MSG_main();

  xbt_dynar_foreach(storages, cur, st){
    XBT_INFO("Init: %llu MiB used on '%s'", MSG_storage_get_used_size(st)/INMEGA, MSG_storage_get_name(st));
  }
  xbt_dynar_free_container(&storages);

  XBT_INFO("Simulation time %g", MSG_get_clock());
  return res != MSG_OK;
}
