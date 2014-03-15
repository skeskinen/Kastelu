#ifndef WORKER_H
#define WORKER_H
#include "app.h"

void worker_start();
void worker_connect(App *app);
void worker_disconnect(App *app);

#endif //WORKER_H
