// the lock server implementation

#include "lock_server.h"
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

lock_server::lock_server():
  nacquire (0),
  lock(false)
{
	pthread_mutex_init(&lock_mutex, NULL);
	pthread_cond_init(&lock_available, NULL);
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
	pthread_mutex_lock(&lock_mutex);
	while (lock) {
		pthread_cond_wait(&lock_available, &lock_mutex);
	}
	lock = true;
	printf("clt %d locked\n", clt);
	pthread_mutex_unlock(&lock_mutex);
	return lock_protocol::OK;
}

lock_protocol::status
lock_server::release(int clt, lock_protocol::lockid_t lid, int &r)
{
	pthread_mutex_lock(&lock_mutex);
	lock = false;
	printf("clt %d unlocked\n", clt);
	pthread_cond_signal(&lock_available);
	pthread_mutex_unlock(&lock_mutex);
	return lock_protocol::OK;
}


