#include "kernel.h"
#include <math.h>

kernel_info_t::kernel_info_t(int tid, int kid, int N, int nB):
tid(tid),
kid(kid),
N(N),
nBlocks(nB)
{
}

int kernel_info_t::get_tid()
{
  return tid;
}

int kernel_info_t::get_kid()
{
  return kid;
}

int kernel_info_t::get_N()
{
  return N;
}

int kernel_info_t::get_nB()
{
  return nBlocks;
}

#if 0
block_info_t::block_info_t(int tid, int block_id):
tid(tid),
block_id(block_id)
{
}

void block_info_t::setGroup(int group)
{

  group = group;

}
#endif
