// © 2020 Joseph Cameron - All Rights Reserved

#include <stdexcept>
#include <iostream>
#include <string>

#include <jfc/http/curl_request.h>

#include <moody/concurrentqueue.h>

using namespace jfc;

static size_t WriteMemoryCallback(unsigned char *const contentPointer, 
    const size_t contentItemSize, 
    const size_t contentItemCount, 
    void *const userPointer)
{
    const size_t contentByteCount(contentItemSize *contentItemCount);

    auto pResponseBuffer(static_cast<http::request::response_data_type *const>(userPointer));

    pResponseBuffer->insert(pResponseBuffer->end(), contentPointer, contentPointer + contentByteCount);

    return contentByteCount;
}

http::curl_request::curl_request(http::curl_context::worker_task_queue_ptr pWorkerTaskQueue,
    const std::string &aURL,
    const std::string &aUserAgent,
    const std::vector<std::string> &aHeaders,
    const http::request::response_handler_functor &aResponseHandler,
    const http::request::failure_handler_functor &aFailureHandler)
: m_pHandle([]()
    {
        auto handle = curl_easy_init();

        if (!handle) throw std::runtime_error("could not initialize a new curl handle");

        return  handle;
    }()
    , [](CURL *p)
    {
        curl_easy_cleanup(p);
    })
, m_pWorkerTaskQueue(pWorkerTaskQueue)
, m_ResponseHandler(aResponseHandler)
, m_FailureHandler(aFailureHandler)
{
    curl_easy_setopt(m_pHandle.get(), CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

    curl_easy_setopt(m_pHandle.get(), CURLOPT_URL, aURL.c_str());

    curl_easy_setopt(m_pHandle.get(), CURLOPT_USERAGENT, aUserAgent.c_str());
    
    if (!aHeaders.empty())
    {
        m_pHeaders = {[&]()
        {
            curl_slist *headerlist = curl_slist_append(nullptr, aHeaders[0].c_str());

            if (aHeaders.size() > 1) for (size_t i(1); i < aHeaders.size(); ++i)
                headerlist = curl_slist_append(headerlist, aHeaders[i].c_str());

            curl_easy_setopt(m_pHandle.get(), CURLOPT_HTTPHEADER, headerlist);

            return headerlist;
        }()
        , [](curl_slist *p)
        {
            curl_slist_free_all(p);
        }};
    }

    //TODO: provide param for timeout
    //curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 10L);
}

void http::curl_request::fetch() 
{
    m_pWorkerTaskQueue->enqueue([&]()
    {
        m_bResponseLocked.test_and_set();
        
        m_ResponseBody.clear();

        curl_easy_setopt(m_pHandle.get(), CURLOPT_WRITEDATA, reinterpret_cast<void *>(&m_ResponseBody)); 
        
        if (const auto error = curl_easy_perform(m_pHandle.get()))
        {
            //TODO: be more precise: switch or map from curl codes to enum class values
            m_RequestError = error::unhandled_error;
        }
        else m_RequestError = error::none;
        
        m_bResponseLocked.clear();
    });
}

void http::curl_request::main_update()
{
    if (!m_bResponseLocked.test_and_set())
    {
        if (m_RequestError == error::none) m_ResponseHandler(m_ResponseBody);
        else m_FailureHandler(m_RequestError);
    }
}

void http::curl_request::worker_update()
{
    if (curl_context::worker_task_type task; m_pWorkerTaskQueue->try_dequeue(task)) task();
}
