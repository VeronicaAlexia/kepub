#pragma once

#include <string>
#include <unordered_map>

#include <klib/http.h>

#include "config.h"

namespace kepub {

namespace esjzone {

klib::Response KEPUB_PUBLIC http_get(const std::string &url,
                                     const std::string &proxy);

}  // namespace esjzone

namespace ciweimao {

klib::Response KEPUB_PUBLIC http_get_rss(const std::string &url);

klib::Response KEPUB_PUBLIC http_post(
    const std::string &url, std::unordered_map<std::string, std::string> data);

}  // namespace ciweimao

namespace sfacg {

klib::Response KEPUB_PUBLIC
http_get(const std::string &url,
         const std::unordered_map<std::string, std::string> &params = {});

klib::Response KEPUB_PUBLIC http_get_rss(const std::string &url);

klib::Response KEPUB_PUBLIC http_post(const std::string &url,
                                      const std::string &json);

}  // namespace sfacg

}  // namespace kepub