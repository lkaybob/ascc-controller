#define AUTH_PATH "/api/getClassAuth?si_num=%d"
#define IN_CLRM_NUM "/api/getInClass"
#define HOST	  "13.124.5.168"
#define PORT	  3000

#define METHOD_GET 	"GET"
#define METHOD_POST 	"POST"

void apiRequest(char* requestHost, int port, char* requestUrl, char* method, char response[]);
void bleCommnd(char* requestHost, int port, char* command, char response[]);
