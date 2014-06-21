/*
 * Copyright (c) 2014, Hewlett-Packard Development Company, LP.
 * The license and distribution terms for this file are placed in LICENSE.txt.
 */
#include "foedus/initializable.hpp"

#include <glog/logging.h>

#include <cstdlib>
#include <iostream>
#include <typeinfo>

#include "foedus/assert_nd.hpp"

namespace foedus {
UninitializeGuard::~UninitializeGuard() {
  if (target_->is_initialized()) {
    if (policy_ != kSilent) {
      LOG(ERROR) << "UninitializeGuard has found that " << typeid(*target_).name()
        <<  "#uninitialize() was not called when it was destructed. This is a BUG!"
        << " We must call uninitialize() before destructors!";
      print_backtrace();
    }
    if (policy_ == kAbortIfNotExplicitlyUninitialized) {
      LOG(FATAL) << "FATAL: According to kAbortIfNotExplicitlyUninitialized policy,"
        << " we abort the program" << std::endl;
      ASSERT_ND(false);
      std::abort();
    } else {
      // okay, call uninitialize().
      ErrorStack error = target_->uninitialize();
      // Note that this is AFTER uninitialize(). "target_" might be Engine
      // or DebuggingSupports. So, we might not be able to use glog any more.
      // Thus, we must use stderr in this case.
      if (error.is_error()) {
        switch (policy_) {
        case kAbortIfUninitializeError:
          std::cerr << "FATAL: UninitializeGuard encounters an error on uninitialize()."
            << " Aborting as we can't propagate this error appropriately."
            << " error=" << error << std::endl;
          ASSERT_ND(false);
          std::abort();
          break;
        case kWarnIfUninitializeError:
          std::cerr << "WARN: UninitializeGuard encounters an error on uninitialize()."
            << " We can't propagate this error appropriately. Not cool!"
            << " error=" << error << std::endl;
          break;
        default:
          // warns nothing. this policy is NOT recommended
          ASSERT_ND(policy_ == kSilent);
        }
      } else {
        if (policy_ != kSilent) {
          std::cerr << "But, fortunately uninitialize() didn't return errors, phew"
            << std::endl;
        }
      }
    }
  }
}
}  // namespace foedus

