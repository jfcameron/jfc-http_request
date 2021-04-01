// © Joseph Cameron - All Rights Reserved

#include <jfc/http/curl_post.h>

using namespace jfc;

http::curl_post::curl_post(std::weak_ptr<http::curl_context> pContext,
    const std::string &aURL,
    const std::string &aUserAgent,
    const size_t aTimeoutMiliseconds,
    const std::vector<std::string> &aHeaders,
    const std::string &aPostData,
    const http::request::response_handler_functor &aResponseHandler,
    const http::request::failure_handler_functor &aFailureHandler)
: http::curl_request::curl_request(pContext, 
    aURL, 
    aUserAgent, 
    aTimeoutMiliseconds,
    aHeaders, 
    aResponseHandler, 
    aFailureHandler)
{}

void http::curl_post::on_enqueue_extra_worker_configuration(CURL *const pCURL)
{
    curl_easy_setopt(pCURL, CURLOPT_POSTFIELDS, m_PostData);
}

void http::curl_post::update_postdata(const std::string &aPostData)
{
    m_PostData = aPostData;
}

bool http::curl_post::try_enqueue()
{
    return curl_request::try_enqueue();
}

