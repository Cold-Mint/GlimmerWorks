//
// Created by coldmint on 2026/5/13.
//

#include "FileWorker.h"

void glimmer::FileWorker::WorkLoop(std::stop_token stopToken) {
    while (!stopToken.stop_requested()) {
        std::unique_lock lock(fileResultMtx_);
        //在执行wait的时候，锁会被自动解开。
        cv_.wait(lock, [this, &stopToken] {
            return !taskReadQueue_.empty()
                   || !taskWriteQueue_.empty()
                   || stopToken.stop_requested();
        });

        if (stopToken.stop_requested()) {
            break;
        }

        if (!taskReadQueue_.empty()) {
            taskReadQueue_.pop();
            lock.unlock();

            lock.lock();
            continue;
        }

        if (!taskWriteQueue_.empty()) {
            taskWriteQueue_.pop();
            lock.unlock();
            //处理
            lock.lock();
        }
    }
}

glimmer::FileWorker::FileWorker() {
    thread_ = std::jthread(&FileWorker::WorkLoop, this);
}

void glimmer::FileWorker::SubmitRead(const FileReadRequest readRequest) {
    std::lock_guard lock(fileResultMtx_);
    taskReadQueue_.push(readRequest);
    cv_.notify_one();
}

void glimmer::FileWorker::SubmitWrite(const FileWriteRequest writeRequest) {
    std::lock_guard lock(fileResultMtx_);
    taskWriteQueue_.push(writeRequest);
    cv_.notify_one();
}
