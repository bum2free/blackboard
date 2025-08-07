# Description
Design a prototype of intra-process blackboard data-sharing mechanism. It supports multiple publisher/subscribers, while the subscribers only interest on the latest data, and "polls" the data at their own interval.

## Feature
- read/write is done simultaneously
- shared_ptr's lifecycle to recycle avaiable slot to the pool

## pre-request
- acutual data is allocated externally
- to support multiple read/write done simultaneously, the number of pre-allocated data should be NUM_OF_PUB + NUM_OF_SUB + 1

# Implementation
atomic_shared_ptr_pool.h:
- construct by an array of pre-allocated data raw pointer
- lock is implemented by spinlock of std::atomic_flag, as actual lock time is very low(only pointer update,etc)

test_atomic_shared_ptr_pool.cpp:
- (stress)test sample on multiple read/write

any_blackboard.h:
- construct a key-value blackboard storage class using std::any

test_any_blackboard.cpp:
- (stress)test sample on multiple blackboard key-value read/write

Note: does it complete eliminate dynamic system heap allocation? No, the usage of shared_ptr's life-cycle would need to dynamically create/destroy shared_ptr itself, which happens do system heap allocation, though it is very small size. Refer to boost::intrusive_ptr instead to improve this.