#ifndef LOGIN_H
#define LOGIN_H

#include "StrEnc.h"
#include "Includes.h"
#include "curl/curl.h"
#include "Tools.h"
#include "json.hpp"
#include "Log.h"
#include <jni.h>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

// لایبرەرییەکانی OpenSSL بۆ RSA و SHA256
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/sha.h>

// دڵنیابەرەوە فایلی obfuscate.h لە هەمان فۆڵدەرە
#include "obfuscate.h" 
#define safe_snprintf snprintf 

using json = nlohmann::ordered_json;
using namespace std;

std::string ts;
bool check;
string g_Auth, g_Token, EXP, real, noticemode, zippassmode, device, credit;
string modname, mod_status, globalstatus, bgmistatus, koreastatus, chinastatus, tiwanstatus, vngstatus, serverstatus, server1, server2;
string cpp1, cpp1_case, cpp2, cpp2_case, cpp3, cpp3_case, cpp4, cpp4_case;
bool xAuth = false, xEnv = false;

// ==========================================
// بەشی شێفەرکردنی RSA و ئامادەکردنی داتا
// ==========================================

// کلیلی گشتی (Public Key) بۆ قوفڵدان و پشکنینی واژۆ
const char* PUBLIC_KEY = 
"-----BEGIN PUBLIC KEY-----\n"
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAisWg0K2qa3VEZcRIzSwR\n"
"Y7ZAN3byE1xorOvwnGf6qBQHqHBf7mwWb3DdawsiXvlNrEcg+xLaZ5kBTjo5FOBg\n"
"SvmLMSkgTJxCYjP2aTNt0jx3pz/MYzOMBctsojNWYOddfJlejspvhFflzPUrgfgM\n"
"OAG62VRbTF4yYgJQV3JE2RvOiH30kGGDyZsLe0SOb2svnficit5CQfj/3lSJi8WN\n"
"yv3Vjfglmye7YDsrijZt1VNHShYBzlugo1AtjRppxo7fvNVNkYXhpMdKN1m9M+rL\n"
"+qN86/zpnEIgiqSAPCWugvssz1fsneYOpLRDIQqGHPaBs9rz3LcWssea46QM1/XT\n"
"vwIDAQAB\n"
"-----END PUBLIC KEY-----\n";

static const string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static inline bool is_base64(unsigned char c) { return (isalnum(c) || (c == '+') || (c == '/')); }

string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
    string ret;
    int i = 0, j = 0;
    unsigned char char_array_3[3], char_array_4[4];

    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            for(i = 0; (i <4) ; i++) ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }
    if (i) {
        for(j = i; j < 3; j++) char_array_3[j] = '\0';
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        for (j = 0; (j < i + 1); j++) ret += base64_chars[char_array_4[j]];
        while((i++ < 3)) ret += '=';
    }
    return ret;
}

string base64_decode(string const& encoded_string) {
    int in_len = encoded_string.size();
    int i = 0, j = 0, in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    string ret;

    while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i ==4) {
            for (i = 0; i <4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
            for (i = 0; (i < 3); i++) ret += char_array_3[i];
            i = 0;
        }
    }
    if (i) {
        for (j = i; j <4; j++) char_array_4[j] = 0;
        for (j = 0; j <4; j++) char_array_4[j] = base64_chars.find(char_array_4[j]);
        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
    }
    return ret;
}

string url_encode(const string &value) {
    ostringstream escaped;
    escaped.fill('0');
    escaped << hex;
    for (string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        string::value_type c = (*i);
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        } else if (c == ' ') {
            escaped << '+';
        } else {
            escaped << '%' << setw(2) << int((unsigned char)c);
        }
    }
    return escaped.str();
}

string EncryptRSA(const string& data) {
    BIO* bio = BIO_new_mem_buf((void*)PUBLIC_KEY, -1);
    RSA* rsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
    if (!rsa) {
        BIO_free(bio);
        return "";
    }

    int rsaLen = RSA_size(rsa);
    unsigned char* encrypted = (unsigned char*)malloc(rsaLen);

    int result = RSA_public_encrypt(data.length(), (const unsigned char*)data.c_str(), encrypted, rsa, RSA_PKCS1_PADDING);
    
    if (result == -1) {
        free(encrypted);
        RSA_free(rsa);
        BIO_free(bio);
        return "";
    }

    string base64_result = base64_encode(encrypted, result);

    free(encrypted);
    RSA_free(rsa);
    BIO_free(bio);
    
    return base64_result;
}

// فەنکشنی پشکنینی واژۆی سێرڤەر (Verify RSA)
bool VerifyRSA(const string& data, const string& signature_base64) {
    string signature = base64_decode(signature_base64);
    if (signature.empty()) return false;

    BIO* bio = BIO_new_mem_buf((void*)PUBLIC_KEY, -1);
    RSA* rsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
    if (!rsa) {
        BIO_free(bio);
        return false;
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)data.c_str(), data.length(), hash);

    int result = RSA_verify(NID_sha256, hash, SHA256_DIGEST_LENGTH, (const unsigned char*)signature.c_str(), signature.length(), rsa);

    RSA_free(rsa);
    BIO_free(bio);

    return result == 1; // ئەگەر 1 بوو واتە واژۆکە ڕاستە و سێرڤەرەکەی خۆت ناردوویەتی
}

// ==========================================
// بەشی cURL و پەیوەندی کردن
// ==========================================

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *) userp;
    char *ptr = (char *) realloc(mem->memory, mem->size + realsize + 1);
    if (ptr == NULL) return 0;
    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

std::string CalcMD5(std::string s) {
    std::string result;
    unsigned char hash[MD5_DIGEST_LENGTH];
    char tmp[4];
    MD5_CTX md5;
    MD5_Init(&md5);
    MD5_Update(&md5, s.c_str(), s.length());
    MD5_Final(hash, &md5);
    for (unsigned char i : hash) {
        sprintf(tmp, "%02x", i);
        result += tmp;
    }
    return result;
}

extern "C" JNIEXPORT jstring JNICALL native_Check(JNIEnv *env, jclass clazz, jobject mContext, jstring mUserKey) {
    const char* user_key = env->GetStringUTFChars(mUserKey, 0);
    
    std::string hwid = user_key;
    hwid += GetAndroidID(env, mContext);
    hwid += GetDeviceModel(env);
    hwid += GetDeviceBrand(env);
    std::string UUID = GetDeviceUniqueIdentifier(env, hwid.c_str());
    
    std::string errMsg;
    struct MemoryStruct chunk{};
    chunk.memory = (char *) malloc(1);
    chunk.size = 0;
    
    CURL *curl = curl_easy_init();
    
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, OBF("POST"));
        curl_easy_setopt(curl, CURLOPT_URL, OBF("https://luffy.murshid-loader.shop/c")); // گۆڕینی بۆ لینکی خۆت لەبیر نەچێت
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, OBF("https"));
        
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, OBF("Accept: application/json"));
        headers = curl_slist_append(headers, OBF("Content-Type: application/x-www-form-urlencoded"));
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        
        json requestData;
        requestData[OBF("game")] = OBF("PUBG");
        requestData[OBF("user_key")] = user_key;
        requestData[OBF("serial")] = UUID;
        
        std::string encrypted_payload = EncryptRSA(requestData.dump());
        
        if(encrypted_payload.empty()) {
            errMsg = OBF("RSA Encryption Failed!");
        } else {
            std::string post_data = "data=" + url_encode(encrypted_payload);
            
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &chunk);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, OBF("AbsoluteX/2.0"));

            CURLcode res = curl_easy_perform(curl);

            if (res == CURLE_OK) {
                try {
                    json result = json::parse(chunk.memory);
                    auto STATUS = std::string{OBF("status")};
                    
                    if (result.contains(STATUS) && result[STATUS] == true) {
                        auto DATA = std::string{OBF("data")};
                        auto SIGN = std::string{OBF("sign")}; // دۆزینەوەی واژۆکە
                        
                        // ئەگەر وەڵامەکە داتا و واژۆی تێدابوو
                        if(result.contains(DATA) && result.contains(SIGN)) {
                            std::string data_string = result[DATA].dump(); // داتاکە وەردەگێڕینەوە بۆ تێکست بۆ پشکنین
                            std::string signature = result[SIGN].get<std::string>();
                            
                            // ئیستا پشکنین دەکەین بزانین واژۆکە ڕاستە یان نا!
                            if (VerifyRSA(data_string, signature)) {
                                
                                // ئەگەر ڕاست بوو ئینجا ڕێگە دەدەین زانیارییەکان بخوێندرێتەوە
                                auto TOKEN = std::string{OBF("token")};
                                auto RNG = std::string{OBF("rng")};
                                
                                EXP = result[DATA].contains(OBF("EXP")) ? result[DATA][OBF("EXP")].get<string>() : "";
                                std::string token = result[DATA][TOKEN].get<std::string>();
                                time_t rng = result[DATA][RNG].get<time_t>();

                                if (rng + 30 > time(0)) {
                                    string auth = OBF("PUBG") + string("-") + user_key + string("-") + UUID + string("-") + OBF("Vm8Lk7Uj2JmsjCPVPVjrLa7zgfx3uz9E"); 
                                    string outputAuth = CalcMD5(auth);
                                    
                                    g_Token = token;
                                    g_Auth = outputAuth;
                                    xAuth = (g_Token == g_Auth);
                                    xEnv = true;
                                }
                            } else {
                                // ئەگەر کەسێک فێڵی لە پەیوەندییەکە کردبێت وەڵامێکی تر بداتەوە
                                errMsg = OBF("Security Error: Fake Server Response Detected!");
                            }
                        } else {
                             errMsg = OBF("Security Error: No Signature Provided!");
                        }
                    } else {
                        auto REASON = string{OBF("reason")};
                        errMsg = result.contains(REASON) ? result[REASON].get<string>() : OBF("Unknown Server Error");
                    }
                } catch (json::exception &e) {
                    errMsg = OBF("JSON Parse Error");
                }
            } else {
                xEnv=false;
                errMsg = curl_easy_strerror(res);
            }
        }
        curl_slist_free_all(headers);
    } else {
        errMsg = OBF("CURL Init Failed");
    }
    
    curl_easy_cleanup(curl);
    free(chunk.memory);
    env->ReleaseStringUTFChars(mUserKey, user_key);
    
    return xAuth ? env->NewStringUTF(OBF("OK")) : env->NewStringUTF(errMsg.c_str());
}

#endif
