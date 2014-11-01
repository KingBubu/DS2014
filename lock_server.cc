// the lock server implementation

#include "lock_server.h"
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

lock_server::lock_server():
  nacquire (0)
{
	pthread_mutex_init(&locks_mutex, NULL);
	pthread_cond_init(&locks_available, NULL);
}

lock_protocol::status
lock_server::stat(int clt, lock_protocol::lockid_t lid, int &r)
{
  lock_protocol::status ret = lock_protocol::OK;
  printf("stat request from clt %d\n", clt);
  r = nacquire;
  return ret;
}

lock_protocol::status
lock_server::acquire(int clt, lock_protocol::lockid_t lid, int &r)
{
	pthread_mutex_lock(&locks_mutex);
	if (locks.find(lid) == locks.end()) {
		locks[lid] = false;
	}
	while (locks[lid]) {
		pthread_cond_wait(&locks_available, &locks_mutex);
	}
	locks[lid] = true;
	//printf("clt %d acquires lock %lld\n", clt, lid);
	pthread_mutex_unlock(&locks_mutex);
	return lock_protocol::OK;
}

lock_protocol::status
lock_server::release(int clt, lock_protocol::lockid_t lid, int &r)
{
	pthread_mutex_lock(&locks_mutex);
	locks[lid] = false;
	//printf("clt %d releases lock %lld\n", clt, lid);
	pthread_cond_signal(&locks_available);
	pthread_mutex_unlock(&locks_mutex);
	return lock_protocol::OK;
}


