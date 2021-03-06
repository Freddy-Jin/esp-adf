// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _ESP_HTTP_CLIENT_H
#define _ESP_HTTP_CLIENT_H

#include "freertos/FreeRTOS.h"
#include "http_parser.h"
#include "sdkconfig.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_HTTP_BUF_SIZE (512)

typedef struct esp_http_client *esp_http_client_handle_t;
typedef struct esp_http_client_event *esp_http_client_event_handle_t;

/**
 * @brief   HTTP Client events id
 */
typedef enum {
    HTTP_EVENT_ERROR = 0,       /*!< This event occurs when there are any errors during execution */
    HTTP_EVENT_ON_CONNECTED,    /*!< Once the HTTP has been connected to the server, no data exchange has been performed */
    HTTP_EVENT_HEADER_SENT,     /*!< After sending all the headers to the server */
    HTTP_EVENT_ON_HEADER,       /*!< Occurs when receiving each header sent from the server */
    HTTP_EVENT_ON_DATA,         /*!< Occurs when receiving data from the server, possibly multiple portions of the packet */
    HTTP_EVENT_ON_FINISH,       /*!< Occurs when finish a HTTP session */
    HTTP_EVENT_DISCONNECTED,    /*!< The connection has been disconnected */
} esp_http_client_event_id_t;

/**
 * @brief      HTTP Client events data
 */
typedef struct esp_http_client_event {
    esp_http_client_event_id_t event_id;    /*!< event_id, to know the cause of the event */
    esp_http_client_handle_t client;        /*!< esp_http_client_handle_t context */
    void *data;                             /*!< data of the event */
    int data_len;                           /*!< data length of data */
    void *user_data;                        /*!< user_data context, from esp_http_client_config_t user_data */
    char *header_key;                       /*!< For HTTP_EVENT_ON_HEADER event_id, it's store current http header key */
    char *header_value;                     /*!< For HTTP_EVENT_ON_HEADER event_id, it's store current http header value */
} esp_http_client_event_t;


/**
 * @brief      HTTP Client transport
 */
typedef enum {
    HTTP_TRANSPORT_UNKNOWN = 0x0,   /*!< Unknown */
    HTTP_TRANSPORT_OVER_TCP,        /*!< Transport over tcp */
    HTTP_TRANSPORT_OVER_SSL,        /*!< Transport over ssl */
} esp_http_client_transport_t;

typedef esp_err_t (*http_event_handle_cb)(esp_http_client_event_t *evt);

/**
 * @brief HTTP method
 */
typedef enum {
    HTTP_METHOD_AUTO = 0,   /*!< Auto select method, default is GET, if there are post_data, the method will be POST */
    HTTP_METHOD_GET,        /*!< HTTP GET Method */
    HTTP_METHOD_POST,       /*!< HTTP POST Method */
    HTTP_METHOD_PUT,        /*!< HTTP PUT Method */
    HTTP_METHOD_PATCH,      /*!< HTTP PATCH Method */
    HTTP_METHOD_DELETE,     /*!< HTTP DELETE Method */
    HTTP_METHOD_MAX,
} esp_http_client_method_t;

/**
 * @brief HTTP Authentication type
 */
typedef enum {
    HTTP_AUTH_TYPE_NONE = 0,    /*!< No authention */
    HTTP_AUTH_TYPE_BASIC,       /*!< HTTP Basic authentication */
    HTTP_AUTH_TYPE_DIGEST,      /*!< HTTP Disgest authentication */
} esp_http_client_auth_type_t;

/**
 * @brief HTTP configurations
 */
typedef struct {
    const char                  *uri;                /*!< HTTP URI, the information on the URI is most important, it overrides the other fields below, if any */
    const char                  *host;               /*!< Domain or IP as string */
    int                         port;                /*!< Port to connect, default depend on esp_http_client_transport_t (80 or 443) */
    const char                  *username;           /*!< Using for Http authentication */
    const char                  *password;           /*!< Using for Http authentication */
    esp_http_client_auth_type_t auth_type;           /*!< Http authentication type, see `esp_http_client_auth_type_t` */
    const char                  *path;               /*!< HTTP Path, if not set, default is `/` */
    const char                  *query;              /*!< HTTP query */
    const char                  *cert_pem;           /*!< SSL Certification, PEM format as string, if the client requires to verify server */
    esp_http_client_method_t    method;                   /*!< HTTP Method */
    int                         timeout_ms;               /*!< Network timeout in milliseconds */
    bool                        disable_auto_redirect;    /*!< Disable HTTP automatic redirects */
    int                         max_redirection_count;    /*!< Max redirection number */
    http_event_handle_cb        event_handle;             /*!< HTTP Event Handle */
    esp_http_client_transport_t transport_type;           /*!< HTTP transport type, see `esp_http_client_transport_t` */
    int                         buffer_size;              /*!< HTTP buffer size (both send and receive) */
    void                        *user_data;               /*!< HTTP user_data context */
} esp_http_client_config_t;


/**
 * @brief      Start a HTTP session
 *             This function must be the first function to call,
 *             and it returns a esp_http_client_handle_t that you must use as input to other functions in the interface.
 *             This call MUST have a corresponding call to esp_http_client_cleanup when the operation is complete.
 *
 * @param[in]  config   The configurations, see `http_client_config_t`
 *
 * @return
 *     - `esp_http_client_handle_t`
 *     - NULL if any errors
 */
esp_http_client_handle_t esp_http_client_init(esp_http_client_config_t *config);

/**
 * @brief      Invoke this function after `esp_http_client_init` and all the options calls are made, and will perform the
 *             transfer as described in the options. It must be called with the same esp_http_client_handle_t as input as the esp_http_client_init call returned.
 *             esp_http_client_perform performs the entire request in a blocking manner and returns when done, or if it failed.
 *             You can do any amount of calls to esp_http_client_perform while using the same esp_http_client_handle_t. The connection maybe keep if the server doesn't require it close.
 *             If you intend to transfer more than one file, you are even encouraged to do so.
 *             esp_http_client will then attempt to re-use the same connection for the following transfers, thus making the operations faster, less CPU intense and using less network resources.
 *             Just note that you will have to use `esp_http_client_set_**` between the invokes to set options for the following esp_http_client_perform.
 *             NOTES: You must never call this function simultaneously from two places using the same easy_handle.
 *             Let the function return first before invoking it another time.
 *             If you want parallel transfers, you must use several esp_http_client_handle_t.
 *             This function include `esp_http_client_open` -> `esp_http_client_write` -> `esp_http_client_finalize_open` -> `esp_http_client_read` (and option) `esp_http_client_close`.
 *
 * @param      client  The esp_http_client handle
 *
 * @return
 *  - ESP_OK on successful
 *  - ESP_FAIL on error
 */
esp_err_t esp_http_client_perform(esp_http_client_handle_t client);

/**
 * @brief      Set URI for client, when performing this behavior, the options in the URI will replace the old ones
 *
 * @param[in]  client  The esp_http_client handle
 * @param[in]  uri     The uri
 *
 * @return
 *  - ESP_OK
 *  - ESP_FAIL
 */
esp_err_t esp_http_client_set_uri(esp_http_client_handle_t client, const char *uri);

/**
 * @brief      Set post data, this function must be called before `esp_http_client_finalize_open` or perform
 *             Note: The data parameter passed to this function is a pointer and this function will not copy the data
 *
 * @param[in]  client  The esp_http_client handle
 * @param[in]  data    post data pointer
 * @param[in]  len     post length
 *
 * @return
 *  - ESP_OK
 *  - ESP_FAIL
 */
esp_err_t esp_http_client_set_post_field(esp_http_client_handle_t client, const char *data, int len);

/**
 * @brief      Get current post field information
 *
 * @param[in]  client  The client
 *
 * @return     Size of post data
 */
int esp_http_client_get_post_field(esp_http_client_handle_t client, char **data);

/**
 * @brief      Set http request header, this function must be called after esp_http_client_init and after any
 *             perform function
 *
 * @param[in]  client  The esp_http_client handle
 * @param[in]  key     The header key
 * @param[in]  value   The header value
 *
 * @return
 *  - ESP_OK
 *  - ESP_FAIL
 */
esp_err_t esp_http_client_set_header(esp_http_client_handle_t client, const char *key, const char *value);

/**
 * @brief      Set http reuqest method
 *
 * @param[in]  client  The esp_http_client handle
 * @param[in]  method  The method
 *
 * @return     ESP_OK
 */
esp_err_t esp_http_client_set_method(esp_http_client_handle_t client, esp_http_client_method_t method);

/**
 * @brief      Delete http request header
 *
 * @param[in]  client  The esp_http_client handle
 * @param[in]  key     The key
 *
 * @return
 *  - ESP_OK
 *  - ESP_FAIL
 */
esp_err_t esp_http_client_delete_header(esp_http_client_handle_t client, const char *key);

/**
 * @brief      Open the http connection with content length will be written. This function will be open the connection,
 *             write all header strings and return
 *
 * @param[in]  client     The esp_http_client handle
 * @param[in]  write_len  The write length
 *
 * @return
 *  - ESP_OK
 *  - ESP_FAIL
 */
esp_err_t esp_http_client_open(esp_http_client_handle_t client, int write_len);

/**
 * @brief      This function will write data to http stream has opened with esp_http_client_open
 *             And data len will be write must not larger than write_len of open function
 *
 * @param[in]  client  The esp_http_client handle
 * @param      buffer  The buffer
 * @param[in]  len     The length
 *
 * @return
 *     - (-1) if any errors
 *     - Length of data written
 */
int esp_http_client_write(esp_http_client_handle_t client, char *buffer, int len);

/**
 * @brief      This function need to call after esp_http_client_open, it will read from http stream, process all receive headers
 *
 * @param[in]  client  The esp_http_client handle
 *
 * @return
 *     - (-1) if stream doesn't contain content-length header, or chunked transfering (checked by `esp_http_client_is_chunked` response)
 *     - Download data length defined by content-length header
 */
int esp_http_client_fetch_headers(esp_http_client_handle_t client);


/**
 * @brief      Check response data is chunked, must call after `esp_http_client_finalize_open`
 *
 * @param[in]  client  The esp_http_client handle
 *
 * @return     true or false
 */
bool esp_http_client_is_chunked_response(esp_http_client_handle_t client);

/**
 * @brief      Read data from http stream
 *
 * @param[in]  client  The esp_http_client handle
 * @param      buffer  The buffer
 * @param[in]  len     The length
 *
 * @return
 *     - (-1) if any errors
 *     - Length of data was read
 */
int esp_http_client_read(esp_http_client_handle_t client, char *buffer, int len);


/**
 * @brief      Get http response status code, the valid value if this function invoke after `esp_http_client_perform` or `esp_http_client_finalize_open`
 *
 * @param[in]  client  The esp_http_client handle
 *
 * @return     Status code
 */
int esp_http_client_get_status_code(esp_http_client_handle_t client);

/**
 * @brief      Get http response content length (from header Content-Length)
 *             the valid value if this function invoke after `esp_http_client_perform` or `esp_http_client_finalize_open`
 *
 * @param[in]  client  The esp_http_client handle
 *
 * @return
 *     - (-1) Chunked transfer
 *     - Content-Length value as bytes
 */
int esp_http_client_get_content_length(esp_http_client_handle_t client);

/**
 * @brief      Close http connection, still kept all http request resources
 *
 * @param[in]  client  The esp_http_client handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t esp_http_client_close(esp_http_client_handle_t client);

/**
 * @brief      This function must be the last function to call for an session.
 *             It is the opposite of the esp_http_client_init function and must be called with the same handle as input that a esp_http_client_init call returned.
 *             This might close all connections this handle has used and possibly has kept open until now.
 *             Don't call this function if you intend to transfer more files, re-using handles is a key to good performance with esp_http_client.
 *
 * @param[in]  client  The esp_http_client handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t esp_http_client_cleanup(esp_http_client_handle_t client);



#ifdef __cplusplus
}
#endif


#endif
