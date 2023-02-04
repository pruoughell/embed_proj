/*
 * TCP Client Base class, version 2.0.3
 *
 * Created January 21, 2023
 *
 * The MIT License (MIT)
 * Copyright (c) 2023 K. Suwatchai (Mobizt)
 *
 *
 * Permission is hereby granted, free of charge, to any person returning a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef TCP_CLIENT_BASE_H
#define TCP_CLIENT_BASE_H

#include <Arduino.h>
#include <sys/time.h>
#include "ESP_Mail_Const.h"
#include <IPAddress.h>
#include <Client.h>
#include "./extras/MB_Time.h"

#define TCP_CLIENT_DEFAULT_TCP_TIMEOUT_SEC 30

typedef enum
{
    esp_mail_cert_type_undefined = -1,
    esp_mail_cert_type_none = 0,
    esp_mail_cert_type_data,
    esp_mail_cert_type_file,
    esp_mail_cert_type_bundle

} esp_mail_cert_type;

typedef enum
{
    esp_mail_client_type_undefined,
    esp_mail_client_type_internal,
    esp_mail_client_type_custom

} esp_mail_client_type;

typedef enum
{
    esp_mail_external_client_type_none,
    esp_mail_external_client_type_basic,
    esp_mail_external_client_type_ssl
} esp_mail_external_client_type;

class TCP_Client_Base
{
    friend class ESP_Mail_Client;
    friend class ESP32_WCS;

public:
    TCP_Client_Base()
    {
        certType = esp_mail_cert_type_undefined;
    };
    virtual ~TCP_Client_Base(){};

    virtual void ethDNSWorkAround(){};

    virtual bool networkReady() { return false; }

    virtual void networkReconnect(){};

    virtual void disconnect(){};

    virtual String fwVersion()
    {
        return String();
    }

    virtual esp_mail_client_type type() { return esp_mail_client_type_undefined; }

    virtual bool isInitialized() { return false; }

    virtual int hostByName(const char *name, IPAddress &ip) { return 0; }

    virtual bool connect(bool secured, bool verify) { return false; }

    virtual bool connectSSL(bool verify) { return false; }

    virtual void stop(){};

    virtual bool connected() { return false; }

    virtual int write(uint8_t *data, int len) { return 0; }

    virtual int send(const char *data) { return 0; }

    virtual int print(const char *data) { return 0; }

    virtual int print(int data) { return 0; }

    virtual int println(const char *data) { return 0; }

    virtual int println(int data) { return 0; }

    virtual int available() { return 0; }

    virtual int read() { return 0; }

    virtual int readBytes(uint8_t *buf, int len) { return 0; }

    virtual int readBytes(char *buf, int len) { return 0; }

    virtual void flush() {}

    virtual void setDebugLevel(int debug) { debugLevel = debug; }

    void baseSetCertType(esp_mail_cert_type type) { certType = type; }

    void baseSetTimeout(uint32_t timeoutSec) { tmo = timeoutSec * 1000; }

    esp_mail_cert_type getCertType() { return certType; }

    int getProtocol(uint16_t port)
    {
        if (session)
        {
            if (session->ports_functions.list)
            {
                for (int i = 0; i < session->ports_functions.size; i++)
                {
                    if (session->ports_functions.list[i].port == port)
                        return (int)session->ports_functions.list[i].protocol;
                }
            }
        }

        return -1;
    }

    esp_mail_external_client_type extClientType()
    {
        return ext_client_type;
    }

private:
    void setMBFS(MB_FS *mbfs) { this->mbfs = mbfs; }
#if defined(ENABLE_IMAP) || defined(ENABLE_SMTP)
    void setSession(ESP_Mail_Session *session)
    {
        this->session = session;
    }
#endif
    int tcpTimeout()
    {
        return tmo;
    }

protected:
    MB_FS *mbfs = nullptr;
    int chunkSize = 4096;
    int tmo = 40000; // 40 sec
    bool clockReady = false;
    time_t now = 0;
    bool tls_required = false;
    bool tls_error = false;
    bool debugLevel = 0;
    esp_mail_cert_type certType = esp_mail_cert_type_undefined;
    esp_mail_external_client_type ext_client_type = esp_mail_external_client_type_basic;
#if defined(ENABLE_IMAP) || defined(ENABLE_SMTP)
    ESP_Mail_Session *session = nullptr;
#endif
};

#endif
