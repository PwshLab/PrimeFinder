#ifndef CHECKCOMS_H
#define CHECKCOMS_H

#include <queue>
#include <vector>
#include <mutex>
//#include <condition_variable>
#include <algorithm>
#include <atomic>

struct CheckJob
{
    size_t lower;
    size_t upper;
    int increment;
    CheckJob(const int l, const int u, const int i)
    {
        lower = l;
        upper = u;
        increment = i;
    }
};

struct CheckComs
{   private:
        std::mutex inputMutex;
        std::mutex outputMutex;
        //std::condition_variable comVariable;
        std::queue<CheckJob> input;
        std::vector<int> output;
        std::atomic<bool> hasJob;
        std::atomic<bool> working;
        std::atomic<bool> hasResult;
        std::atomic<bool> exitRequested;
    public:
        CheckComs()
        {
            hasJob = false;
            working = false;
            hasResult = false;
            exitRequested = false;
        }
        void AddJob(CheckJob newJob)
        {
            std::lock_guard<std::mutex> lock(inputMutex);
            input.emplace(newJob);
            hasJob.store(true, std::memory_order_release);
            StartWorking();
        }
        CheckJob ReceiveJob()
        {
            std::lock_guard<std::mutex> lock(inputMutex);
            CheckJob job = input.front();
            input.pop();
            hasJob = !input.empty();
            return job;
        }
        bool HasJob()
        {
            return hasJob.load(std::memory_order_acquire);
        }
        bool IsWorking()
        {
            return working.load(std::memory_order_acquire);
        }
        void StartWorking()
        {
            working.store(true, std::memory_order_acquire);
        }
        void StopWorking()
        {
            working.store(false, std::memory_order_release);
        }
        void AddResult(int number)
        {
            std::lock_guard<std::mutex> lock(outputMutex);
            output.push_back(number);
            hasResult.store(true, std::memory_order_release);
        }
        void ReceiveResult(std::vector<int>& dest)
        {
            std::lock_guard<std::mutex> lock(outputMutex);
            std::vector<int> newVec(dest.size() + output.size());
            std::merge(dest.begin(), dest.end(), output.begin(), output.end(), newVec.begin());
            std::swap(newVec, dest);
            output.clear();
            hasResult.store(false, std::memory_order_release);
        }
        void ReceiveResultUnsorted(std::vector<int>& dest)
        {
            std::lock_guard<std::mutex> lock(outputMutex);
            dest.reserve(dest.size() + output.size());
            dest.insert(dest.end(), output.begin(), output.end());
            output.clear();
            hasResult.store(false, std::memory_order_release);
        }
        bool HasResult()
        {
            return hasResult.load(std::memory_order_acquire);
        }
        void SetExit()
        {
            exitRequested.store(true, std::memory_order_release);
        }
        bool ShouldExit()
        {
            return exitRequested.load(std::memory_order_acquire);
        }
};

#endif