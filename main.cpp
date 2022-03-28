#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <curl/curl.h>
#include <numeric>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
static char errorBuffer[CURL_ERROR_SIZE];

std::string buffer;
int writer(char *data, size_t size, size_t nmemb, std::string *buffer)
{
    int result = 0;
    if (buffer != NULL)
    {
        buffer->append(data, size * nmemb);
        result = size * nmemb;
    }
    return result;
}

std::vector<std::vector<double>> a;
std::vector<std::string> name;

int main() {
    CURL *curl;
    CURLcode res;

    bool exit = false;
    while(!exit) {
        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, "https://www.cbr-xml-daily.ru/daily_json.js");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
            curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, &errorBuffer);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
            res = curl_easy_perform(curl);
            if (res == CURLE_OK)
                std::cout << "OK" << std::endl;
            else
                std::cout << res << " " << errorBuffer << std::endl;

            auto j = json::parse(buffer);
            int cnt = 0;
            for (const auto &i: j["Valute"].items()) {
                if (a.size() == cnt){
                    a.push_back({});
                    name.push_back(i.value()["CharCode"]);
                }
                std::cout << i.value()["CharCode"] << " " << i.value()["Value"] << std::endl;
                a[cnt].push_back(i.value()["Value"]);
                cnt += 1;
            }
        }
        curl_easy_cleanup(curl);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        buffer = "";
        if (GetAsyncKeyState(VK_ESCAPE))
        {
            exit = true;
        }
    }

    std::cout << "____________________" << std::endl;
    for (int i = 0; i < a.size(); i ++){
        std::sort(a[i].begin(), a[i].end());
        std::cout << name[i] << " mean: " << std::accumulate(a[i].begin(), a[i].end(), 0.0) / a[i].size() << ", median: " << a[i][a[i].size()/2] << std::endl;
    }

}

