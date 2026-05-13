//
// Created by coldmint on 2026/5/13.
//

#ifndef GLIMMERWORKS_FILEWORKER_H
#define GLIMMERWORKS_FILEWORKER_H
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

#include "FileReadRequest.h"
#include "FileWriteRequest.h"

namespace glimmer {
    class FileWorker {
        std::jthread thread_;
        //One lock protects two queues + thread safety
        //一个锁 保护 两个队列 + 线程安全
        std::mutex fileResultMtx_;
        //Variable: When the queue is empty, the thread goes into sleep mode; when there is a task, it wakes up.
        //件变量：队列空时线程休眠，有任务时唤醒
        std::condition_variable cv_;
        std::queue<FileReadRequest> taskReadQueue_;
        std::queue<FileWriteRequest> taskWriteQueue_;

        void WorkLoop(std::stop_token stopToken);

    public:
        FileWorker();

        void SubmitRead(FileReadRequest readRequest);

        void SubmitWrite(FileWriteRequest writeRequest);
    };
}

#endif //GLIMMERWORKS_FILEWORKER_H
