#ifdef _WIN32
#define USE_SOCKETS
#include <windows.h>
#include <winsock.h>
#else
#include <unistd.h>
#include <netdb.h>
#endif
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <openssl/ui.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#if defined (_XOPEN_SOURCE_EXTENDED) && defined (__hpux)
#include <sys/byteorder.h>
#endif
#ifdef OPENSSL_USE_APPLINK
#include <openssl/applink.c>
#endif

// Define possible WEB ports.
#define HTTP_PROXY_PORT 8080
#define HTTP_PORT 80
#define HTTPS_PORT 443

#ifndef _WIN32
#define SOCKET int
#define SOCKET_ERROR -1
#endif

#define PW_MIN_LENGTH 4

typedef struct {
   int verbose_mode;
   int verify_depth;
   int always_continue;
} mydata_t;

const char *KEYFILE="client.pem";
const char *pass="password";
// Setup constants.
char const *http = "http://";
char const *https = "https://";
char const *CRLF = "\015\012";

static int mydata_index;
static BIO *err;

// extern char *encode_64(char *str, const char *eol);

// Small error handling routine, that prints out the error, plus OpenSSL
// relevant information upon why the call failed.
void berr_exit(char *str, BIO *err){
   char buffer[500];
   BIO_printf(err,"%s\n",str);
   ERR_error_string(ERR_get_error(),buffer);
   BIO_printf (err,"%s\n",buffer);
   exit(EXIT_FAILURE);
}
int password_cb(char *buf, int bufsiz, int verify, void *u){
  UI *ui = NULL;
  int res = 0;
  char *password=NULL;
  UI_METHOD *ui_method = NULL;

  ui = UI_new_method(ui_method);
  if (ui){
      int ok = 0;
      char *buff = NULL;
      int ui_flags = 0;
      char *prompt = NULL;
      UI_METHOD *ui_method = NULL;

      prompt = UI_construct_prompt(ui, "pass phrase",
                        (char *)u);
      ui_flags |= UI_INPUT_FLAG_DEFAULT_PWD;
      UI_ctrl(ui, UI_CTRL_PRINT_ERRORS, 1, 0, 0);
      ok = UI_add_input_string(ui,prompt,ui_flags,buf,PW_MIN_LENGTH,BUFSIZ-1);
      if (ok >= 0 && verify){
          buff = (char *)OPENSSL_malloc(bufsiz);
          ok = UI_add_verify_string(ui,prompt,ui_flags,buff,
                                    PW_MIN_LENGTH,BUFSIZ-1, buf);
      }
      if (ok >= 0)
          do{
             ok = UI_process(ui);
          } while (ok < 0 && UI_ctrl(ui, UI_CTRL_IS_REDOABLE, 0, 0, 0));

      if (buff){
          OPENSSL_cleanse(buff,(unsigned int)bufsiz);
          OPENSSL_free(buff);
      }
      if (ok >= 0)
          res = strlen(buf);
      if (ok == -1){
          BIO_printf(err, "User interface error\n");
          ERR_print_errors(err);
          OPENSSL_cleanse(buf,(unsigned int)bufsiz);
          res = 0;
      }
      if (ok == -2){
          BIO_printf(err,"aborted!\n");
          OPENSSL_cleanse(buf,(unsigned int)bufsiz);
          res = 0;
      }
      UI_free(ui);
      OPENSSL_free(prompt);
  }
  return res;
}

//
// This routine parses the command line to determine the remote host and port 
// where to connect to, according to most Web browsers rules. It recognizes if
// the http command contains an authentification string (Username/password).
char *parse(char *command,char **filters,
    short number_filters,
    char *which,unsigned short *port, char **userpasswd){
    char const slash = '/';
    char const star = '*';
    char *cp,*cp1,*cp2;
    static char *host;
    int j;

    cp = strstr(command,http);
    cp2 = strstr(command,https);
    if ((cp != command) && (cp2 != command)){
        fprintf(stderr,"Bad http:// command : %s\n",command);;
        exit(0);
    }
    if (cp) cp += strlen(http);
    if (cp2) cp = cp2 + strlen(https);
    if ((cp1 = strchr(cp,'@')) == NULL){
         *userpasswd = NULL;
         // host located, return string after the first single slash.
         host = cp;
    }
    else{
         *cp1++ = '\0';
         host = cp1;
         *userpasswd = cp;
         cp = cp1;
    }
    cp = strchr(host,slash);
    if (cp == NULL)
        strcpy(which,"/");
    else {
        strcpy(which,cp);
        *cp = '\0';
    }
    // Now determine the port we will use.
    // The rule is that if found in the set of filters,
    // we use port 80, otherwise port=8080.
    // The star character in an address filter must be in
    // first position.
    if (number_filters >= 0)
        *port = HTTP_PROXY_PORT; // Assume not found in the filters.
    else{
        if (strstr(command,https))
            *port=HTTPS_PORT;
        else
            *port = HTTP_PORT;
    }
    for (j=0; j< number_filters;j++){
         if (strstr(host,filters[j] + sizeof(star)) != NULL){
             if (strstr(command,https))
                 *port=HTTPS_PORT;
             else
                 *port=HTTP_PORT;
             break;
         }
    }
    return host;
}
void usage(char *str){
    fprintf(stderr,"Syntax : %s <http command> [<proxy server name>]",str);
    fprintf(stderr,"[<filter1> .. <filterN>]\n");
    fprintf(stderr,"Examples : ");
    fprintf(stderr,"%s http://www.compaq.com/\n",str);
    fprintf(stderr,"With authentification string : %s http://vouters:vouters@sos6.evt.cpqcorp.net/\n",str);
    fprintf(stderr,"With alternate HTTP port (default 80) : %s http://sos6.evt.cpqcorp.net:82/\n",str);
    fprintf(stderr,"With alternate HTTP_PROXY port (default 8080) : %s http://www.compaq.fr/ proxy:8086\n",str);
    exit(0);
}
#ifdef _WIN32
int strcasecmp(char *s1,char *s2){
   while ((*s1 != '\0') && (*s2 != '\0')){
           if (toupper(*s1) > toupper(*s2)) return 1;
           if (toupper(*s1++) < toupper(*s2++)) return -1;
   }
   return 0;
}
#endif

/* Check that the common name matches the

   host name*/

void check_cert(SSL *ssl, char *host, BIO *err){
    X509 *peer;
    char peer_CN[256];

    // Verify certificate
    if (SSL_get_verify_result(ssl) != X509_V_OK)
        berr_exit("Certificate doesn't verify",err);
    /*
     * Check the cert chain. The chain length
     * is automatically checked by OpenSSL when
     * we set the verify depth in the ctx 
     */

    /*
     * Check the common name
     */
    if ((peer=SSL_get_peer_certificate(ssl)) == NULL)
         berr_exit("Can't get peer certificate",err);
    X509_NAME_get_text_by_NID
      (X509_get_subject_name(peer),
      NID_commonName, peer_CN, 256);

    if(strcasecmp(peer_CN,host)){
       fprintf(stderr,"Peer name %s doesn't match host name %s\n",peer_CN,host);
    }
}

SOCKET tcp_connect(char *connect_str,BIO *err){
    struct hostent *hp;
    struct sockaddr_in addr;
    SOCKET sock;
    char *Webhost=connect_str;
    unsigned short port;
    char *cp;
    char error[80];

    if((cp=strrchr(connect_str,':')) == NULL){
        fprintf(stderr,"Bad syntax %s\n", connect_str);
        exit(EXIT_FAILURE);
    }
    *cp='\0';
    cp++;
    port=(unsigned short)atoi(cp);
    if(!(hp=gethostbyname(Webhost)))
       berr_exit("Couldn't resolve host",err);
    memset(&addr,0,sizeof(addr));
    addr.sin_addr=*(struct in_addr*)hp->h_addr_list[0];
    addr.sin_family=AF_INET;
    addr.sin_port=htons(port);
    if((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0){
       sprintf(error,"Couldn't create socket %s",strerror(errno));
       berr_exit(error,err);
    }
    if (connect(sock,(struct sockaddr *)&addr,sizeof(addr))<0){
        sprintf(error,"Couldn't connect socket %s",strerror(errno));
#ifndef _WIN32
        close(sock);
#else
        closesocket(sock);
#endif
        berr_exit(error,err);
    }
    return sock;
}
static int verify_callback(int preverify_ok, X509_STORE_CTX *ctx)
{
  char    buf[256];
  X509   *err_cert;
  int     err, depth;
  SSL    *ssl;
  mydata_t *mydata;

  err_cert = X509_STORE_CTX_get_current_cert(ctx);
  err = X509_STORE_CTX_get_error(ctx);
  depth = X509_STORE_CTX_get_error_depth(ctx);
  /*
   * Retrieve the pointer to the SSL of the connection currently treated
   * and the application specific data stored into the SSL object.
   */
  ssl = X509_STORE_CTX_get_ex_data(ctx, SSL_get_ex_data_X509_STORE_CTX_idx());
  mydata = SSL_get_ex_data(ssl, mydata_index);
  X509_NAME_oneline(X509_get_subject_name(err_cert), buf, 256);
  /*
   * Catch a too long certificate chain. The depth limit set using
   * SSL_CTX_set_verify_depth() is by purpose set to "limit+1" so
   * that whenever the "depth>verify_depth" condition is met, we
   * have violated the limit and want to log this error condition.
   * We must do it here, because the CHAIN_TOO_LONG error would not
   * be found explicitly; only errors introduced by cutting off the
   * additional certificates would be logged.
   */
  if (depth > mydata->verify_depth) {
      preverify_ok = 0;
      err = X509_V_ERR_CERT_CHAIN_TOO_LONG;
      X509_STORE_CTX_set_error(ctx, err);
  }
  if (!preverify_ok) {
      fprintf(stderr,"verify error:num=%d:%s:depth=%d:%s\n", err,
      X509_verify_cert_error_string(err), depth, buf);
  }
  else if (mydata->verbose_mode) {
      fprintf(stderr,"depth=%d:%s\n", depth, buf);
  }
  /*
   * At this point, err contains the last verification error. We can
   * use it for something special
   */

  if ((!preverify_ok) && (err == X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT))
  {
      X509_NAME_oneline(X509_get_issuer_name(ctx->current_cert), buf, 256);
      fprintf(stderr,"issuer= %s\n", buf);
  }
  if (mydata->always_continue)
      return 1;
  else
      return preverify_ok;
}
// This routine handle a https request. It uses a Secure Socket handle to
// communicate with the remote HTTP server.
void https_connection_handle(char *connect_str, char *tmpbuf, BIO *err){
  FILE *fp;
  SSL_CTX *ctx;
  SSL *ssl;
  X509 *x509;
  EVP_PKEY *pkey;
  BIO *cbio,*out;
  int len,verify_depth=1;
  SOCKET s;
  char *cp=tmpbuf;
  int r;
  mydata_t mydata;

  SSL_load_error_strings();
  // Initialize the SSL library.
  SSL_library_init();
  OpenSSL_add_all_ciphers();
  // Create an SSL context that will be used to communicate over TLSv1 protocol.
  // See RFC 2818.
  if ((fp = fopen(KEYFILE,"r")) == NULL){
       printf("Can't open certificate %s : %s\n",KEYFILE,strerror(errno));
       exit(EXIT_FAILURE);
  }
  if ((ctx=SSL_CTX_new(SSLv23_client_method()))==NULL)
      berr_exit("Can't initialize SSLv3 context",err);
  pkey=PEM_read_PrivateKey(fp, NULL,password_cb,(char *)KEYFILE);
  if (pkey == NULL)
     exit(EXIT_FAILURE);
  if ((x509=PEM_read_X509_AUX(fp, NULL, 0, NULL)) == NULL)
      berr_exit("Can't read certificate",err);
  if (SSL_CTX_use_PrivateKey(ctx,pkey)!= 1)
      berr_exit("Can't use Private Key",err);
  if (SSL_CTX_use_certificate(ctx,x509) != 1)
      berr_exit("Can't add extra chain cert",err);
  if (!SSL_CTX_load_verify_locations(ctx,NULL,"./"))
      berr_exit("Can't load verify locations",err);
  // Connect to remote peer either proxy or https server
  // using classic Socket API. Get the actual host name through host variable.
  s=tcp_connect(connect_str,err);
  //Connect SSL to CTX
  ssl=SSL_new(ctx);
  // Associate BIO cbio with socket s 
  cbio=BIO_new_socket((int)s,BIO_NOCLOSE);
  SSL_set_bio(ssl,cbio,cbio);
  if (!SSL_set_cipher_list(ssl,"ALL"))
      berr_exit("Can't set cipher list",err);
//  SSL_set_connect_state(ssl);
  mydata_index = SSL_get_ex_new_index(0, "mydata index", NULL, NULL, NULL);
  SSL_set_verify(ssl,SSL_VERIFY_PEER,verify_callback);
  SSL_set_verify_depth(ssl,verify_depth+1);
  mydata.verify_depth = verify_depth;
  mydata.verbose_mode=1;
  mydata.always_continue=0;
  SSL_set_ex_data(ssl, mydata_index, &mydata);

  //SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

  out = BIO_new_fp(stdout, BIO_CLOSE | BIO_FP_TEXT );
  // Establish a link with the remote SSL session using chosen transport.
  if(SSL_connect(ssl) <= 0){
     berr_exit("Error connecting to server's session",err);
  }
  check_cert(ssl,connect_str,err); 
  len=(int)strlen(cp);
  r = SSL_write(ssl,cp,len);
  for (;;){
       len = SSL_read(ssl, tmpbuf, 1024);
       if(len <= 0) break;
       BIO_write(out, tmpbuf, len);
  }
  BIO_write(out,"\n",sizeof("\n"));
  SSL_shutdown(ssl);
  BIO_free_all(cbio);
}
// This routine handles a http request. Also it uses OpenSSL BIO functions,
// the connection is unsecure. It can be expressed with a classic TCP/IP
// communication.
void http_connection_handle(char *connect_str,char *tmpbuf,BIO *err){
   BIO *cbio, *out;
   int len;
   char str[80];

   ERR_load_crypto_strings();
   cbio = BIO_new_connect(connect_str);
   out = BIO_new_fp(stdout, BIO_NOCLOSE | BIO_FP_TEXT );
   if(BIO_do_connect(cbio) <= 0) {
      sprintf(str, "Error connecting to server %s",connect_str);
      berr_exit(str,err);
   }
   BIO_puts(cbio, tmpbuf);
   for(;;) {
       len = BIO_read(cbio, tmpbuf, 1024);
       if(len <= 0) break;
       BIO_write(out, tmpbuf, len);
           //fwrite(tmpbuf,1,len,stdout);
   }
   BIO_write(out,"\n",sizeof("\n"));
   //fwrite("\n",1,sizeof("\n"),stdout);
   BIO_free(cbio);
   BIO_free(out);
}
int main(int argc, char **argv){

   char *proxy_server;
   char *host;
   char tmpbuf[1024];
   unsigned short port, http_proxy_port = HTTP_PROXY_PORT;
   char Webpage[80];
   char *userpwd;
   char *cp;
   char connect_str[80];
#ifdef _WIN32
  WORD wVersionRequested = MAKEWORD (2,0);
  WSADATA wsaData;
  int Err;
#endif

#ifdef WIN32
   CRYPTO_malloc_init();

#endif
   if (argc < 2)
       usage(argv[0]);
   proxy_server=argv[2];
   host = parse(argv[1],&argv[3],argc-3,Webpage,&port,&userpwd);
   if ((proxy_server != NULL) && (strcmp(proxy_server,""))){
        // Parse a proxy server with a colon followed by
        // the http default proxy port.
        cp = strchr(proxy_server,':');
        if (cp != NULL){
            *cp = 0;
            cp++;
            http_proxy_port = (unsigned short)atoi(cp);
        }
   }
   if ((proxy_server != NULL) && (!strcmp(proxy_server,"")))
        proxy_server = NULL;

    // Setup the HTTP command.
    strcpy(tmpbuf,"GET ");
    if (port == http_proxy_port){
        if (strstr(argv[1],https))
            strcat(tmpbuf,https);
        else
            strcat(tmpbuf,http);
        strcat(tmpbuf,host);
    }
    // did the user specified a HTTP_PORT ?
    if ((cp = strchr(host,':')) != NULL){
        *cp++ = '\0';
        port = (unsigned short)atoi(cp);
    }
    strcat(tmpbuf,Webpage);
    strcat(tmpbuf," HTTP/1.1");
    strcat(tmpbuf,CRLF);
    strcat(tmpbuf,"Host: ");
    strcat(tmpbuf,host);
    strcat(tmpbuf,CRLF);
    if (userpwd){
        strcat (tmpbuf,"Authorization: Basic ");
//        strcat (tmpbuf,encode_64(userpwd,"\0"));
        strcat(tmpbuf,CRLF);
    }
    strcat(tmpbuf,CRLF);
    sprintf(connect_str,"%s:%1u",
               proxy_server ? proxy_server : host,
               proxy_server ? http_proxy_port : port);
#ifdef _WIN32
  Err = WSAStartup(wVersionRequested,&wsaData);
  if (Err != 0){
      printf("%%BIO_HTML-F-CANTLOAD, cannot load winsock.dll");
      exit (1);
  }
  if ((LOBYTE(wsaData.wVersion) != 2) ||
     (HIBYTE (wsaData.wVersion) != 0)){
      printf ("%%BIO_HTML-F-BADVER, incorrect winsock.dll version\r\n");
      WSACleanup();
      exit (1);
  }
#endif
  err = BIO_new_fp(stderr, BIO_CLOSE | BIO_FP_TEXT );
  if (strstr(argv[1],https))
      https_connection_handle(connect_str,tmpbuf,err);
  else
      http_connection_handle(connect_str,tmpbuf,err);
#ifdef _WIN32
    WSACleanup();
#endif
}



