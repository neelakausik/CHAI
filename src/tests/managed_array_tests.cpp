#include "gtest/gtest.h"

#define CUDA_TEST(X, Y) \
static void cuda_test_ ## X ## Y();\
TEST(X,Y) { cuda_test_ ## X ## Y();}\
static void cuda_test_ ## X ## Y()

#include "chai/ManagedArray.hpp"

#include "../util/forall.hpp"

TEST(ManagedArray, DefaultConstructor) {
  chai::ManagedArray<float> array;
  ASSERT_EQ(array.size(), 0);
}

TEST(ManagedArray, SizeConstructor) {
  chai::ManagedArray<float> array(10);
  ASSERT_EQ(array.size(), 10);
  array.free();
}

TEST(ManagedArray, SpaceConstructorCPU) {
  chai::ManagedArray<float> array(10, chai::CPU);
  ASSERT_EQ(array.size(), 10);
  array.free();
}

#if defined(ENABLE_CUDA)
TEST(ManagedArray, SpaceConstructorGPU) {
  chai::ManagedArray<float> array(10, chai::GPU);
  ASSERT_EQ(array.size(), 10);
  array.free();
}
#endif

TEST(ManagedArray, SetOnHost) {
  chai::ManagedArray<float> array(10);

  forall(sequential(), 0, 10, [=] (int i) {
      array[i] = i;
  });

  forall(sequential(), 0, 10, [=] (int i) {
    ASSERT_EQ(array[i], i);
  });

  array.free();
}

TEST(ManagedArray, Const) {
  chai::ManagedArray<float> array(10);

  forall(sequential(), 0, 10, [=] (int i) {
      array[i] = i;
  });

  chai::ManagedArray<const float> array_const(array);
  chai::ManagedArray<const float> array_const2 = array;

  forall(sequential(), 0, 10, [=] (int i) {
      ASSERT_EQ(array_const[i], i);
  });
}

#if defined(ENABLE_CUDA)
CUDA_TEST(ManagedArray, SetOnDevice) {
  chai::ManagedArray<float> array(10);

  forall(cuda(), 0, 10, [=] __device__ (int i) {
      array[i] = i;
  });

  forall(sequential(), 0, 10, [=] (int i) {
    ASSERT_EQ(array[i], i);
  });

  array.free();
}

CUDA_TEST(ManagedArray, GetGpuOnHost) {
  chai::ManagedArray<float> array(10, chai::GPU);

  forall(cuda(), 0, 10, [=] __device__ (int i) {
      array[i] = i;
  });

  forall(sequential(), 0, 10, [=] (int i) {
    ASSERT_EQ(array[i], i);
  });

  array.free();
}
#endif
