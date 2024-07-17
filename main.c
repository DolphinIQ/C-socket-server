// gcc -std=c2x -Wall -g main.c -lws2_32 -o sockets | ./sockets

#include <stdio.h>
#include <stdbool.h>
#include <WinSock2.h>
#include <string.h>

// #pragma comment( lib,"ws2_32.lib" ) //Winsock Library

#define TCP_TYPE SOCK_STREAM
// AF_INET = internet ipv4

#define PORT 8888
#define BUFFER_SIZE 1024

#define STR_NOT_FOUND -1

typedef struct str {
    char * data;
    size_t len;
} str;

str cstr( char * s ) {
    str new_string = {
        .data = s,
        .len = strlen( s )
    };
    return new_string;
}

/**
 * Returns new string being s1 + s2
 */
// str str_concat( const str s1, const str s2 ) {

//     // const char * char_buf[ s1.len ];
//     str new_string = {
//         .len = s1.len + s2.len
//     };
//     size_t new_str_idx = 0;
//     for ( size_t i = 0; i < s1.len; i++, new_str_idx++ ) {
//         new_string.data[ new_str_idx ] = s1.data[ i ];
//     }
//     for ( size_t i = 0; i < s2.len; i++, new_str_idx++ ) {
//         new_string.data[ new_str_idx ] = s2.data[ i ];
//     }
    
//     return new_string;
// }

void str_print( const str string ) {

    printf( "str( %lld ): \"", string.len );
    // printf( "str( %lld ): \"", string.len );
    for ( size_t i = 0; i < string.len; i++ ) {
        putchar( string.data[ i ] );
    }
    printf( "\" \n" );
}

void str_print_text( const str string ) {

    for ( size_t i = 0; i < string.len; i++ ) {
        printf( "%c", string.data[ i ] );
    }
    printf( "\n" );
}

/**
 * Finds the index of n-th occurence of a character in a given string.
 * The n starts from 1, so its assumed that n=1 = first
 */
size_t str_find_char_nth_occurence( const str string, const char c, const size_t n ) {
    // size_t i = 0;
    size_t occurence_count = 1;

    for ( size_t i = 0; i < string.len; i++ ) {
        if ( string.data[ i ] == c ) {
            if ( occurence_count == n ) {
                return i;
            } else {
                occurence_count ++;
            }
        }
    }
    return STR_NOT_FOUND;
}

size_t str_find_char_first_occurence( const str string, const char c ) {
    size_t i = 0;
    while( string.data[ i ] != c ) {
        if ( i == string.len ) {
            return STR_NOT_FOUND; // Failed to find the character, returns -1
        }
        i++;
    }
    return i;
}


/**
 * Finds the index of n-th occurence of a character in a given string.
 * Returns STR_NOT_FOUND (-1) if fails to find the index.
 * The n starts from 1, so its assumed that n=1 = first.
 */
size_t str_find_str_nth_occurence( const str haystack, const str needle, const size_t n ) {
    // size_t i = 0;
    size_t occurence_count = 1;
    size_t needle_char_idx = 0;

    for ( size_t i = 0; i < haystack.len; i++ ) {
        if ( haystack.data[ i ] == needle.data[ needle_char_idx ] ) {
            needle_char_idx++;

            if ( needle_char_idx == needle.len ) {
                if ( occurence_count == n ) {
                    return i - needle.len + 1; // Retract the index to the start of the needle str
                } else {
                    occurence_count ++;
                    needle_char_idx = 0;
                }
            }

        } else { // Missed a letter, so reset the idx counter
            needle_char_idx = 0;
        }
    }
    return STR_NOT_FOUND;
}

void copy_str_to_str( const str * source, str * dest, const size_t length ) {

    for ( size_t i = 0; i < length; i++ ) {
        dest->data[ i ] = source->data[ i ];
    }
}

/**
 * Creates a new VIEW str into the provided source str, from index start to end (exclusive)
 */
str str_copy( const str source, const size_t start, const size_t end ) {
    // "GET " 0 3
    if ( start < 0 || end < 0 ) {
        printf( "WARNING! str_copy( str, %lld, %lld ) start or end index values are negative!", start, end );
        return source;
    }

    size_t length = end - start;
    str new_str = {
        .len = length,
        .data = source.data + start
    };
    
    return new_str;
}

/**
 * Creates a string from existing string, starting from the provided starting_string, up to the endl_char
 */
str str_copy_line_starting_from_str( const str haystack, const str starting_string, const char endl_char ) {
    // First create a string starting from the starting_string
    str new_string = str_copy(
        haystack,
        str_find_str_nth_occurence( haystack, starting_string, 1 ),
        haystack.len
    );
    // Secondly, create a string from the starting_string, up to the endl_char
    new_string = str_copy(
        new_string,
        0,
        str_find_char_nth_occurence( new_string, endl_char, 1 )
    );

    return new_string;
}

bool str_is_equal( const str stringA, const str stringB ) {
    if ( stringA.len != stringB.len ) return false;

    for ( size_t i = 0; i < stringA.len; i++ ) {
        if ( stringA.data[ i ] != stringB.data[ i ] ) return false;
    }
    return true;
}


// #define STATIC_DIR cstr("/static")
#define STATIC_DIR "static"
#define HTML_TYPE cstr("text/html")
#define CSS_TYPE cstr("text/css")
#define HTTP_HEADER_BASIC_HTML "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
#define HTTP_HEADER_BASIC_CSS "HTTP/1.1 200 OK\r\nContent-Type: text/css\r\n\r\n"
// #define HTTP_HEADER_BASIC_HTML "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n"
// #define HTTP_HEADER_BASIC_CSS "HTTP/1.1 200 OK\r\nContent-Type: text/css\r\nConnection: close\r\n\r\n"

void handle_client( SOCKET client_socket, struct sockaddr_in client_info ) {

    // Accept new connections
    // struct sockaddr_in client_info = { 0 };
    // SOCKET client_socket = accept(
    //     server_socket,
    //     (struct sockaddr *)&client_info,
    //     &address_length
    // );
    // if ( client_socket == INVALID_SOCKET ) {
    //     printf( "accept() failed with error code : %d \n" , WSAGetLastError() );
    //     exit( EXIT_FAILURE );
    // }

    char * client_ip = inet_ntoa( client_info.sin_addr );
    int client_port = ntohs( client_info.sin_port );

    printf(
        "\nNew request! Client socket {%lld}, ip: %s, port: %d \n",
        client_socket, client_ip, client_port
    );

    // Receive the http request
    // char *response;
    // char *path;
    char buffer[ BUFFER_SIZE ];
    str request = { .len = BUFFER_SIZE, .data = buffer };

    // Receive the request from the client
    int bytes_received_count = recv( client_socket, request.data, BUFFER_SIZE - 1, 0 ); // -1 for \0
    if ( bytes_received_count == SOCKET_ERROR ) {
        printf("recv failed: %d \n", WSAGetLastError());
        closesocket( client_socket );
        return;
    }
    // If the connection has been gracefully closed, the return value is zero.
    if ( bytes_received_count == 0 ) {
        printf("\n> Connection with socket {%lld} has closed \n\n", client_socket );
        closesocket( client_socket );
        return;
    }
    // printf( "> RECEIVED %d bytes: \n", bytes_received_count );
    request = str_copy( request, 0, bytes_received_count );
    // str_print( request );

    /**
     * Example of a request http header:
     * GET /home HTTP/1.1\r\n
     * Host: example.com\r\n
     * Connection: keep-alive\r\n
     * Accept: text/html\r\n
     * \r\n
     * 
     * The \r character is a carriage return (CR) character, and the \n character is a newline (LF) character.
     * In the context of HTTP and many other internet protocols, lines are typically terminated
     * with the sequence \r\n (CRLF). This convention originates from the early days of network protocols
     * and is specified in many standards, including the HTTP/1.1 specification (RFC 2616).
     */

    str request_type = str_copy( request, 0, str_find_char_first_occurence( request, ' ' ) );
    printf( "> request_type: " );
    str_print_text( request_type );

    str request_accept_type = str_copy_line_starting_from_str( request, cstr("Accept:"), '\r' );
    // printf( "> request_accept_type: " );
    // str_print( request_accept_type );


    if ( str_is_equal( request_type, cstr("GET") ) ) {

        size_t end_of_first_line = str_find_char_first_occurence( request, '\r' );
        str first_line = str_copy( request, 0, end_of_first_line );
        // printf( "> First line: " );
        // str_print( first_line );

        str request_path = str_copy(
            request,
            str_find_char_first_occurence( first_line, '/'),
            str_find_char_nth_occurence( first_line, ' ', 2 ) // Looking for the second space char
        );
        printf( "> request_path: " );
        str_print( request_path );

        bool is_css_type = ( str_find_str_nth_occurence( request_accept_type, CSS_TYPE, 1 ) != STR_NOT_FOUND );
        bool is_html_type = ( str_find_str_nth_occurence( request_accept_type, HTML_TYPE, 1 ) != STR_NOT_FOUND );
        // printf( "> is_css_type: %d \n", is_css_type );
        // printf( "> is_html_type: %d \n", is_html_type );

        if ( is_css_type ) { // send CSS

            char path[256] = STATIC_DIR; // strncat() mutates the first string arg, so it needs padding bytes
            strncat( path, request_path.data, request_path.len );

            // printf( "> > FILE PATH: \"%s\" \n", path );

            // FILE * file_ptr = fopen( path, "r" );
            // if ( file_ptr == NULL ) {
            //     // File not found, send 404 response
            //     // send_response(client_socket, "404 Not Found", "text/html", "<html><body><h1>404 Not Found</h1></body></html>");
            //     printf( "FILE \"%s\" NOT FOUND\n", path );
            //     exit( EXIT_FAILURE );
            // }
            // printf( "> SENDING FILE: \"%s\" \n", path );

            // char file_buffer[ BUFFER_SIZE ] = HTTP_HEADER_BASIC_CSS;
            // const size_t header_len = strlen( HTTP_HEADER_BASIC_CSS );

            // size_t written_bytes = fread(
            //     file_buffer + header_len,
            //     sizeof( char ),
            //     BUFFER_SIZE - header_len,
            //     file_ptr
            // );
            // // printf( "> > CSS contents: \"%s\" \n", file_buffer );

            // // Send the (http-header + file content) as response
            // send( client_socket, file_buffer, written_bytes + header_len, 0 );
            // fclose( file_ptr );

            char file_buffer[ BUFFER_SIZE ] = HTTP_HEADER_BASIC_CSS;
            char * css = 
            "body {"
                "background-color: #333;"
                "color: red;"
            "}";
            strcat( file_buffer, css );
            send( client_socket, file_buffer, strlen( file_buffer ), 0 );

        } else if ( is_html_type ) { // send HTML

            // char * response;
            str html_ext = cstr(".html");

            if ( str_is_equal( cstr("/home"), request_path ) ) {

                // response =
                //     "HTTP/1.1 200 OK\r\n"
                //     "Content-Type: text/html\r\n\r\n"
                //     "<!DOCTYPE html><html lang=\"en\"><head><title>Doc</title> <link rel=\"stylesheet\" href=\"style.css\"> </head>"
                //     "<body><h1> Welcome to the Home Page! </h1></body></html>";

                char path[256] = STATIC_DIR; // strncat() mutates the first string arg, so it needs padding bytes
                strncat( path, request_path.data, request_path.len );
                strncat( path, html_ext.data, html_ext.len );

                printf( "> SENDING FILE: \"%s\" \n", path );

                FILE * file_ptr = fopen( path, "r" );
                if ( file_ptr == NULL ) {
                    // File not found, send 404 response
                    // send_response(client_socket, "404 Not Found", "text/html", "<html><body><h1>404 Not Found</h1></body></html>");
                    printf( "FILE \"%s\" NOT FOUND\n", path );
                    exit( EXIT_FAILURE );
                }

                // char file_buffer[ BUFFER_SIZE ];
                // size_t written_bytes = fread( file_buffer, sizeof( char ), BUFFER_SIZE, file_ptr );
                char file_buffer[ BUFFER_SIZE ] = HTTP_HEADER_BASIC_HTML;
                const size_t header_len = strlen( HTTP_HEADER_BASIC_HTML );
                size_t written_bytes = fread(
                    file_buffer + header_len,
                    sizeof( char ),
                    BUFFER_SIZE - header_len,
                    file_ptr
                );
                // printf( "Read %lld bytes of html\n", written_bytes );
                // printf( "> > HTML contents: \"%s\" \n", file_buffer );


                // FILE *file = fopen( "htmlreceived.txt", "w" );
                // fputs( file_buffer, file );
                // fclose( file );


                // Send the (http-header + file content) as response
                send( client_socket, file_buffer, written_bytes + header_len, 0 );
                fclose( file_ptr );

            } else {
                // printf( "404 UNDEFINED PATH" );
                char * response =
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n\r\n"
                    "<html><body><h1> 404 Page not found :( </h1></body></html>";

                send( client_socket, response, strlen( response ), 0 );
            }

            // send( client_socket, response, strlen( response ), 0 );
        }

    } else {
        printf("Non-GET response \n");
    }

    // closesocket( client_socket );
}

int main() {

    // str_print( cstr("Hello there") );
    // str new = str_concat( cstr("Hello "), cstr("there") );
    // str_print( new );

    // char hello[100] = "hello ";
    // char there[] = "there";
    // char * string = strcat( hello, there );
    // printf( "hello: \"%s\" \n", hello );
    // printf( "there: \"%s\" \n", there );
    // printf( "string: \"%s\" \n", string );

    // return 0;

    // Winsock need initialization on Windows >:(
    #if defined WIN32
        WSADATA wsaData;
        int iResult = WSAStartup( MAKEWORD(2 ,2), &wsaData );
        if ( iResult != 0 ) {
            printf( "error at WSASturtup \n" );
            return 0;
        }
    #endif

    SOCKET server_socket = socket( AF_INET, TCP_TYPE, 0 );
    printf( "server_socket: %lld \n", server_socket );
    if ( server_socket == INVALID_SOCKET ) {
        printf( "Error creating socket() \n" );
        exit( EXIT_FAILURE );
    }

    struct sockaddr_in server_info = {
        .sin_family = AF_INET,
        .sin_addr = {
            .s_addr = INADDR_ANY
        },
        .sin_port = htons( PORT )
    };

    // Bind a socket to a particular IP address and a certain port number.
    // With this we ensure that all incoming data, which is directed towards
    // this port number, is received by this application.
    // Thus its impossible to have 2+ sockets bound to the same port.
    int address_length = sizeof( server_info );
    int bind_response = bind(
        server_socket,
        (struct sockaddr *)&server_info,
        address_length
    );
    if ( bind_response == SOCKET_ERROR ) {
        printf( "Bind failed with error code : %d \n" , WSAGetLastError() );
        exit( EXIT_FAILURE );
    }

    // Put the socket in listening mode
    // The backlog (2nd arg) defines the maximum length to which the queue of pending connections
    // for sockfd may grow. If a connection request arrives when the queue is full, the client may
    // receive an error with an indication of ECONNREFUSED.
    listen( server_socket, 3 );
    printf( "Listening for connections on port %d... \n", PORT );

    SOCKET client_socket;
    struct sockaddr_in client_info = { 0 };

    client_socket = accept(
        server_socket,
        (struct sockaddr *)&client_info,
        &address_length
    );
    if ( client_socket == INVALID_SOCKET ) {
        printf( "accept() failed with error code : %d \n" , WSAGetLastError() );
        exit( EXIT_FAILURE );
    }
    handle_client( client_socket, client_info );
    // closesocket( client_socket );

    // Serve new clients continuously FOREVER
    // while ( true ) {
    // while ( 
    //     (client_socket = accept(server_socket, (struct sockaddr *)&client_info, &address_length ))
    //     != INVALID_SOCKET
    // ) {
    //     handle_client( client_socket, client_info );
        
        // Accept new connections
        // struct sockaddr_in client_info = { 0 };
        // SOCKET client_socket = accept(
        //     server_socket,
        //     (struct sockaddr *)&client_info,
        //     &address_length
        // );
        // if ( client_socket == INVALID_SOCKET ) {
        //     printf( "accept() failed with error code : %d \n" , WSAGetLastError() );
        //     exit( EXIT_FAILURE );
        // }

        // char * client_ip = inet_ntoa( client_info.sin_addr );
        // int client_port = ntohs( client_info.sin_port );

        // printf(
        //     "New connection! client socket: %lld, ip: %s, port: %d \n",
        //     client_socket, client_ip, client_port
        // );

        // // char buffer[1024] = { 0 };
        // // int valread = read( client_socket, buffer, 1024 - 1); // -1 for null terminator
        // // printf("%s\n", buffer);

        // // Send message to client
        // char * message = "<!DOCTYPE html><html lang=\"en\"><head><title>Doc</title></head><body></body></html>";
        // send( client_socket, message, strlen( message ), 0 );

        // // closesocket( client_socket );
    // }
    // if ( client_socket == INVALID_SOCKET ) {
    //     printf( "accept() failed with error code : %d \n" , WSAGetLastError() );
    //     exit( EXIT_FAILURE );
    // }

    // Close server socket and cleanup
    closesocket( server_socket );
    WSACleanup();

    return 0;
}