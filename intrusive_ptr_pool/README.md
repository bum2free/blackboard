# Description
Design a prototype of intra-process blackboard data-sharing mechanism. It supports multiple publisher/subscribers, while the subscribers only interest on the latest data, and "polls" the data at their own interval.

## Feature
- no any system heap allocation/free(refer to pre-request)
- read/write is done simultaneously

## pre-request
- acutual data is allocated externally, e.g.: by 3rd memory pool; no system heap allocation/free can be assured if 3rd memory pool can ensure it when actual data value change
- boost::intrusive_ptr requires embedding std::atomic<int> refcount to actual data struct
- to support multiple read/write done simultaneously, the number of pre-allocated data should be NUM_OF_PUB + NUM_OF_SUB + 1

# Implementation 1
atomic_intrusive_ptr_pool_1.h:
- construct by an array of pre-allocated data raw pointer
- lock is implemented by spinlock of std::atomic_flag, as actual lock time is very low(only pointer update,etc)
- std::atomic<int> refcount needs to be embedded in every data struct

test_atomic_intrusive_ptr_pool_1.cpp:
- (stress)test sample on multiple read/write

variant_blackboard_1.h:
- construct a key-value blackboard storage class using std::variant

test_variant_blackboard_1.cpp:
- (stress)test sample on multiple blackboard key-value read/write

# Implementation 2
atomic_intrusive_ptr_pool_2.h:
- this makes wrapper of actual data, which eliminates refcount needs to be embedded in every data struct
- then this introduces can not deference the pointer, which needs to call the get() method to get the actual data pointer

test_atomic_intrusive_ptr_pool_2.cpp:
- (stress)test sample on multiple read/write
