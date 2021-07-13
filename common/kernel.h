#pragma once


class kernel_info_t
{
public:
  kernel_info_t(int tid, int kid, int N, int nB);
  ~kernel_info_t(){};

  int get_tid();
  int get_kid();
  int get_N();
  int get_nB();

private:
  int tid; //Thread ID
  int kid; //Kernel ID
  int N; //Thread Size of Kernel
  //int lws; //local work size specified
  int nBlocks; //number of blocks

  //std::vector<block_info_t *> blocks;
};

#if 0
class block_info_t
{
public:
  block_info_t(int tid, int block_id);
  ~block_info_t();

private:
  int tid;      //thread id from CPU
  int block_id; //Block id from kernel
  int group;    //Assigned group

  void setGroup(int group);

}
#endif
