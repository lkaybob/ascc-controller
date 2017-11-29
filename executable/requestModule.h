#define AUTH_PATH "/api/getClassAuth?si_num=%d"
#define HOST	  "13.124.255.163"
#define PORT	  3000

#define METHOD_GET 	"GET"
#define METHOD_POST 	"POST"

void apiRequest(char* requestHost, int port, char* requestUrl, char* method, char response[]);
