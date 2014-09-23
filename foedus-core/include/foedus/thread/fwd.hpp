/*
 * Copyright (c) 2014, Hewlett-Packard Development Company, LP.
 * The license and distribution terms for this file are placed in LICENSE.txt.
 */
#ifndef FOEDUS_THREAD_FWD_HPP_
#define FOEDUS_THREAD_FWD_HPP_
#include "foedus/thread/thread_id.hpp"
/**
 * @file foedus/thread/fwd.hpp
 * @brief Forward declarations of classes in thread package.
 * @ingroup THREAD
 */
namespace foedus {
namespace thread {
struct  ImpersonateSession;
class   Rendezvous;
class   StoppableThread;
class   Thread;
struct  ThreadControlBlock;
class   ThreadGroup;
class   ThreadGroupRef;
struct  ThreadOptions;
class   ThreadPimpl;
class   ThreadPool;
class   ThreadPoolPimpl;
class   ThreadRef;
}  // namespace thread
}  // namespace foedus
#endif  // FOEDUS_THREAD_FWD_HPP_
