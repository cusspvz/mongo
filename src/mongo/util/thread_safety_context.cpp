/**
 *    Copyright (C) 2020-present MongoDB, Inc.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the Server Side Public License, version 1,
 *    as published by MongoDB, Inc.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    Server Side Public License for more details.
 *
 *    You should have received a copy of the Server Side Public License
 *    along with this program. If not, see
 *    <http://www.mongodb.com/licensing/server-side-public-license>.
 *
 *    As a special exception, the copyright holders give permission to link the
 *    code of portions of this program with the OpenSSL library under certain
 *    conditions as described in each individual source file and distribute
 *    linked combinations including the program with the OpenSSL library. You
 *    must comply with the Server Side Public License in all respects for
 *    all of the code used other than as permitted herein. If you modify file(s)
 *    with this exception, you may extend this exception to your version of the
 *    file(s), but you are not obligated to do so. If you do not wish to do so,
 *    delete this exception statement from your version. If you delete this
 *    exception statement from all source files in the program, then also delete
 *    it in the license file.
 */

#include "mongo/util/thread_safety_context.h"

#include <boost/preprocessor/control/iif.hpp>

#include "mongo/platform/compiler.h"
#include "mongo/util/assert_util.h"

namespace mongo {

ThreadSafetyContext* ThreadSafetyContext::getThreadSafetyContext() noexcept {
    static auto safetyContext = new ThreadSafetyContext();  // Intentionally leaked
    return safetyContext;
}

void ThreadSafetyContext::forbidMultiThreading() noexcept {
    invariant(_isSingleThreaded.load());
    invariant(_safeToCreateThreads.swap(false));
}

void ThreadSafetyContext::allowMultiThreading() noexcept {
    invariant(_isSingleThreaded.load());
    invariant(!_safeToCreateThreads.swap(true));
}

void ThreadSafetyContext::onThreadCreate() noexcept {
    invariant(_safeToCreateThreads.load());
    if (MONGO_unlikely(_isSingleThreaded.load())) {
        _isSingleThreaded.store(false);
    }
}

}  // namespace mongo
